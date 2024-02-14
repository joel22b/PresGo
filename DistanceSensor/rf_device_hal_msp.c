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
#include "I2C_Adv_IT_main.h"

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
  if(hi2c->Instance==I2Cx)
  {
    /* Peripheral clock enable */
    __HAL_RCC_I2Cx_SCL_GPIO_CLK_ENABLE();
    __HAL_RCC_I2Cx_SDA_GPIO_CLK_ENABLE();
    
    GPIO_InitStruct.Pin = I2Cx_SCL_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = I2Cx_SCL_AF;
    HAL_GPIO_Init(I2Cx_SCL_PORT, &GPIO_InitStruct);
    
    GPIO_InitStruct.Pin = I2Cx_SDA_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = I2Cx_SDA_AF;
    HAL_GPIO_Init(I2Cx_SDA_PORT, &GPIO_InitStruct);	
    
    /* Peripheral clock enable */
    __HAL_RCC_I2Cx_CLK_ENABLE();
    
    /* I2Cx interrupt Init */
    HAL_NVIC_SetPriority(I2Cx_IRQn, IRQ_LOW_PRIORITY );
    HAL_NVIC_EnableIRQ(I2Cx_IRQn);
  }
}

/**
* @brief I2C MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param hi2c: I2C handle pointer
* @retval None
*/
void HAL_I2C_MspDeInit(I2C_HandleTypeDef* hi2c)
{
  if(hi2c->Instance==I2Cx)
  {
    /* Peripheral clock disable */
    __HAL_RCC_I2Cx_CLK_DISABLE();
    
	/**I2Cx GPIO Configuration*/
    HAL_GPIO_DeInit(I2Cx_SCL_PORT, I2Cx_SCL_PIN);
    HAL_GPIO_DeInit(I2Cx_SDA_PORT, I2Cx_SDA_PIN);
    
    /* I2Cx interrupt DeInit */
    HAL_NVIC_DisableIRQ(I2Cx_IRQn);
  }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/


