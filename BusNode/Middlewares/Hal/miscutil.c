/**
******************************************************************************
* @file    miscutil.c 
* @author  AMS - RF Application Team
* @version V1.0.0
* @date    3-April-2019
* @brief   Miscellaneous utilities for radio HW
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
* <h2><center>&copy; COPYRIGHT 2017 STMicroelectronics</center></h2>
******************************************************************************
*/ 
/* Includes ------------------------------------------------------------------*/
#include "rf_driver_ll_system.h"
#include "rf_driver_ll_utils.h"
#include "rf_driver_ll_bus.h"
#include "rf_driver_ll_radio_2g4.h"
#include "rf_driver_hal_vtimer.h"
#include "system_BlueNRG_LP.h"
#include "hal_miscutil.h"
#include "bleplat.h"
#include "ble_status.h"
#include "rf_driver_ll_gpio.h"
#include "ble_controller.h"
#include "miscutil.h"
#include "bluenrg_lp_api.h"
#include "ble_const.h"
#include "osal.h"
#include "rng_manager.h"
#include "aes_manager.h"
#include "pka_manager.h"
#include "crypto.h"

/** @addtogroup BlueNRG_LP_Miscellaneous_Utilities
* @{
*/

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define PLL_ADC_CALIB_CORR  2
#define CP_ISEL_Msk         (0x07UL)
#define SYNTH0_ANA_ENG      (*(volatile uint32_t *)0x60001610)
#define SYNTHCAL3_ANA_TST   (*(volatile uint32_t *)0x600015A4)
#define LL_DUMMY_ACCESS_ADDRESS (0x00000000U)
#define LL_DTM_ACCESS_ADDRESS   (0x71764129U)

#ifndef ANTENNA_ID_BIT_SHIFT
#define ANTENNA_ID_BIT_SHIFT (0)
#endif

/* This macro can be set to avoid breaking communication by changing the function
  of pins with aci_hal_set_antenna_switch_parameters().  */
#define RESERVED_GPIOS  0x00

// TODO: to be removed
#ifndef CONFIG_NUM_MAX_LINKS
#define CONFIG_NUM_MAX_LINKS 8
#endif

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
#if defined(CONFIG_DEVICE_BLUENRG_LPS)
uint32_t SYNTH0_ANA_ENG_bak, PWR_ENGTRIM_bak;
#endif
uint8_t pa_level = DEFAULT_TX_PA_LEVEL;
uint8_t tone_started = FALSE;
/*----------------------------------------------------------------------------*/

void BLEPLAT_NvmInit(void);

void BLEPLAT_Init(void)
{
  BLEPLAT_NvmInit();
}

void BLEPLAT_MemCpy(void *Dest, const void *Src, unsigned int Size)
{
  Osal_MemCpy(Dest, Src, Size);
}

void BLEPLAT_MemSet(void *Ptr, int Value, unsigned int Size)
{
  memset(Ptr, Value, Size);
}

int BLEPLAT_MemCmp(void *S1, void *S2, unsigned int Size)
{
  return memcmp(S1, S2, Size);
}

/**
 * @brief Get Device ID, Version and Revision numbers
 */
void BLEPLAT_GetPartInfo(uint8_t *pDeviceId, uint8_t *pMajorCut, uint8_t *pMinorCut)
{
   PartInfoType partInfo;
   
   /* get partInfo */
   HAL_GetPartInfo(&partInfo);
  
  /* Set device ID */
  *pDeviceId  = partInfo.die_id;
  
  /* Set major cut  */
  *pMajorCut = partInfo.die_major; 
 
  /* Set minor cut */
  *pMinorCut = partInfo.die_cut;
}

// TODO: change uint32_t *private_key to uint8_t*
BLEPLAT_PkaStatusTypeDef BLEPLAT_PkaStartP256Key(const uint32_t *private_key, BLEPLAT_PkaFuncCb funcCb)
{
  return (BLEPLAT_PkaStatusTypeDef)PKAMGR_StartP256PublicKeyGeneration(private_key, (PKAMGR_funcCB)funcCb);
}

BLEPLAT_PkaStatusTypeDef BLEPLAT_PkaStartDHkey(uint32_t* private_key,
                                         uint32_t* public_key,
                                         BLEPLAT_PkaFuncCb funcCb)
{
  return (BLEPLAT_PkaStatusTypeDef)PKAMGR_StartP256DHkeyGeneration(private_key, public_key, (PKAMGR_funcCB)funcCb);
}

void BLEPLAT_AesEcbEncrypt(const uint32_t *plainTextData,
                           const uint32_t *key,
                           uint32_t *encryptedData)
{
  AESMGR_Encrypt(plainTextData, key, encryptedData, 0);
}

