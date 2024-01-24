#include "sensors.h"
#include "gpio_ex.h"

static float zs05_data[2] = {0};
static struct p_bmp180 p_bmp180 = {0};
static float lm75bd_temp = 0;
static uint32_t oss = 0;

static void init_lm75bd(void)
{
    LL_mDelay(100);

    data_pack[0].chunk_hdr.id = CHUNK_ID_TEMP;
    data_pack[0].chunk_hdr.type = DATA_TYPE_FLOAT32;
    data_pack[0].chunk_hdr.payload_sz = 4;

    chunk_cnt = 1;

    sensor_type = SENSOR_TYPE_LM75BD;
    lm75bd_read_temp();
}

static void init_zs05(void)
{
    float zs05_data[2] = {0};
    data_pack[0].chunk_hdr.id = CHUNK_ID_TEMP;
    data_pack[0].chunk_hdr.type = DATA_TYPE_FLOAT32;
    data_pack[0].chunk_hdr.payload_sz = 4;

    data_pack[1].chunk_hdr.id = CHUNK_ID_HUM;
    data_pack[1].chunk_hdr.type = DATA_TYPE_FLOAT32;
    data_pack[1].chunk_hdr.payload_sz = 4;

    chunk_cnt = 2;

    sensor_type = SENSOR_TYPE_ZS05;
}

void init_bmp180(void)
{
    data_pack[0].chunk_hdr.id = CHUNK_ID_TEMP;
    data_pack[0].chunk_hdr.type = DATA_TYPE_FLOAT32;
    data_pack[0].chunk_hdr.payload_sz = 4;
    
    data_pack[1].chunk_hdr.id = CHUNK_ID_PRESS;
    data_pack[1].chunk_hdr.type = DATA_TYPE_FLOAT32;
    data_pack[1].chunk_hdr.payload_sz = 4;
    
    chunk_cnt = 2;

    sensor_type = SENSOR_TYPE_BMP180;
    bmp180_get_cal_param(&p_bmp180);
}

void init_wetsens(void)
{
    data_pack[0].chunk_hdr.id = CHUNK_ID_WETSENS;
    data_pack[0].chunk_hdr.type = DATA_TYPE_UINT16;
    data_pack[0].chunk_hdr.payload_sz = 2;
    
    chunk_cnt = 1;

    uint16_t null = 0;
    memcpy_u8(&null, data_pack[0].data, 2);
    sensor_type = SENSOR_TYPE_WETSENS;
    
    LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_13);
}

void sensors_init(void)
{
    #ifdef LM75BD
        init_lm75bd();
	#endif
		
	#ifdef ZS05
		init_zs05();
	#endif
		
	#ifdef BMP180
        init_bmp180();
	#endif
	
	#ifdef WET_SENSOR
        init_wetsens();
	#endif
}

void sensors_measure(usart_packet p[])
{
    #ifdef LM75BD
        lm75bd_temp = lm75bd_read_temp();
        if (lm75bd_temp) 
            turn_green_on();
        else turn_green_off();
        memcpy_u8(&lm75bd_temp, p[0].data, 4);
    #endif
		
    #ifdef ZS05
        if(zs05_read(zs05_data))
            turn_green_on();
        else turn_green_off();
        memcpy_u8(&zs05_data[0], p[0].data, 4);
        memcpy_u8(&zs05_data[1], p[1].data, 4);
    #endif
    
    #ifdef BMP180
        if (bmp180_get_temp(&p_bmp180) & bmp180_get_press(&p_bmp180, oss))
        {
            turn_green_on();
            memcpy_u8(&p_bmp180.temp, p[0].data, 4);
            memcpy_u8(& p_bmp180.press, p[1].data, 4);
        }
        else turn_green_off();
    #endif
}