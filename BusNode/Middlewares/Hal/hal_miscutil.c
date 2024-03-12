/**
******************************************************************************
* @file    hal_miscutils.c 
* @author  AMG - RF Application Team
* @version V1.1.0
* @date    3-April-2018
* @brief   Miscellaneous utilities for interfacing to  HW
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
#include "system_BlueNRG_LP.h"
#include "hal_miscutil.h"
#include "rf_driver_ll_pwr.h"
#include "rf_driver_ll_bus.h"
#include "rf_driver_ll_gpio.h"
#include "rf_driver_ll_radio_2g4.h"

#include "bluenrg_lp_evb_led.h"

NO_INIT_SECTION(crash_info_t CrashInfoRam, ".crash_info_ram_vr");

/** @addtogroup BlueNRG_LP_Miscellaneous_Utilities
* @{
*/

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define TX_POWER_LEVELS                (32U)
#define HP_TX_POWER_LEVELS             (TX_POWER_LEVELS)

#define LOWEST_TX_POWER_LEVEL_INDEX     (1U)

/** Minimum supported TX power in dBm. */
#define MIN_TX_POWER_LOW  (normal_pa_level_table[LOWEST_TX_POWER_LEVEL_INDEX]) /* high power mode disabled */
#define MIN_TX_POWER_HIGH (high_power_pa_level_table[LOWEST_TX_POWER_LEVEL_INDEX]) /* high power mode enabled */

/** Maximum supported TX power in dBm. */
#define MAX_TX_POWER_LOW  (normal_pa_level_table[TX_POWER_LEVELS-1]) /* high power mode disabled */
#define MAX_TX_POWER_HIGH (high_power_pa_level_table[HP_TX_POWER_LEVELS-1]) /* high power mode enabled */

/* Parameters of the RSSI Exponential Moving Average algorithm */ /* @todo: review */
#define MAX_RSSI_FILTER_COEFF       (4U)
#define RSSI_EMA_SMOOTH_FACTOR_BITS (3)

/* Parameters of the RSSI calculation algorithm */
#define RSSI_OFFSET  (118)

//volatile uint32_t * const RRM_TEST = &RRM->RRM_CTRL + 0x01;
#define RRM_TEST                                (*(&RRM->RRM_CTRL + 0x01))

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t high_power = FALSE;

/* Expected TX output power (dBm) for each PA level when SMPS voltage is 1.4V */
static const int8_t normal_pa_level_table[TX_POWER_LEVELS] = {
    -54, -21, -20, -19, -17, -16, -15, -14,
    -13, -12, -11, -10,  -9,  -8,  -7,  -6,
     -6,  -4,  -3,  -3,  -2,  -2,  -1,  -1,
      0,   0,   1,   2,   3,   4,   5,   6
};

/* Expected TX output power (dBm) for each PA level when SMPS voltage is 1.9V
   (high power mode). */
static const int8_t high_power_pa_level_table[HP_TX_POWER_LEVELS] = {
    -54, -19, -18, -17, -16, -15, -14, -13,
    -12, -11, -10,  -9,  -8,  -7,  -6,  -5,
     -4,  -3,  -3,  -2,  -1,   0,   1,   2,
      3,   8,   8,   8,   8,   8,   8,   8
};

/*---------------------------------------------------------------------------*/

void HAL_GetPartInfo(PartInfoType *partInfo)
{ 
  uint32_t jtag_id;
  
  partInfo->die_id = DIE_SW_ID_UNKOWN;
  
  jtag_id = LL_SYSCFG_GetDeviceJTAG_ID();
  if(jtag_id == JTAG_ID_CODE_LP)
  {
    partInfo->die_id = DIE_SW_ID_BLUENRG_LP;
  }
  else if(jtag_id == JTAG_ID_CODE_LPS)
  {
    partInfo->die_id = DIE_SW_ID_BLUENRG_LPS;
  }  
 
  
  partInfo->die_major     =  LL_SYSCFG_GetDeviceVersion(); 
  partInfo->die_cut       =  LL_SYSCFG_GetDeviceRevision(); 
  partInfo->jtag_id_code  =  LL_SYSCFG_GetDeviceJTAG_ID(); // Duplicated
  partInfo->flash_size    =  (LL_GetFlashSize() + 1) * 4;

  if (LL_GetRAMSize() != LL_UTILS_RAMSIZE_24K) {
    partInfo->ram_size    =  (LL_GetRAMSize() + 1) * 16 * 1024;
  } else {
    partInfo->ram_size    =  24*1024;
  }

}

