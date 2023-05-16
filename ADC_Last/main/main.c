#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <mcp342x.h>
#include <string.h>
#include <inttypes.h>

#include <esp_idf_lib_helpers.h>
#include <esp_log.h>

#define CHECK(x) do { esp_err_t __; if ((__ = x) != ESP_OK) return __; } while (0)
#define CHECK_ARG(VAL) do { if (!(VAL)) return ESP_ERR_INVALID_ARG; } while (0)

static const char *TAG = "mcp342x";
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
/*
static mcp342x_t adc1_port1;
static mcp342x_t adc2_port1;

static mcp342x_t adc1_port2;
static mcp342x_t adc2_port2;
*/

esp_err_t my_mcp342x_oneshot_conversion(mcp342x_t *dev, int32_t *data)
{
    CHECK_ARG(dev && data);

    dev->mode = MCP342X_ONESHOT;

    uint32_t st;
    CHECK(mcp342x_get_sample_time_us(dev, &st));
    CHECK(mcp342x_start_conversion(dev));
    vTaskDelay(pdMS_TO_TICKS(2*st / 1000 + 1));
    bool ready;
    CHECK(mcp342x_get_data(dev, data, &ready));
    if (!ready)
    {
        ESP_LOGE(TAG, "Data not ready");
        return ESP_FAIL;
    }

    return ESP_OK;
}




static float read_adc(uint8_t addr, i2c_port_t port,  mcp342x_channel_t channel, gpio_num_t sda_gpio, gpio_num_t scl_gpio)
{
	static const float lsb[] = {
	    [MCP342X_RES_12] = 0.001,
	    [MCP342X_RES_14] = 0.00025,
	    [MCP342X_RES_16] = 0.0000625,
	    [MCP342X_RES_18] = 0.000015625
	};

	static const int gain_val[] = {
	    [MCP342X_GAIN1] = 1,
	    [MCP342X_GAIN2] = 2,
	    [MCP342X_GAIN4] = 4,
	    [MCP342X_GAIN8] = 8
	};

	mcp342x_t adc_dev;
	int32_t result;

	memset(&adc_dev, 0, sizeof(adc_dev));
	adc_dev.channel = channel;
	adc_dev.gain = GAIN;
	adc_dev.resolution = RESOLUTION;
	adc_dev.mode = MCP342X_ONESHOT;

	ESP_ERROR_CHECK(mcp342x_init_desc(&adc_dev, addr, port, sda_gpio, scl_gpio));

	ESP_ERROR_CHECK(my_mcp342x_oneshot_conversion(&adc_dev, &result));

	ESP_ERROR_CHECK(mcp342x_free_desc(&adc_dev));
	float v = lsb[adc_dev.resolution] * result / gain_val[adc_dev.gain];
	return v;


}


static void ADC_task(void *arg)
{
	printf("vers1\n");
	// Clear device descriptor

	while(1) {
		printf("Reading...\n");

		float adc1_channel1 = read_adc(ADC1_ADDRESS, I2C_PORT, MCP342X_CHANNEL1, GPIO_NUM_6, GPIO_NUM_5);
		float adc2_channel1 = read_adc(ADC2_ADDRESS, I2C_PORT, MCP342X_CHANNEL1, GPIO_NUM_6, GPIO_NUM_5);
		float adc1_channel2 = read_adc(ADC1_ADDRESS, I2C_PORT, MCP342X_CHANNEL2, GPIO_NUM_6, GPIO_NUM_5);
		float adc2_channel2 = read_adc(ADC2_ADDRESS, I2C_PORT, MCP342X_CHANNEL2, GPIO_NUM_6, GPIO_NUM_5);

		printf("a1c1 = %.5f\n",adc1_channel1);
		printf("a1c2 = %.5f\n",adc1_channel2);
		printf("a2c1 = %.5f\n",adc2_channel1);
		printf("a2c2 = %.5f\n",adc2_channel2);


		uint64_t time = esp_timer_get_time();
		printf("time=%" PRIu64 " ms\n", time / 1000);

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