int32_t BLEPLAT_AesCMACEncryptInit(BLEPLAT_AESCMACctxTypeDef *pAESCMACctx)
{
  return AES_CMAC_Encrypt_Init((AESCMACctx_stt*)pAESCMACctx);
}

int32_t BLEPLAT_AesCMACEncryptAppend(BLEPLAT_AESCMACctxTypeDef *pAESCMACctx,
                                     const uint8_t  *pInputBuffer, 
                                     int32_t InputSize)
{
  return AES_CMAC_Encrypt_Append((AESCMACctx_stt*)pAESCMACctx, pInputBuffer, InputSize);
}

int32_t BLEPLAT_AesCMACEncryptFinish(BLEPLAT_AESCMACctxTypeDef *pAESCMACctx,
                                     uint8_t *pOutputBuffer,
                                     int32_t *pOutputSize)
{
  return AES_CMAC_Encrypt_Finish((AESCMACctx_stt*)pAESCMACctx, pOutputBuffer, pOutputSize);
}

void BLEPLAT_RngGetRandom16(uint16_t* num)
{
  uint32_t rand_num;
  
  RNGMGR_GetRandom16(&rand_num, 0);
  
  *num = rand_num;
}

void BLEPLAT_RngGetRandom32(uint32_t* num)
{
  RNGMGR_GetRandom32(num);  
}

uint8_t BLEPLAT_DBmToPALevel(int8_t TX_dBm)
{
  return HAL_DBmToPALevel(TX_dBm);
}

uint8_t BLEPLAT_DBmToPALevelGe(int8_t TX_dBm)
{
  return HAL_DBmToPALevelGe(TX_dBm);
}

int8_t BLEPLAT_PALevelToDBm(uint8_t PA_Level)
{
  return HAL_PALevelToDBm(PA_Level);                          
}

void BLEPLAT_ReadTransmitPower(int8_t *Min_Tx_Power, int8_t *Max_Tx_Power)
{
  HAL_ReadTransmitPower(Min_Tx_Power, Max_Tx_Power);
}

uint8_t BLEPLAT_GetMaxPALevel(void)
{
  return HAL_GetMaxPALevel();
}

uint8_t BLEPLAT_GetDefaultPALevel(void)
{
  return HAL_GetDefaultPALevel();
}

void BLEPLAT_SetHighPower(uint8_t enable)
{
  HAL_SetHighPower((FunctionalState)enable);
}

int8_t BLEPLAT_CalculateRSSI(void)
{
  return HAL_CalculateRSSI();
}

int8_t BLEPLAT_UpdateAvgRSSI(int8_t avg_rssi, int8_t rssi, uint8_t rssi_filter_coeff)
{
  return HAL_UpdateAvgRSSI(avg_rssi, rssi, rssi_filter_coeff);
}

/* ------------------- Section for CTE functions -----------------------------*/

void BLEPLAT_InitCTE(uint8_t smNo)
{
#if defined(CONFIG_DEVICE_BLUENRG_LPS)
  SYNTH0_ANA_ENG_bak = SYNTH0_ANA_ENG;
  PWR_ENGTRIM_bak = PWR->ENGTRIM;
  
  SYNTHCAL3_ANA_TST = 0;
  // Set RFD_PLL_CP_ISEL = 0 in RRM->SYNTH0_ANA_ENG
  SYNTH0_ANA_ENG &= ~CP_ISEL_Msk;
  PWR->ENGTRIM = 0x00000001;
  
  /* First packet does not have the fix, so it needs to be discarded. Set an
     invalid Access Address. */
  LL_RADIO_SetAccessAddress(smNo, LL_DUMMY_ACCESS_ADDRESS);
#endif
}

void BLEPLAT_DeinitCTE(void)
{
#if defined(CONFIG_DEVICE_BLUENRG_LPS)
  PWR->ENGTRIM = PWR_ENGTRIM_bak;
  SYNTH0_ANA_ENG = SYNTH0_ANA_ENG_bak;
  SYNTHCAL3_ANA_TST = 0;
#endif
}

void BLEPLAT_CalibrateCTE(uint8_t smNo)
{
#if defined(CONFIG_DEVICE_BLUENRG_LPS)
  uint32_t dac_word = RRM->SYNTHCAL4_DIG_OUT & RRM_SYNTHCAL4_DIG_OUT_MOD_REF_DAC_WORD_OUT_Msk;
  dac_word += PLL_ADC_CALIB_CORR;
  dac_word &= RRM_SYNTHCAL4_DIG_OUT_MOD_REF_DAC_WORD_OUT_Msk;  
  // Set SYNTHCAL3_ANA_TST
  SYNTHCAL3_ANA_TST = dac_word | 0x80;  
  
  /* Restore access address. */
  LL_RADIO_SetAccessAddress(smNo, LL_DTM_ACCESS_ADDRESS);
#endif  
}

