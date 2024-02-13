/*
 * init.h
 *
 *  Created on: Feb. 8, 2024
 *      Author: Joel
 */

#ifndef SRC_INIT_H_
#define SRC_INIT_H_

#include "rf_driver_hal.h"
#include "bluenrg_lp_evb_config.h"

#include "stdio.h"

/* Exported macro ------------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/
/**
* @brief  This function is executed in case of error occurrence.
* @retval None
*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private defines -----------------------------------------------------------*/

/* Configure the SDA setup, hold time and the SCL high, low period */
/* (uint32_t)0x10320309 = I2C TIMING in Fast Mode                  */
/* (uint32_t)0x00200204 = I2C TIMING in Fast Mode plus             */

/* Configure the SDA setup, hold time and the SCL high, low period
* For Fast-mode     kHz, PRESC | 0h | SCLDEL | SDADEL | SCLH | SCLL
*                          1h  | 0h |    3h  |   2h   |  03h |  09h
* timing = __LL_I2C_CONVERT_TIMINGS(0x01, 0x03, 0x02, 0x03, 0x09);
*/

/* Configure the SDA setup, hold time and the SCL high, low period
* For Fast-mode +   kHz, PRESC | 0h | SCLDEL | SDADEL | SCLH | SCLL
*                          0h  | 0h |    2h  |   0h   |  02h |  04h
*  timing = __LL_I2C_CONVERT_TIMINGS(0x00, 0x02, 0x00, 0x02, 0x04);
*/

/**
* @brief I2Cx Initialization Function
* @param None
* @retval None
*/

/**
* @brief GPIO Initialization Function
* @param None
* @retval None
*/
static void MX_GPIO_Init(void)
{

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

}

#endif /* SRC_INIT_H_ */
