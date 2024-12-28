#include "tmp117.h"
#include "driver/i2c_master.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define I2C_MASTER_TIMEOUT_MS 1000

static i2c_master_dev_handle_t i2c_dev_handle;
static const char *TAG = "TMP117";

// Initialize I2C for TMP117
void i2c_master_init() {
    i2c_master_bus_config_t bus_config = {
        .i2c_port = I2C_NUM_1,
        .sda_io_num = I2C_MASTER_SDA_IO,     // Define in project
        .scl_io_num = I2C_MASTER_SCL_IO,     // Define in project
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .flags.enable_internal_pullup = true,
    };

    i2c_master_bus_handle_t bus_handle;
    ESP_ERROR_CHECK(i2c_new_master_bus(&bus_config, &bus_handle));

    i2c_device_config_t dev_config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = TMP117_ADDR,       // TMP117 I2C address
        .scl_speed_hz = I2C_MASTER_FREQ_HZ,  // Define in project
    };

    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_config, &i2c_dev_handle));
    ESP_LOGI(TAG, "I2C master initialized successfully.");
}

// Initialize TMP117 sensor
void tmp117_init(void) {
    uint8_t config_data[] = {TMP117_CONFIG_REG, 0x02, 0x20}; // Example configuration
    ESP_ERROR_CHECK(i2c_master_transmit(i2c_dev_handle, config_data, sizeof(config_data), pdMS_TO_TICKS(I2C_MASTER_TIMEOUT_MS)));
    ESP_LOGI(TAG, "TMP117 initialized successfully.");
}

// Read raw temperature data from TMP117
int16_t tmp117_read_raw(void) {
    uint8_t reg = TMP117_TEMP_REG;
    uint8_t data[2];

    ESP_ERROR_CHECK(i2c_master_transmit(i2c_dev_handle, &reg, 1, pdMS_TO_TICKS(I2C_MASTER_TIMEOUT_MS)));
    ESP_ERROR_CHECK(i2c_master_receive(i2c_dev_handle, data, sizeof(data), pdMS_TO_TICKS(I2C_MASTER_TIMEOUT_MS)));

    return (data[0] << 8) | data[1]; // Combine MSB and LSB
}

// Convert raw temperature to Celsius
float tmp117_compensate(int16_t raw_temp) {
    return raw_temp * 0.0078125f; // Convert to Celsius
}
