/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    i2c.c
  * @brief   This file provides code for the configuration
  *          of the I2C instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "i2c.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* I2C1 init function */
void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  LL_I2C_InitTypeDef I2C_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
  /**I2C1 GPIO Configuration
  PB8   ------> I2C1_SCL
  PB9   ------> I2C1_SDA
  */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_8;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_1;
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LL_GPIO_PIN_9;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_1;
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* Peripheral clock enable */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C1);

  /* I2C1 interrupt Init */
  NVIC_SetPriority(I2C1_IRQn, 0);
  NVIC_EnableIRQ(I2C1_IRQn);

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */

  /** I2C Initialization
  */
  LL_I2C_DisableClockStretching(I2C1);
  LL_I2C_DisableOwnAddress2(I2C1);
  LL_I2C_DisableGeneralCall(I2C1);
  I2C_InitStruct.PeripheralMode = LL_I2C_MODE_I2C;
  I2C_InitStruct.Timing = 0x40003EFF;
  I2C_InitStruct.AnalogFilter = LL_I2C_ANALOGFILTER_ENABLE;
  I2C_InitStruct.DigitalFilter = 0;
  I2C_InitStruct.OwnAddress1 = 0;
  I2C_InitStruct.TypeAcknowledge = LL_I2C_ACK;
  I2C_InitStruct.OwnAddrSize = LL_I2C_OWNADDRESS1_7BIT;
  LL_I2C_Init(I2C1, &I2C_InitStruct);
  //LL_I2C_EnableAutoEndMode(I2C1);
  LL_I2C_SetOwnAddress2(I2C1, 0, LL_I2C_OWNADDRESS2_NOMASK);
  /* USER CODE BEGIN I2C1_Init 2 */
	LL_I2C_Enable(I2C1);
	
//	LL_I2C_EnableIT_RX(I2C1);
//  LL_I2C_EnableIT_NACK(I2C1);
//  LL_I2C_EnableIT_ERR(I2C1);
//  LL_I2C_EnableIT_STOP(I2C1);
  /* USER CODE END I2C1_Init 2 */

}

/* USER CODE BEGIN 1 */
void i2c1_send (uint8_t data)
{	
	while (!LL_I2C_IsActiveFlag_TXIS(I2C1))
	{
		if(LL_I2C_IsActiveFlag_NACK(I2C1)) return;
	}
	LL_I2C_TransmitData8(I2C1, data);
}

uint32_t i2c1_read(uint8_t *data, uint32_t addr, uint32_t nbytes)
{
	uint32_t cnt = 0;
	LL_I2C_HandleTransfer(I2C1, (addr << 1), LL_I2C_ADDRSLAVE_7BIT, nbytes, LL_I2C_MODE_AUTOEND, LL_I2C_GENERATE_START_READ);

	for (uint32_t i = 0; i < nbytes; i++){
		while(!LL_I2C_IsActiveFlag_RXNE(I2C1));
		*data++ = LL_I2C_ReceiveData8(I2C1);
		cnt++;
	}
	
	return cnt;
}

uint32_t i2c1_pointer_read(uint8_t *data, uint32_t addr, uint32_t pointer, uint32_t nbytes)
{
	uint32_t cnt = 0;
	LL_I2C_HandleTransfer(I2C1, (addr << 1), LL_I2C_ADDRSLAVE_7BIT, 1, LL_I2C_MODE_SOFTEND, LL_I2C_GENERATE_START_WRITE);
	
	while(!LL_I2C_IsActiveFlag_TXIS(I2C1));
	LL_I2C_TransmitData8(I2C1, (uint8_t)pointer);
	
	LL_I2C_HandleTransfer(I2C1, (addr << 1), LL_I2C_ADDRSLAVE_7BIT, nbytes, LL_I2C_MODE_SOFTEND, LL_I2C_GENERATE_RESTART_7BIT_READ);

	for (uint32_t i = 0; i < nbytes; i++){
		while(!LL_I2C_IsActiveFlag_RXNE(I2C1));
		*data++ = LL_I2C_ReceiveData8(I2C1);
		cnt++;
	}
	
	return cnt;
}

/* USER CODE END 1 */
