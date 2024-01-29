#include "i2c.h"

#define SHT3X_DIS_ADDR                                    0x44

#define SHT3X_DIS_CMD_SINGLE_SHOT_CLOCK_STRETCHING_HIGH   0x2C06
#define SHT3X_DIS_CMD_SINGLE_SHOT_CLOCK_STRETCHING_MEDIUM 0x2C0D
#define SHT3X_DIS_CMD_SINGLE_SHOT_CLOCK_STRETCHING_LOW    0x2C10

#define SHT3X_DIS_CMD_SINGLE_SHOT_HIGH                    0x2400
#define SHT3X_DIS_CMD_SINGLE_SHOT_MEDIUM                  0x240B
#define SHT3X_DIS_CMD_SINGLE_SHOT_LOW                     0x2416

#define SHT3X_DIS_CMD_PERIODIC_0_5_MPS_HIGH               0x2032
#define SHT3X_DIS_CMD_PERIODIC_0_5_MPS_MEDIUM             0x2024
#define SHT3X_DIS_CMD_PERIODIC_0_5_MPS_LOW                0x202F

#define SHT3X_DIS_CMD_PERIODIC_1_MPS_HIGH                 0x2130
#define SHT3X_DIS_CMD_PERIODIC_1_MPS_MEDIUM               0x2126
#define SHT3X_DIS_CMD_PERIODIC_1_MPS_LOW                  0x212D

#define SHT3X_DIS_CMD_PERIODIC_2_MPS_HIGH                 0x2236
#define SHT3X_DIS_CMD_PERIODIC_2_MPS_MEDIUM               0x2220
#define SHT3X_DIS_CMD_PERIODIC_2_MPS_LOW                  0x222B

#define SHT3X_DIS_CMD_PERIODIC_4_MPS_HIGH                 0x2334
#define SHT3X_DIS_CMD_PERIODIC_4_MPS_MEDIUM               0x2322
#define SHT3X_DIS_CMD_PERIODIC_4_MPS_LOW                  0x2329

#define SHT3X_DIS_CMD_PERIODIC_10_MPS_HIGH                0x2737
#define SHT3X_DIS_CMD_PERIODIC_10_MPS_MEDIUM              0x2721
#define SHT3X_DIS_CMD_PERIODIC_10_MPS_LOW                 0x272A

#define SHT3X_DIS_CMD_FETCH                               0xE000
#define SHT3X_DIS_CMD_PERIODIC_ART                        0x2B32

#define SHT3X_DIS_CMD_STOP_PERIODIC                       0x3093

#define SHT3X_DIS_CMD_SOFT_RESET                          0x30A2

#define GENERAL_CALL                                      0x0006

#define SHT3X_DIS_CMD_HEATER_ENABLE                       0x306D
#define SHT3X_DIS_CMD_HEATER_DISABLE                      0x3066

#define SHT3X_DIS_CMD_READ_STATUS                         0xF32D
#define SHT3X_DIS_CMD_CLEAR_STATUS                        0x3041

enum meas_mode_single {
    HIGH,
    MEDIUM,
    LOW
};

enum meas_mode_periodic {
    HIGH_0_5_MPS = 0x2032,
    MEDIUM_0_5_MPS = 0x2024,
    LOW_0_5_MPS = 0x202F,
    HIGH_1_MPS = 0x2130,
    MEDIUM_1_MPS = 0x2126,
    LOW_1_MPS = 0x212D,
    HIGH_2_MPS = 0x2236,
    MEDIUM_2_MPS = 0x2220,
    LOW_2_MPS = 0x222B,
    HIGH_4_MPS = 0x2334,
    MEDIUM_4_MPS = 0x2322,
    LOW_4_MPS = 0x2329,
    HIGH_10_MPS = 0x2737,
    MEDIUM_10_MPS = 0x2721,
    LOW_10_MPS0x272A,
};

struct sht3x_dis_temp_hum {
    float temp;
    uint32_t crc_temp;
    float hum;
    uint32_t crc_hum;
};

void sht3x_dis_single_meas_no_stretching(struct sht3x_dis_temp_hum *data, enum meas_mode_single meas_mode);
void sht3x_dis_single_meas_stretching(struct sht3x_dis_temp_hum *data, enum meas_mode_single meas_mode);

void sht3x_dis_enter_periodic_meas(enum meas_mode_periodic meas_mode);
uint32_t sht3x_dis_fetch_data(struct sht3x_dis_temp_hum *data);
void sht3x_dis_stop_periodic_meas(void);

void sht3x_dis_soft_reset(void);

void sht3x_dis_enable_heater(void);
void sht3x_dis_disable_heater(void);

void sht3x_dis_read_status(void);
void sht3x_dis_clear_status(void);
