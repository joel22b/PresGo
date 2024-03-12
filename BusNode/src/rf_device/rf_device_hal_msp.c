/*
 * rf_device_hal_msp.c
 *
 *  Created on: Feb. 12, 2024
 *      Author: Joel
 */

/**
******************************************************************************
* @file    I2C/I2C_Adv_IT/Src/rf_device_hal_msp.c
* @author  RF Application Team
* @brief   HAL MSP module.
******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT(c) 2020 STMicroelectronics.
* All rights reserved.</center></h2>
*
* This software component is licensed by ST under BSD 3-Clause license,
* the "License"; You may not use this file except in compliance with the
* License. You may obtain a copy of the License at:
*                        opensource.org/licenses/BSD-3-Clause
*
******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "distance_sensor_defs.h"
#include "rf_driver_hal.h"

//#if defined(CONFIG_DEVICE_BLUENRG_LP) || defined(CONFIG_DEVICE_BLUENRG_LPS)
//#include "bluenrg_lp_evb_config.h"
//#endif

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* External functions --------------------------------------------------------*/

/**
* Initializes the Global MSP.
*/
void HAL_MspInit(void)
{
  /* System interrupt init*/
  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, IRQ_LOW_PRIORITY );
}

/**
* @brief I2C MSP Initialization
* This function configures the hardware resources used in this example
* @param hi2c: I2C handle pointer
* @retval None
*/
void HAL_I2C_MspInit(I2C_HandleTypeDef* hi2c)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  if(hi2c->Instance==I2C1)
  {
    /* Peripheral clock enable */
    __HAL_RCC_I2C1_SCL_GPIO_CLK_ENABLE();
    __HAL_RCC_I2C1_SDA_GPIO_CLK_ENABLE();

    GPIO_InitStruct.Pin = I2C1_SCL_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = I2C1_SCL_AF;
    HAL_GPIO_Init(I2C1_SCL_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = I2C1_SDA_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = I2C1_SDA_AF;
    HAL_GPIO_Init(I2C1_SDA_PORT, &GPIO_InitStruct);

    /* Peripheral clock enable */
    __HAL_RCC_I2C1_CLK_ENABLE();

    /* I2Cx interrupt Init */
    HAL_NVIC_SetPriority(I2C1_IRQn, IRQ_LOW_PRIORITY );
    HAL_NVIC_EnableIRQ(I2C1_IRQn);
  }
  /*else if (hi2c->Instance==I2C2) {
	  // Peripheral clock enable
	      __HAL_RCC_I2C1_SCL_GPIO_CLK_ENABLE();
	      __HAL_RCC_I2C1_SDA_GPIO_CLK_ENABLE();

	      GPIO_InitStruct.Pin = I2C1_SCL_PIN;
	      GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
	      GPIO_InitStruct.Pull = GPIO_PULLUP;
	      GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	      GPIO_InitStruct.Alternate = I2C1_SCL_AF;
	      HAL_GPIO_Init(I2C1_SCL_PORT, &GPIO_InitStruct);

	      GPIO_InitStruct.Pin = I2C1_SDA_PIN;
	      GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
	      GPIO_InitStruct.Pull = GPIO_PULLUP;
	      GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	      GPIO_InitStruct.Alternate = I2C1_SDA_AF;
	      HAL_GPIO_Init(I2C1_SDA_PORT, &GPIO_InitStruct);

	      // Peripheral clock enable
	      __HAL_RCC_I2C1_CLK_ENABLE();

	      // I2Cx interrupt Init
	      HAL_NVIC_SetPriority(I2C1_IRQn, IRQ_LOW_PRIORITY );
	      HAL_NVIC_EnableIRQ(I2C1_IRQn);
  }*/
}

/**
* @brief I2C MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param hi2c: I2C handle pointer
* @retval None
*/
void HAL_I2C_MspDeInit(I2C_HandleTypeDef* hi2c)
{
  if(hi2c->Instance==I2C1)
  {
    /* Peripheral clock disable */
    __HAL_RCC_I2C1_CLK_DISABLE();

	/**I2Cx GPIO Configuration*/
    HAL_GPIO_DeInit(I2C1_SCL_PORT, I2C1_SCL_PIN);
    HAL_GPIO_DeInit(I2C1_SDA_PORT, I2C1_SDA_PIN);

    /* I2Cx interrupt DeInit */
    HAL_NVIC_DisableIRQ(I2C1_IRQn);
  }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