/**
 * @brief Get Crash Information utility
 */
void HAL_GetCrashInfo(crash_info_t *crashInfo)
{
  *crashInfo = CrashInfoRam;
  /* Reset crash info value */
  CrashInfoRam.signature = 0;
}
void HAL_CrashHandler(uint32_t msp, uint32_t signature)
{
	NMI_SIGNATURE;
  volatile uint32_t * crash_info = (volatile uint32_t *)&CrashInfoRam;
  register uint32_t reg_content;
  printf("HAL Crash Handler msp=0x%08X signature=0x%08X\n\r", msp, signature);
  //BSP_LED_On(BSP_LED3);
  /* Init to zero the crash_info RAM locations */
  for (reg_content=0; reg_content<NMB_OF_EXCEP_RAM_WORD; reg_content++) {
    crash_info[reg_content] = 0;
  }
  /* Store Crash Signature */
  CrashInfoRam.signature = signature;
  /* Store SP register */
  CrashInfoRam.SP = msp;
  for (reg_content=2; reg_content<NMB_OF_EXCEP_RAM_WORD; reg_content++) {
    uint32_t *ptr = ((uint32_t *)msp)+(reg_content-2);
    if ((ptr >= ((uint32_t *)  _MEMORY_RAM_BEGIN_)) && 
        (ptr <= ((uint32_t *) _MEMORY_RAM_END_)))
      crash_info[reg_content] = *ptr;
  }
  //NVIC_SystemReset();
  while(1){}
}

uint8_t HAL_DBmToPALevel(int8_t TX_dBm)
{
  uint8_t i;
  const int8_t *pa_level_table = high_power ? high_power_pa_level_table : normal_pa_level_table;
  const uint8_t tx_power_levels = high_power ? HP_TX_POWER_LEVELS : TX_POWER_LEVELS;
  
  for(i = 0; i < tx_power_levels; i++)
  {
    if(pa_level_table[i] >= TX_dBm)
      break;
  }
  if (((pa_level_table[i] > TX_dBm) && (i > LOWEST_TX_POWER_LEVEL_INDEX)) ||
      (i == tx_power_levels))
  {
    i--;
  }
  
  return i;  
}

uint8_t HAL_DBmToPALevelGe(int8_t TX_dBm)
{
  const int8_t *pa_level_table = high_power ? high_power_pa_level_table : normal_pa_level_table;
  const uint8_t tx_power_levels = high_power ? HP_TX_POWER_LEVELS : TX_POWER_LEVELS;
  uint8_t i;
  
  for(i = LOWEST_TX_POWER_LEVEL_INDEX; i < tx_power_levels; i++)
  {
    if (pa_level_table[i] >= TX_dBm)
      break;
  }
  
  if(i == tx_power_levels)
  {
    i--;
  }
  
  return i;  
}

int8_t HAL_PALevelToDBm(uint8_t PA_Level)
{
  const int8_t *pa_level_table = high_power ? high_power_pa_level_table : normal_pa_level_table;
  const uint8_t tx_power_levels = high_power ? HP_TX_POWER_LEVELS : TX_POWER_LEVELS;
  
  if(PA_Level < LOWEST_TX_POWER_LEVEL_INDEX || PA_Level >= tx_power_levels)
  {
    return 127;
  }
  
  return pa_level_table[PA_Level];
}

void HAL_ReadTransmitPower(int8_t *Min_Tx_Power, int8_t *Max_Tx_Power)
{
    if (high_power)
    {
        *Min_Tx_Power = MIN_TX_POWER_HIGH;
        *Max_Tx_Power = MAX_TX_POWER_HIGH;
    }
    else
    {
        *Min_Tx_Power = MIN_TX_POWER_LOW;
        *Max_Tx_Power = MAX_TX_POWER_LOW;
    }
}

uint8_t HAL_GetMaxPALevel(void)
{
  return HP_TX_POWER_LEVELS - 1;
}

uint8_t HAL_GetDefaultPALevel(void)
{
  return DEFAULT_TX_PA_LEVEL;
}


