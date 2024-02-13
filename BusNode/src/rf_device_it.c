/**
******************************************************************************
* @file    rf_device_it.c 
* @author  AMS - RF Application Team
* @brief   Main Interrupt Service Routines.
*          This file provides template for all exceptions handler and
*          peripherals interrupt service routine.
******************************************************************************
* @attention
*
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
* TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
* DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
* FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
* CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*
* <h2><center>&copy; COPYRIGHT 2019 STMicroelectronics</center></h2>
******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "rf_device_it.h"
#include "ble_const.h"
#include "bluenrg_lp_stack.h"
#include "bluenrg_lp_evb_com.h"
#include "clock.h"
#include "hal_miscutil.h"
#include "crash_handler.h"
#include "rf_driver_hal_vtimer.h"

#include "distance_sensor.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M0 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  */
NOSTACK_FUNCTION(NORETURN_FUNCTION(void NMI_IRQHandler(void)))
{
  HAL_CrashHandler(__get_MSP(), NMI_SIGNATURE);  
  /* Go to infinite loop when NMI exception occurs */
  while (1)
  {}
}

/**
* @brief  This function handles Hard Fault exception.
*/
NOSTACK_FUNCTION(NORETURN_FUNCTION(void HardFault_IRQHandler(void)))
{
  HAL_CrashHandler(__get_MSP(), HARD_FAULT_SIGNATURE);  
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles SVCall exception.
  */
void SVC_IRQHandler(void)
{
}


/**
* @brief  This function handles SysTick Handler.
*/
void SysTick_IRQHandler(void)
{
  SysCount_Handler();
  HAL_IncTick();
  HAL_SYSTICK_IRQHandler();
}


/******************************************************************************/
/*                 BLUENRG_LP Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (EXTI), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_bluenrg_lp.s).                                               */
/******************************************************************************/
/**
* @brief  This function handles UART interrupt request.
* @param  None
* @retval None
*/

void USART1_IRQHandler(void)
{  
  BSP_COM_IRQHandler();
}


/**
* @brief  This function handles DMA Handler.
*/
void DMA_IRQHandler(void)
{
  
}


/**
* @brief  This function handles GPIO interrupt request.
* @param  None
* @retval None
*/

void GPIOA_IRQHandler(void)
{  
  extern uint8_t button1_pressed;
  
  if(BSP_PB_GetITPendingBit(BSP_PUSH1) == SET) 
  {
    BSP_PB_ClearITPendingBit(BSP_PUSH1);
    button1_pressed = TRUE;
  }  
}

void GPIOB_IRQHandler(void)
{  
  extern uint8_t button2_pressed;
  
  if(BSP_PB_GetITPendingBit(BSP_PUSH2) == SET) 
  {
    BSP_PB_ClearITPendingBit(BSP_PUSH2);
    button2_pressed = TRUE;
  }  
}

void BLE_WKUP_IRQHandler(void)
{
  HAL_VTIMER_WakeUpCallback();
}

void CPU_WKUP_IRQHandler(void) 
{
  HAL_VTIMER_TimeoutCallback();
}

void BLE_ERROR_IRQHandler(void)
{
  volatile uint32_t debug_cmd;
  
  BLUE->DEBUGCMDREG |= 1;

  /* If the device is configured with 
     System clock = 64 MHz and BLE clock = 16 MHz
     a register read is necessary to end fine  
     the clear interrupt register operation,
     due the AHB down converter latency */ 
  debug_cmd = BLUE->DEBUGCMDREG;
}

void BLE_TX_RX_IRQHandler(void)
{
  uint32_t blue_status = BLUE->STATUSREG;
  uint32_t blue_interrupt = BLUE->INTERRUPT1REG;

  /** clear all pending interrupts */
  BLUE->INTERRUPT1REG = blue_interrupt;

  HAL_VTIMER_EndOfRadioActivityIsr();
  BLE_STACK_RadioHandler(blue_status|blue_interrupt);
  HAL_VTIMER_RadioTimerIsr();

  /* If the device is configured with 
     System clock = 64 MHz and BLE clock = 16 MHz
     a register read is necessary to end fine  
     the clear interrupt register operation,
     due the AHB down converter latency */ 
  blue_interrupt = BLUE->INTERRUPT1REG;
}

void BLE_RXTX_SEQ_IRQHandler(void)
{
  HAL_RXTX_SEQ_IRQHandler();
}

/**
* @}
*/ 

/**
* @}
*/ 

/**
* @}
*/ 

/**
  * @brief This function handles I2Cx event interrupt.
  */
void I2Cx_IRQHandler(void)
{
  HAL_I2C_EV_IRQHandler(ds_get_i2c_handler());

  HAL_I2C_ER_IRQHandler(ds_get_i2c_handler());

}

/******************* (C) COPYRIGHT 2015 STMicroelectronics *****END OF FILE****/
