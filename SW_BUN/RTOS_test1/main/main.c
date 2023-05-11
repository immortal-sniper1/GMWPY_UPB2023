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




static mcp342x_t adc;


#define LED_PIN1 8
#define LED_PIN2 9

void led_blink(void *pvParams) {
    gpio_pad_select_gpio(LED_PIN1);
    gpio_set_direction (LED_PIN1,GPIO_MODE_OUTPUT);
    while (1)
    {
        gpio_set_level(LED_PIN1,0);
        vTaskDelay(200/portTICK_RATE_MS);
        gpio_set_level(LED_PIN1,1);
        vTaskDelay(400/portTICK_RATE_MS);
    }
}


void led_blink2(void *pvParams) {
    gpio_pad_select_gpio(LED_PIN2);
    gpio_set_direction (LED_PIN2,GPIO_MODE_OUTPUT);
    while (1)
    {
        gpio_set_level(LED_PIN2,0);
        vTaskDelay(1000/portTICK_RATE_MS);
        gpio_set_level(LED_PIN2,1);
        vTaskDelay(1000/portTICK_RATE_MS);
    }
}

static void task(void *arg)
{
	printf("vers1\n");
    // Clear device descriptor
    memset(&adc, 0, sizeof(adc));

    ESP_ERROR_CHECK( mcp342x_init_desc(&adc, 0x6C, I2C_PORT,  GPIO_NUM_6 , GPIO_NUM_5 ));

    adc.channel = CHANNEL;
    adc.gain = GAIN;
    //adc.resolution = RESOLUTION;
    adc.resolution = 3;
    adc.mode = MCP342X_CONTINUOUS;

    uint32_t wait_time;
    ESP_ERROR_CHECK(mcp342x_get_sample_time_us(&adc, &wait_time)); // microseconds
    wait_time = wait_time / 1000 + 1; // milliseconds

    // start first conversion
    ESP_ERROR_CHECK(mcp342x_start_conversion(&adc));

    while (1)
    {
        // Wait for conversion
        vTaskDelay(pdMS_TO_TICKS(wait_time));

        // Read data
        float volts;
        ESP_ERROR_CHECK(mcp342x_get_voltage(&adc, &volts, NULL));
        printf("Channel: %d, voltage: %0.8f\n", adc.channel, volts);
    }
}

void app_main()
{
    // Init library
    ESP_ERROR_CHECK(i2cdev_init());

    // Start task
    xTaskCreatePinnedToCore(task, "test", configMINIMAL_STACK_SIZE * 8, NULL, 5, NULL, APP_CPU_NUM);
    xTaskCreatePinnedToCore(led_blink, "test2", configMINIMAL_STACK_SIZE * 8, NULL, 5, NULL, APP_CPU_NUM);
    xTaskCreatePinnedToCore(led_blink2, "test3", configMINIMAL_STACK_SIZE * 8, NULL, 5, NULL, APP_CPU_NUM);
}

