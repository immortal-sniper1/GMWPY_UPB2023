/* i2c - Simple example

   Simple I2C example that shows how to initialize I2C
   as well as reading and writing from and to registers for a sensor connected over I2C.

   The sensor used in this example is a MPU9250 inertial measurement unit.

   For other examples please check:
   https://github.com/espressif/esp-idf/tree/master/examples

   See README.md file to get detailed usage of this example.

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "esp_log.h"
#include "driver/i2c.h"
#include "mcp342x.h"
#include <string.h>


/*
#include "driver/i2c.h"
#include <mcp342x.h>
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
 */


static const char *TAG = "main_file";

#define I2C_MASTER_SCL_IO           5        //GPIO_NUM_5      /*!< GPIO number used for I2C master clock */
#define I2C_MASTER_SDA_IO           6        //GPIO_NUM_6      /*!< GPIO number used for I2C master data  */
#define I2C_MASTER_NUM              0                          /*!< I2C master i2c port number, the number of i2c peripheral interfaces available will depend on the chip */
#define I2C_MASTER_FREQ_HZ          400000                     /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS       1000



#define MCP3422_1_ADDR               0xA3
#define MCP3422_2_ADDR               0xA6

#define I2C_PORT 0     // proably redundant but i kept it here since it is the same value

#ifndef APP_CPU_NUM
#define APP_CPU_NUM PRO_CPU_NUM
#endif

#define GAIN  MCP342X_GAIN1       // +-2.048
#define CHANNEL MCP342X_CHANNEL1
#define RESOLUTION MCP342X_RES_16 // 16-bit, 15 sps

static mcp342x_t adc;



/**
 * @brief i2c master initialization
 */
static esp_err_t i2c_master_init(void)
{
    int i2c_master_port = I2C_MASTER_NUM;

    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };

    i2c_param_config(i2c_master_port, &conf);

    return i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
}





static void task(void *arg)
{
    // Clear device descriptor
    memset(&adc, 0, sizeof(adc));

    uint8_t data[2];
    ESP_ERROR_CHECK(i2c_master_init());
    ESP_LOGI(TAG, "I2C initialized successfully");

    ESP_ERROR_CHECK(mcp342x_init_desc(&adc, MCP3422_1_ADDR , I2C_PORT, CONFIG_EXAMPLE_I2C_MASTER_SDA, CONFIG_EXAMPLE_I2C_MASTER_SCL));

    adc.channel = CHANNEL;
    adc.gain = GAIN;
    adc.resolution = RESOLUTION;
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
        printf("Channel: %d, voltage: %0.4f\n", adc.channel, volts);
    }
}

void app_main()
{
    // Init library
    ESP_ERROR_CHECK(i2cdev_init());

    // Start task
    xTaskCreatePinnedToCore(task, "test", configMINIMAL_STACK_SIZE * 8, NULL, 5, NULL, APP_CPU_NUM);
}

