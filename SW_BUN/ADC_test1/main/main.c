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

static mcp342x_t adc1_port1;
static mcp342x_t adc2_port1;

static mcp342x_t adc1_port2;
static mcp342x_t adc2_port2;

static void ADC_task(void *arg)
{
	printf("vers1\n");
	// Clear device descriptor
	memset(&adc1_port1, 0, sizeof(adc1_port1));

	adc1_port1.channel = MCP342X_CHANNEL1;
	adc1_port1.gain = GAIN;
	adc1_port1.resolution = RESOLUTION;
	adc1_port1.mode = MCP342X_CONTINUOUS;

	memset(&adc2_port1, 0, sizeof(adc2_port1));

	adc2_port1.channel = MCP342X_CHANNEL1;
	adc2_port1.gain = GAIN;
	adc2_port1.resolution = RESOLUTION;
	adc2_port1.mode = MCP342X_CONTINUOUS;

	memset(&adc1_port2, 0, sizeof(adc1_port2));

	adc1_port1.channel = MCP342X_CHANNEL2;
	adc1_port1.gain = GAIN;
	adc1_port1.resolution = RESOLUTION;
	adc1_port1.mode = MCP342X_CONTINUOUS;

	memset(&adc2_port2, 0, sizeof(adc2_port2));

	adc2_port1.channel = MCP342X_CHANNEL2;
	adc2_port1.gain = GAIN;
	adc2_port1.resolution = RESOLUTION;
	adc2_port1.mode = MCP342X_CONTINUOUS;

	ESP_ERROR_CHECK(
			mcp342x_init_desc(&adc1_port1, ADC1_ADDRESS, I2C_PORT, GPIO_NUM_6,
					GPIO_NUM_5));
	ESP_ERROR_CHECK(
			mcp342x_init_desc(&adc2_port1, ADC2_ADDRESS, I2C_PORT, GPIO_NUM_6,
					GPIO_NUM_5));

	ESP_ERROR_CHECK(
			mcp342x_init_desc(&adc1_port2, ADC1_ADDRESS, I2C_PORT, GPIO_NUM_6,
					GPIO_NUM_5));
	ESP_ERROR_CHECK(
			mcp342x_init_desc(&adc2_port2, ADC2_ADDRESS, I2C_PORT, GPIO_NUM_6,
					GPIO_NUM_5));

	// start first conversion
	ESP_ERROR_CHECK(mcp342x_start_conversion(&adc1_port1));
	ESP_ERROR_CHECK(mcp342x_start_conversion(&adc2_port1));

	ESP_ERROR_CHECK(mcp342x_start_conversion(&adc1_port2));
	ESP_ERROR_CHECK(mcp342x_start_conversion(&adc2_port2));

	// Read data
	float volts[5];

	int wait_time= pdMS_TO_TICKS(300);

	while (1)
	{
		// Wait for conversion
		vTaskDelay( wait_time); // not sure why it was not done the standard  [/ portTICK_RATE_MS] way

		ESP_ERROR_CHECK(mcp342x_get_voltage(&adc1_port1, &volts[1], NULL));
		ESP_ERROR_CHECK(mcp342x_get_voltage(&adc2_port1, &volts[3], NULL));
		printf(
				"ADC1 Channel: %d, voltage: %0.6f     ADC2 Channel: %d, voltage: %0.6f    \n",
				adc1_port1.channel, volts[1], adc2_port1.channel, volts[3]);  //
		vTaskDelay(wait_time);

		ESP_ERROR_CHECK(mcp342x_get_voltage(&adc1_port2, &volts[2], NULL));
		ESP_ERROR_CHECK(mcp342x_get_voltage(&adc2_port2, &volts[4], NULL));
		printf(
				"ADC1 Channel: %d, voltage: %0.6f     ADC2 Channel: %d, voltage: %0.6f    \n",
				adc1_port1.channel, volts[2], adc2_port1.channel, volts[4]);
		vTaskDelay(wait_time);

	}
}

void app_main()
{
	// Init library
	ESP_ERROR_CHECK(i2cdev_init());

	// Start task
	xTaskCreatePinnedToCore(ADC_task, "ADC", configMINIMAL_STACK_SIZE * 8, NULL,
			5, NULL, APP_CPU_NUM);
}

