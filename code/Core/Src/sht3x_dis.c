#include "sht3x_dis.h"
#include "i2c_ex.h"

void sht3x_dis_send_cmd(uint32_t cmd)
{
    LL_I2C_SetSlaveAddr(I2C1, SHT3X_DIS_ADDR);
    LL_I2C_SetTransferRequest(I2C1, LL_I2C_REQUEST_WRITE);
    LL_I2C_SetTransferSize(I2C1, 2);

    LL_I2C_GenerateStartCondition(I2C1);
    i2c1_send((uint8_t)(cmd << 8));
    i2c1_send((uint8_t)cmd);
}

uint32_t calculate_crc(uint32_t *data, uint32_t len)
{
    uint8_t crc = 0xFF;
    while (len--) {
        crc ^= *data++;

        for (unsigned int i = 0; i < 8; i++) {
            crc = crc & 0x80 ? (crc << 1) ^ 0x31 : crc << 1;
        }
    }
    return (uint32_t)crc;
}

uint32_t sht3x_dis_check_crc(uint32_t sent_crc, uint32_t *data, uint32_t len)
{
    uint32_t real_crc = calculate_crc(data, len);

    return (real_crc == sent_crc);
}

float convert_hum(uint32_t data)
{
    return 100.0f * data / 65535.0f;
}

float convert_temp(uint32_t data)
{
    return -45.0f + 175.0f * data / 65535.0f;
}

void sht3x_dis_data_processing(struct sht3x_dis_temp_hum *processed, uint32_t *buf)
{
    uint32_t temp = *buf++ << 8;
    temp &= *buf++;
    processed->crc_temp = *buf++;
    if (sht3x_dis_check_crc(processed->crc_temp, &temp, 2)) {
        processed->temp = convert_temp(temp);
    }

    uint32_t hum = *buf++ << 8;
    hum &= *buf++;
    processed->crc_hum = *buf++;
    if (sht3x_dis_check_crc(processed->crc_temp, &hum, 2)) {
        processed->hum = convert_hum(hum);
    }
}

void sht3x_dis_single_meas_no_stretching(struct sht3x_dis_temp_hum *data, enum meas_mode_single meas_mode)
{
    static uint32_t cmd = 0;
    uint32_t buf[6] = {0};
    switch (meas_mode) {
    case HIGH:
        cmd = SHT3X_DIS_CMD_SINGLE_SHOT_HIGH;
    case MEDIUM:
        cmd = SHT3X_DIS_CMD_SINGLE_SHOT_MEDIUM;
    default:
        cmd = SHT3X_DIS_CMD_SINGLE_SHOT_LOW;
    }

    sht3x_dis_send_cmd(cmd);
    LL_mDelay(1);

    LL_I2C_SetSlaveAddr(I2C1, SHT3X_DIS_ADDR);
    LL_I2C_SetTransferRequest(I2C1, LL_I2C_REQUEST_READ);
    LL_I2C_SetTransferSize(I2C1, 6);

    do {
        LL_I2C_GenerateStartCondition(I2C1);
    } while (LL_I2C_IsActiveFlag_NACK(I2C1));

    for (uint32_t i = 0; i < 6; i++) {
        LL_I2C_AcknowledgeNextData(I2C1, LL_I2C_ACK);
        while (!LL_I2C_IsActiveFlag_RXNE(I2C1)) {
        };
        buf[i] = LL_I2C_ReceiveData8(I2C1);
    }

    sht3x_dis_data_processing(data, buf);
}

void sht3x_dis_single_meas_stretching(struct sht3x_dis_temp_hum *data, enum meas_mode_single meas_mode)
{
    static uint32_t cmd = 0;
    uint32_t buf[6] = {0};
    switch (meas_mode) {
    case HIGH:
        cmd = SHT3X_DIS_CMD_SINGLE_SHOT_CLOCK_STRETCHING_HIGH;
    case MEDIUM:
        cmd = SHT3X_DIS_CMD_SINGLE_SHOT_CLOCK_STRETCHING_MEDIUM;
    default:
        cmd = SHT3X_DIS_CMD_SINGLE_SHOT_CLOCK_STRETCHING_LOW;
    }

    sht3x_dis_send_cmd(cmd);
    LL_mDelay(1);

    // TODO wait for releasing of SCL line
    sht3x_dis_data_processing(data, buf);
}

void sht3x_dis_enter_periodic_meas(enum meas_mode_periodic meas_mode)
{
    sht3x_dis_send_cmd(meas_mode);
}

uint32_t sht3x_dis_fetch_data(struct sht3x_dis_temp_hum *data)
{
    uint32_t buf[6] = {0};
    sht3x_dis_send_cmd(SHT3X_DIS_CMD_FETCH);
    sht3x_dis_data_processing(data, buf);
    return 1;
}

void sht3x_dis_stop_periodic_meas(void)
{
    sht3x_dis_send_cmd(SHT3X_DIS_CMD_STOP_PERIODIC);
}

void sht3x_dis_soft_reset(void)
{
    sht3x_dis_send_cmd(SHT3X_DIS_CMD_SOFT_RESET);
}

void sht3x_dis_enable_heater(void)
{
    sht3x_dis_send_cmd(SHT3X_DIS_CMD_HEATER_ENABLE);
}

void sht3x_dis_disable_heater(void)
{
    sht3x_dis_send_cmd(SHT3X_DIS_CMD_HEATER_DISABLE);
}

void sht3x_dis_read_status(void)
{
    uint32_t buf[3] = {0};
    sht3x_dis_send_cmd(SHT3X_DIS_CMD_READ_STATUS);
    if (sht3x_dis_check_crc(buf[2], buf, 2)) {
        // TODO parse status data and trasmit them to user;
    }
}

void sht3x_dis_clear_status(void)
{
    sht3x_dis_send_cmd(SHT3X_DIS_CMD_CLEAR_STATUS);
}
