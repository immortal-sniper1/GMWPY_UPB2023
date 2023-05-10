#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <mcp342x.h>
#include <string.h>

#define I2C_PORT 0

#ifndef APP_CPU_NUM
#define APP_CPU_NUM PRO_CPU_NUM
#endif

#define GAIN  MCP342X_GAIN1       // +-2.048
#define CHANNEL MCP342X_CHANNEL2
#define RESOLUTION MCP342X_RES_16 // 16-bit, 15 sps

#define ADC1_ADDRESS 0X69
#define ADC2_ADDRESS 0X6C

#define SAMPLING_RATE 1.5 // Hz  per channel


static mcp342x_t adc1;
static mcp342x_t adc2;



static void ADC_task(void *arg)
{
	printf("vers1\n");
    // Clear device descriptor
    memset(&adc1, 0, sizeof(adc1));

    ESP_ERROR_CHECK( mcp342x_init_desc(&adc1, ADC1_ADDRESS, I2C_PORT,  GPIO_NUM_6 , GPIO_NUM_5 ));
    ESP_ERROR_CHECK( mcp342x_init_desc(&adc2, ADC2_ADDRESS, I2C_PORT,  GPIO_NUM_6 , GPIO_NUM_5 ));
    /*
    adc1.channel = CHANNEL;
    adc1.gain = GAIN;
    adc1.resolution = RESOLUTION;
    adc1.mode = MCP342X_CONTINUOUS;
    */

    //adc1.channel = CHANNEL;
    adc1.gain = 0;          				//!< x1,
    adc1.resolution = 3;					//!< 18 bits, 3.75 samples per second
    adc1.mode = 0;							//!< One-shot conversion mode

   // adc2.channel = CHANNEL;
    adc2.gain = 0;          				//!< x1,
    adc2.resolution = 3;					//!< 18 bits, 3.75 samples per second
    adc2.mode = 0;							//!< One-shot conversion mode

    adc1.channel =0;
    adc2.channel =0;

    uint32_t wait_time;

    ESP_ERROR_CHECK(mcp342x_get_sample_time_us(&adc1, &wait_time)); // microseconds
    ESP_ERROR_CHECK(mcp342x_get_sample_time_us(&adc2, &wait_time)); // microseconds
    wait_time = 1.0f /SAMPLING_RATE*1000 + 1; // milliseconds
    // 1/rate to get seconds from Hz, then *1000 for ms and +1 just to be safe

    // start first conversion
    ESP_ERROR_CHECK(mcp342x_start_conversion(&adc1));
    ESP_ERROR_CHECK(mcp342x_start_conversion(&adc2));





    // Read data
    float volts[5];

    printf("ADC1 Resolution: %d      ADC2 Resolution: %d,     \n", adc1.resolution,  adc2.resolution  );


    while (1)
    {
        // Wait for conversion
        vTaskDelay(  pdMS_TO_TICKS(wait_time)  );   // not sure why it was not done the standard  [/ portTICK_RATE_MS] way

        adc1.channel =0;
        adc2.channel =0;
        ESP_ERROR_CHECK(mcp342x_get_voltage(&adc1, &volts[1], NULL));
        ESP_ERROR_CHECK(mcp342x_get_voltage(&adc2, &volts[3], NULL));
        printf("ADC1 Channel: %d, voltage: %0.6f     ADC2 Channel: %d, voltage: %0.6f    \n", adc1.channel, volts[1] , adc2.channel, volts[3]);        //
        vTaskDelay(  pdMS_TO_TICKS(wait_time)  );


        adc1.channel =1;
        adc2.channel =1;
        ESP_ERROR_CHECK(mcp342x_get_voltage(&adc1, &volts[2], NULL));
        ESP_ERROR_CHECK(mcp342x_get_voltage(&adc2, &volts[4], NULL));
        printf("ADC1 Channel: %d, voltage: %0.6f     ADC2 Channel: %d, voltage: %0.6f    \n", adc1.channel, volts[2] , adc2.channel, volts[4]);
        vTaskDelay(  pdMS_TO_TICKS(wait_time)  );



    }
}







void app_main()
{
    // Init library
    ESP_ERROR_CHECK(i2cdev_init());

    // Start task
    xTaskCreatePinnedToCore(ADC_task, "ADC", configMINIMAL_STACK_SIZE * 8, NULL, 5, NULL, APP_CPU_NUM);
}