void HAL_SetHighPower(FunctionalState state)
{
  if(state != DISABLE)
  {
    if(high_power == FALSE)
    {
      high_power = TRUE;    
#if defined(RRM_LDO_ANA_ENG_RFD_LDO_TRANSFO_BYPASS)      
      LL_APB0_EnableClock(LL_APB0_PERIPH_SYSCFG);
      LL_SYSCFG_BLERXTX_SetTrigger(LL_SYSCFG_BLERXTX_TRIGGER_BOTH_EDGE, LL_SYSCFG_BLE_TX_EVENT);
      LL_SYSCFG_BLERXTX_SetType(LL_SYSCFG_BLERXTX_DET_TYPE_EDGE, LL_SYSCFG_BLE_TX_EVENT);
      LL_SYSCFG_BLERXTX_EnableIT(LL_SYSCFG_BLE_TX_EVENT);
      LL_SYSCFG_BLERXTX_ClearInterrupt(LL_SYSCFG_BLE_TX_EVENT);
      NVIC_EnableIRQ(BLE_SEQ_IRQn);
#endif      
    }
  }
  else
  {
    if(high_power == TRUE)
    {
      high_power = FALSE;
#if defined(RRM_LDO_ANA_ENG_RFD_LDO_TRANSFO_BYPASS)       
      LL_SYSCFG_BLERXTX_DisableIT(LL_SYSCFG_BLE_TX_EVENT);
      NVIC_DisableIRQ(BLE_SEQ_IRQn);
      LL_SYSCFG_BLERXTX_ClearInterrupt(LL_SYSCFG_BLE_TX_EVENT);
#endif
    }
  }
  
  if (LL_PWR_IsEnabledSMPSPrechargeMode() || (LL_PWR_GetSMPSMode() == LL_PWR_NO_SMPS))
    return;

  /* Bypass SMPS */  
  LL_PWR_SetSMPSPrechargeMode(LL_PWR_SMPS_PRECHARGE);
  while(LL_PWR_IsSMPSReady());
  /* Change level */
  if(state != DISABLE)
  {
    LL_PWR_SetSMPSOutputLevel(LL_PWR_SMPS_OUTLVL_1V9);    
  }
  else
  {
    LL_PWR_SetSMPSOutputLevel(LL_PWR_SMPS_OUTLVL_1V4);
  }
  /* Disable bypass*/
  LL_PWR_SetSMPSPrechargeMode(LL_PWR_NO_SMPS_PRECHARGE);
  while(!LL_PWR_IsSMPSReady());  
}

void HAL_RXTX_SEQ_IRQHandler(void)
{
#if defined(RRM_LDO_ANA_ENG_RFD_LDO_TRANSFO_BYPASS)
  if(high_power == FALSE)
    return;

  if(LL_SYSCFG_BLERXTX_IsInterruptPending(LL_SYSCFG_BLE_TX_EVENT)){
    if(RRM->FSM_STATUS_DIG_OUT & RRM_FSM_STATUS_DIG_OUT_STATUS_4)
    {
      // Rising edge
      MODIFY_REG_FIELD(RRM->LDO_ANA_ENG, RRM_LDO_ANA_ENG_RFD_LDO_TRANSFO_BYPASS, 1);
    }
    else
    {
      // Falling edge
      MODIFY_REG_FIELD(RRM->LDO_ANA_ENG, RRM_LDO_ANA_ENG_RFD_LDO_TRANSFO_BYPASS, 0);
    }
    LL_SYSCFG_BLERXTX_ClearInterrupt(LL_SYSCFG_BLE_TX_EVENT);
  }
#endif
}

static void HAL_GetRawRSSIRegs(uint32_t *rssi_reg, uint32_t *agc_reg)
{
    volatile uint32_t rssi0 = RRM->RSSI0_DIG_OUT;
    volatile uint32_t rssi1 = RRM->RSSI1_DIG_OUT;
    
    *rssi_reg  = rssi0 & 0xFFu;
    *rssi_reg |= (rssi1 & 0xFFu) << 8;
    
    *agc_reg   = RRM->AGC_DIG_OUT;
}