void BLEPLAT_AntIdxRemap(uint8_t AntPattLen, uint8_t *pAntRamTable, const uint8_t* pAntPatt)
{
  HAL_AntIdxRemap(AntPattLen, pAntRamTable, pAntPatt);
}

uint64_t BLEPLAT_GetCurrentSysTime(void)
{
  return HAL_VTIMER_GetCurrentSysTime();
  
}
uint64_t BLEPLAT_GetFutureSysTime64(uint32_t SysTime)
{
  return HAL_VTIMER_GetFutureSysTime64(SysTime);
}

int BLEPLAT_StartTimer(BLEPLAT_TimerHandleTypeDef *TimerHandle, uint64_t Time)
{
  return HAL_VTIMER_StartTimerSysTime((VTIMER_HandleType*)TimerHandle, Time);
}

void BLEPLAT_StopTimer(BLEPLAT_TimerHandleTypeDef *TimerHandle)
{
  HAL_VTIMER_StopTimer((VTIMER_HandleType*)TimerHandle);
}

uint8_t BLEPLAT_SetRadioTimerValue(uint32_t Time, uint8_t EventType, uint8_t CalReq)
{
  return HAL_VTIMER_SetRadioTimerValue(Time, EventType, CalReq);
}

uint8_t BLEPLAT_ClearRadioTimerValue(void)
{
  return HAL_VTIMER_ClearRadioTimerValue();
}

uint64_t BLEPLAT_GetAnchorPoint(uint64_t *pCurrentSysTime)
{
  return HAL_VTIMER_GetAnchorPoint(pCurrentSysTime);
}

void BLEPLAT_SetRadioCloseTimeout(void)
{
  TIMER_SetRadioCloseTimeout();
}

uint8_t BLEPLAT_SetRadioTimerRelativeUsValue(uint32_t RelTimeoutUs, uint8_t Tx, uint8_t PLLCal)
{
  return TIMER_SetRadioTimerRelativeUsValue(RelTimeoutUs, Tx, PLLCal);
}

/* ---------- Utility functions for antenna switching ------------------------*/

struct antenna_conf_s antenna_conf = {0, ANTENNA_ID_BIT_SHIFT, 0, 0};

void HAL_AntIdxRemap(uint8_t AntPattLen, uint8_t *pAntRamTable, const uint8_t* pAntPatt)
{
#if defined(CONFIG_DEVICE_BLUENRG_LPS)
    for (uint8_t i=0; i<AntPattLen; i++)
    {
        pAntRamTable[i] = (pAntPatt[i] << antenna_conf.Antenna_ID_Shift);
    }
#endif
}

#if defined(CONFIG_DEVICE_BLUENRG_LPS)
tBleStatus aci_hal_set_antenna_switch_parameters(uint8_t Antenna_IDs,
                                                 uint8_t Antenna_ID_Shift,
                                                 uint8_t Default_Antenna_ID,
                                                 uint8_t RF_Activity_Enable)
{
    LL_GPIO_InitTypeDef GPIO_InitStruct = {
    .Pin = Antenna_IDs, // With this we assume that Antenna_IDs bitmask is equal to the pin bitmask.
    .Mode = LL_GPIO_MODE_ALTERNATE,
    .Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH,
    .OutputType = LL_GPIO_OUTPUT_PUSHPULL,
    .Pull = LL_GPIO_PULL_NO, // TODO: with sleep enabled, a pull down is probably needed.
    .Alternate = LL_GPIO_AF_6
  };
  
  if(Antenna_IDs > 0x7F || Antenna_IDs & RESERVED_GPIOS || Antenna_ID_Shift > 7 ||
     Default_Antenna_ID > 0x7F || RF_Activity_Enable > 1)
  {
    return BLE_ERROR_INVALID_HCI_CMD_PARAMS;
  }
  
  antenna_conf.Antenna_IDs = Antenna_IDs;
  antenna_conf.Antenna_ID_Shift = Antenna_ID_Shift;
  antenna_conf.Default_Antenna_ID = Default_Antenna_ID;
  antenna_conf.RF_Activity_Enable = RF_Activity_Enable;
  
  LL_RADIO_SetDefaultAntennaID(Default_Antenna_ID);
  
  if(RF_Activity_Enable)
  {
    GPIO_InitStruct.Pin |= LL_GPIO_PIN_7;
  }    
  
  LL_AHB_EnableClock(LL_AHB_PERIPH_GPIOB);
    
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  LL_GPIO_ResetOutputPin(GPIOB, Antenna_IDs);
  
  return BLE_STATUS_SUCCESS;
}
#else
tBleStatus aci_hal_set_antenna_switch_parameters(uint8_t Antenna_IDs,
                                                 uint8_t Antenna_ID_Shift,
                                                 uint8_t Default_Antenna_ID,
                                                 uint8_t RF_Activity_Enable)
{
  return BLE_ERROR_UNKNOWN_HCI_COMMAND;
}
#endif

