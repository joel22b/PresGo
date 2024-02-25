/* Includes ------------------------------------------------------------------*/
#include <rf_device/rf_device_it.h>
#include <stdio.h>
#include <string.h>
#include "rf_driver_hal.h"
#include "ble_const.h" 
#include "bluenrg_lp_stack.h"
#include "app_state.h"
#include "profile.h"
#include "MultipleConnections_config.h"
#include "bluenrg_lp_evb_config.h"
#include "rf_driver_hal_vtimer.h"
#include "bleplat.h"
#include "nvm_db.h"
#include "pka_manager.h"
#include "rng_manager.h"
#include "aes_manager.h"
#include "ble_controller.h"

#include "init.h"
#include "protocol_serial.h"
#include "protocol_serial_types.h"
#include "btc_controller.h"
#include "distance_sensor.h"

/* External variables --------------------------------------------------------*/
uint8_t button1_pressed, button2_pressed;
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/   
/* Private macro -------------------------------------------------------------*/
NO_INIT(uint32_t dyn_alloc_a[DYNAMIC_MEMORY_SIZE>>2]);

#define DEBUG 1

#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

void ModulesInit(void)
{
  uint8_t ret;
  BLE_STACK_InitTypeDef BLE_STACK_InitParams = BLE_STACK_INIT_PARAMETERS;
  
  
  LL_AHB_EnableClock(LL_AHB_PERIPH_PKA|LL_AHB_PERIPH_RNG);

  
  BLECNTR_InitGlobal();
  
  HAL_VTIMER_InitType VTIMER_InitStruct = {HS_STARTUP_TIME, INITIAL_CALIBRATION, CALIBRATION_INTERVAL};
  HAL_VTIMER_Init(&VTIMER_InitStruct);
  
  BLEPLAT_Init();  
  if (PKAMGR_Init() == PKAMGR_ERROR)
  {
      while(1);
  }
  if (RNGMGR_Init() != RNGMGR_SUCCESS)
  {
      while(1);
  }
  
  /* Init the AES block */
  AESMGR_Init();
  
  /* BlueNRG-LP stack init */
  ret = BLE_STACK_Init(&BLE_STACK_InitParams);
  if (ret != BLE_STATUS_SUCCESS) {
    printf("Error in BLE_STACK_Init() 0x%02x\r\n", ret);
    while(1);
  }

  
}

void ModulesTick(void)
{
  /* Timer tick */
  HAL_VTIMER_Tick();
  
  /* Bluetooth stack tick */
  BLE_STACK_Tick();
  
  /* NVM manager tick */
  NVMDB_Tick();
}

 int main(void) 
{
  uint8_t ret;
  WakeupSourceConfig_TypeDef wakeupIO = {
    .RTC_enable=0,
    .LPU_enable = 0,
    .IO_Mask_High_polarity=BSP_PUSH1_WAKEUP|BSP_PUSH2_WAKEUP,
    .IO_Mask_Low_polarity=0
  };
  PowerSaveLevels stopLevel;

  /* System initialization function */
  if (SystemInit(SYSCLK_64M, BLE_SYSCLK_32M) != SUCCESS) 
  {
    /* Error during system clock configuration take appropriate action */
    while(1);
  }

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure IOs for power save modes */
  BSP_IO_Init();

  /* Configure I/O communication channel */
  BSP_COM_Init(ps_recv_callback);

  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);

  ModulesInit();
  
  /* Initialize all configured peripherals */
  MX_GPIO_Init();

  ds_init();

  BSP_PB_Init(BSP_PUSH1, BUTTON_MODE_EXTI);
  BSP_PB_Init(BSP_PUSH2, BUTTON_MODE_EXTI);
  BSP_LED_Init(BSP_LED3);

  /* Init Device */
  //ret = DeviceInit();
  //if (ret != BLE_STATUS_SUCCESS) {
  //  PRINTF("SerialPort_DeviceInit()--> Failed 0x%02x\r\n", ret);
  //  while(1);
  //}
  
  ps_init();
  btc_init();

  // Send initialization message
  uint8_t initFlags = PS_INIT_EMPTY;
  if (ds_is_init()) {
	  initFlags |= PS_INIT_DISTANCE_SENSOR;
  }
  // TODO: Check BTC init
  initFlags |= PS_INIT_BLUETOOTH_CONTROLLER;
  ps_send_ann_init(initFlags);

  while(1) {
    
    ModulesTick();
    
    /* Application tick */
    //APP_Tick();
    btc_tick();
    
    ps_process();

    ds_tick();

    /* Request to go to sleep */
    HAL_PWR_MNGR_Request(POWER_SAVE_LEVEL_RUNNING, wakeupIO, &stopLevel);
  }
  
} /* end main() */

void HAL_PWR_MNGR_WakeupIOCallback(uint32_t source)
{  
  if(source & BSP_PUSH1_WAKEUP){    
    button1_pressed = TRUE;
  }
  if(source & BSP_PUSH2_WAKEUP){
    button2_pressed = TRUE;
  }  
}

/* Event used to notify the Host that a hardware failure has occurred in the Controller. 
   See bluenrg_lp_events.h. */
void hci_hardware_error_event(uint8_t Hardware_Code)
{
  if (Hardware_Code <= 0x03)
  {
    NVIC_SystemReset();
  }
}

/**
  * This event is generated to report firmware error informations.
  * FW_Error_Type possible values: 
  * Values:
  - 0x01: L2CAP recombination failure
  - 0x02: GATT unexpected response
  - 0x03: GATT unexpected request
    After this event with error type (0x01, 0x02, 0x3) it is recommended to disconnect. 
*/
void aci_hal_fw_error_event(uint8_t FW_Error_Type,
                            uint8_t Data_Length,
                            uint8_t Data[])
{
  PRINTF("aci_hal_fw_error_event 0x%02X", FW_Error_Type);
  if (FW_Error_Type <= 0x03)
  {
    uint16_t connHandle;
    
    /* Data field is the connection handle where error has occurred */
    connHandle = LE_TO_HOST_16(Data);
    
    aci_gap_terminate(connHandle, BLE_ERROR_TERMINATED_REMOTE_USER); 
  }
}



#ifdef  USE_FULL_ASSERT

/**
* @brief  Reports the name of the source file and the source line number
*         where the assert_param error has occurred.
* @param  file: pointer to the source file name
* @param  line: assert_param error line source number
*/
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
  ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  
  /* Infinite loop */
  while (1)
  {
  }
}

#endif

/******************* (C) COPYRIGHT 2015 STMicroelectronics *****END OF FILE****/
/** \endcond
 */