int8_t HAL_CalculateRSSI(void)
{
    int32_t rssi_dbm;
    uint32_t rssi;
    uint32_t agc;
    
    HAL_GetRawRSSIRegs(&rssi, &agc);
    
    if ((rssi == 0U) || (agc > 0xbU))
    {
        rssi_dbm = RSSI_INVALID;
    }
    else
    {
        rssi_dbm = (int32_t)agc * 6 - RSSI_OFFSET;
        while (rssi > 30U)
        {
            rssi_dbm += 6;
            rssi >>= 1;
        }
        rssi_dbm += (int32_t)(uint32_t)((417U * rssi + 18080U) >> 10);
    }
    
    return (int8_t)rssi_dbm;
}

/* @todo: review with the use of linear values instead of dBm values to have more precision */
static const int8_t rssi_ema_smoothing_factor_table[MAX_RSSI_FILTER_COEFF + 1] = {
    7, 5, 3, 2, 1
};

int8_t HAL_UpdateAvgRSSI(int8_t avg_rssi, int8_t rssi, uint8_t rssi_filter_coeff)
{
    if (avg_rssi == RSSI_INVALID)
    {
        return rssi;
    }
    
    if ((rssi == RSSI_INVALID) || (rssi_filter_coeff > MAX_RSSI_FILTER_COEFF))
    {
        return avg_rssi;
    }
    
    return (avg_rssi +
            (((rssi - avg_rssi) * rssi_ema_smoothing_factor_table[rssi_filter_coeff])
             >> RSSI_EMA_SMOOTH_FACTOR_BITS));
}

/* ------------ Section for tone generation functions ------------------------*/

void HAL_ToneStart(uint8_t RF_Channel, uint8_t Offset, uint8_t PA_Level)
{
  uint8_t internal_pa_level = PA_Level;
  
  /* Calculate the synt divide factor for 16 MHz quarts and +250 kHz offset wrt the channel center frequency
  *  Algorithmically MAK = F_rf*(2^20)/35 = F_rf*(2^15)
  *  With F_rf = (2402+2*RF_Channel)+0.25 MHz
  *  K corresponds to b19-b0  of MAK
  *  A corresponds to b22-b20 of MAK
  *  M corresponds to b27-b23 of MAK
  */
  uint32_t kHz_250_scaled = 8192; /* = 0.250*2^20/32 */
  uint32_t MAK = ((2402UL + 2UL * RF_Channel) << 15);
  
  if (Offset == 1U)
  {
    MAK += kHz_250_scaled;
  }
  else if (Offset == 2U)
  {
    MAK -= kHz_250_scaled;
  }
  else
  {
    /* MISRAC2012-Rule-15.7: explicit else; no action required */
  }
  
  uint8_t M  = ((uint8_t)(MAK >> 23) & 0x1FU);
  uint8_t A  = ((uint8_t)(MAK >> 20) & 0x07U);
  uint32_t K = (MAK & 0x000FFFFFUL) + 1UL;
  
  RRM->RADIO_FSM_USR = ((uint32_t)internal_pa_level << 3) | RRM_RADIO_FSM_USR_EN_CALIB_SYNTH_Msk | RRM_RADIO_FSM_USR_EN_CALIB_CBP_Msk;
  RRM->MOD3_DIG_TST = ((uint32_t)M << 3) | ((uint32_t)A & 0x7UL);
  RRM->MOD2_DIG_TST = (K >> 12) & 0xFFU;          
  RRM->MOD1_DIG_TST = (K >> 4) & 0xFFU;
  RRM->MOD0_DIG_TST = ((K & 0x0FU) << 4) | 0x09U;
  
  /* Take control of the radio FSM through the test bus */
  RRM->DTB5_DIG_ENG = 0x09;
  RRM_TEST = 0x03;
  RRM->DTB5_DIG_ENG = 0x39;
  while (RRM->FSM_STATUS_DIG_OUT != 0x04UL);
  RRM->DTB5_DIG_ENG = 0x3B;
}

void HAL_ToneStop(void)
{
  /* Release control of the radio FSM through the test bus */
  RRM->DTB5_DIG_ENG = 0x00;
  volatile uint32_t *rrm_udra_test = &RRM->RRM_CTRL + 0x01;
  *rrm_udra_test = 0x00;
  RRM->MOD0_DIG_TST = 0;
}

/* ------------ End of section for tone generation functions ------------------------*/

/** 
 *@
} */ /* End of group BlueNRG_LP_Miscellaneous_Utilities */
