
#include <stdio.h>
#include "boards.h"
#include "app_util_platform.h"
#include "app_error.h"
#include "nrf_drv_twi.h"



#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

// ADXL372 I2C Address
#define ADXL372_I2C_ADDRESS    0x1D

// ADXL372 Registers
#define ADXL372_REG_DEVID_AD    0x00
#define ADXL372_REG_DEVID_MST   0x01
#define ADXL372_REG_PARTID      0x02
#define ADXL372_REG_REVID       0x03
#define ADXL372_REG_XDATA       0x08
#define ADXL372_REG_YDATA       0x0A
#define ADXL372_REG_ZDATA       0x0C
#define ADXL372_REG_STATUS      0x04


/* TWI instance ID. */
#if TWI0_ENABLED
#define TWI_INSTANCE_ID     0
#elif TWI1_ENABLED
#define TWI_INSTANCE_ID     1
#endif

 
 #define TWI_ADDRESSES      127

static const nrf_drv_twi_t m_twi = NRF_DRV_TWI_INSTANCE(TWI_INSTANCE_ID);

void ADXL372_write_register(uint8_t reg, uint8_t value) {
    ret_code_t err_code;
    uint8_t data[2];
    
    data[0] = reg;
    data[1] = value;
    
    err_code = nrf_drv_twi_tx(&m_twi, ADXL372_I2C_ADDRESS, data, sizeof(data), false);
    APP_ERROR_CHECK(err_code);
}

uint16_t ADXL372_read_register(uint8_t reg) {
    ret_code_t err_code;
    uint8_t data[2];

    // Send the register address to the ADXL372
    err_code = nrf_drv_twi_tx(&m_twi, ADXL372_I2C_ADDRESS, &reg, sizeof(reg), true);
    APP_ERROR_CHECK(err_code);

    // Read data from the register
    err_code = nrf_drv_twi_rx(&m_twi, ADXL372_I2C_ADDRESS, data, sizeof(data));
    APP_ERROR_CHECK(err_code);

    // Combine high and low bytes
    uint16_t value = (data[1] << 8) | data[0];
    return value;
}

void ADXL372_read_accel_data(int16_t *x, int16_t *y, int16_t *z) {
    uint8_t data[6];
    
    
    data[0] = ADXL372_read_register(ADXL372_REG_XDATA);
    data[1] = ADXL372_read_register(ADXL372_REG_XDATA + 1);
    data[2] = ADXL372_read_register(ADXL372_REG_YDATA);
    data[3] = ADXL372_read_register(ADXL372_REG_YDATA + 1);
    data[4] = ADXL372_read_register(ADXL372_REG_ZDATA);
    data[5] = ADXL372_read_register(ADXL372_REG_ZDATA + 1);
    
   
    *x = (int16_t)((data[1] << 8) | data[0]);
    *y = (int16_t)((data[3] << 8) | data[2]);
    *z = (int16_t)((data[5] << 8) | data[4]);
}


void twi_init (void)
{
    ret_code_t err_code;

    const nrf_drv_twi_config_t twi_config = {
       .scl                = ARDUINO_SCL_PIN,
       .sda                = ARDUINO_SDA_PIN,
       .frequency          = NRF_DRV_TWI_FREQ_100K,
       .interrupt_priority = APP_IRQ_PRIORITY_HIGH,
       .clear_bus_init     = false
    };

    err_code = nrf_drv_twi_init(&m_twi, &twi_config, NULL, NULL);
    APP_ERROR_CHECK(err_code);

    nrf_drv_twi_enable(&m_twi);
}


int main(void)
{
    ret_code_t err_code;
    uint8_t address;
    uint8_t sample_data;
    bool detected_device = false;

    APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
    NRF_LOG_DEFAULT_BACKENDS_INIT();

    NRF_LOG_INFO("Sensor-test-started.");
    NRF_LOG_FLUSH();
    twi_init();

    

   while (true) {
    int16_t x_accel, y_accel, z_accel;
    
    x_accel = ADXL372_read_register(ADXL372_REG_XDATA);
    y_accel = ADXL372_read_register(ADXL372_REG_YDATA);
    z_accel = ADXL372_read_register(ADXL372_REG_ZDATA);
        
    NRF_LOG_INFO("X-Axis: %d", x_accel);
    NRF_LOG_INFO("Y-Axis: %d", y_accel);
    NRF_LOG_INFO("Z-Axis: %d", z_accel);
    
    NRF_LOG_FLUSH(); // Flush the log

    // Delay for a short period before reading again
    
}
}