/* ---------------------------------------------------------------------------*/

tBleStatus aci_hal_set_tx_power_level_preprocess(uint8_t En_High_Power,
                                      uint8_t PA_Level)
{
  if((PA_Level > HAL_GetMaxPALevel()) || (En_High_Power > 1))
  {
    return BLE_ERROR_INVALID_HCI_CMD_PARAMS;
  }
  
  pa_level = PA_Level;
  
  return BLE_STATUS_SUCCESS;
}

static BOOL LL_busy(void)
{
  uint8_t n_banks = ((CONFIG_NUM_MAX_LINKS-1)/8+1);
  uint8_t link_status[8];
  uint16_t link_connection_handles[8];  
  
  for(int i = 0; i < n_banks; i++)
  {
    aci_hal_get_link_status(i, link_status, link_connection_handles);
    
    for(int j = 0; j < 8; j++)
    {
      if(link_status[j] != LL_IDLE)
      {
        return TRUE;
      }
    }    
  }
  
  return FALSE;
}

tBleStatus aci_hal_tone_start(uint8_t RF_Channel, uint8_t Offset)
{
  tBleStatus status;
  
  if (tone_started)          /* already started before */
  {
    status = BLE_ERROR_COMMAND_DISALLOWED;
  }
  else
  {
    if ((RF_Channel >= 40U) || (Offset > 2U))
    {    /* channel ID must be from 0-39 */
      status = BLE_ERROR_INVALID_HCI_CMD_PARAMS;
    }
    else
    {
      if (LL_busy() == FALSE)
      {
        
#if defined(CONFIG_DEVICE_BLUENRG_LPS)
        /* Set GPIOs for antenna switch in output mode. */
        LL_GPIO_InitTypeDef GPIO_InitStruct = {
          .Pin = antenna_conf.Antenna_IDs,
          .Mode = LL_GPIO_MODE_OUTPUT,
          .Speed = LL_GPIO_SPEED_FREQ_LOW,
          .OutputType = LL_GPIO_OUTPUT_PUSHPULL,
          .Pull = LL_GPIO_PULL_NO,
        };
        
        if(antenna_conf.RF_Activity_Enable)
        {
          GPIO_InitStruct.Pin |= LL_GPIO_PIN_7;
        }
        
        /* No need to enable clock of GPIOB peripheral since
          aci_hal_set_antenna_switch_parameters() must be called to set
          pin configuration for antenna switch. */
        
        LL_GPIO_Init(GPIOB, &GPIO_InitStruct);
        
        LL_GPIO_ResetOutputPin(GPIOB, antenna_conf.Antenna_IDs);
        LL_GPIO_SetOutputPin(GPIOB, antenna_conf.Default_Antenna_ID & antenna_conf.Antenna_IDs);
        
        if(antenna_conf.RF_Activity_Enable)
        {
          /* Drive RF activity pin to enable the antenna switch. */
          LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_7);
        }        
#endif
        
        HAL_ToneStart(RF_Channel, Offset, pa_level);
        tone_started = TRUE;
        
        status = BLE_STATUS_SUCCESS;
      }else
      {
        status = BLE_ERROR_COMMAND_DISALLOWED;
      }
    }
  }
  
  return status;
}

tBleStatus aci_hal_tone_stop()
{
  tBleStatus status;
  
  if (tone_started)
  {
    HAL_ToneStop();
    tone_started = FALSE;

#if defined(CONFIG_DEVICE_BLUENRG_LPS)
    aci_hal_set_antenna_switch_parameters(antenna_conf.Antenna_IDs,
                                          antenna_conf.Antenna_ID_Shift,
                                          antenna_conf.Default_Antenna_ID,
                                          antenna_conf.RF_Activity_Enable);
#endif    
    status = BLE_STATUS_SUCCESS;
  }
  else
  {
    status = BLE_ERROR_COMMAND_DISALLOWED;
  }
  
  return status;
}
    

/** 
 *@
} */ /* End of group BlueNRG_LP_Miscellaneous_Utilities */
