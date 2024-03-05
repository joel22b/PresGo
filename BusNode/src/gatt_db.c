
#include <stdio.h>
#include <string.h>
#include "ble_const.h" 
#include "bluenrg_lp_stack.h"
#include "osal.h"
#include "app_state.h"
#include "profile.h"
#include "gatt_db.h"
#include "MultipleConnections_config.h"

#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

/*
UUIDs:
D973F2E0-B19E-11E2-9E96-0800200C9A66
D973F2E1-B19E-11E2-9E96-0800200C9A66
D973F2E2-B19E-11E2-9E96-0800200C9A66
*/

// 1D14D6EE-FD63-4FA1-BFA4-8F47B42119F0
// c3e89809-caba-41e3-a916-8ff04ad65eba
#define SRVC_UUID			0xba, 0x5e, 0xd6, 0x4a, 0xf0, 0x8f, 0x16, 0xa9, 0xe3, 0x41, 0xba, 0xca, 0x09, 0x98, 0xe8, 0xc3

//#define TX_CHR_UUID			0xe5, 0x2d, 0x86, 0xc8, 0x4b, 0x50, 0x2c, 0xa1, 0x9d, 0x4a, 0x16, 0x8c, 0xb5, 0x1c, 0xb5, 0x5a
//#define RX_CHR_UUID			0xc1, 0x86, 0x2a, 0xa9, 0xf7, 0xf5, 0x30, 0xa8, 0x49, 0x47, 0x0c, 0xed, 0xec, 0x90, 0x71, 0x06

#define RX_CHR_UUID			0xe5, 0x2d, 0x86, 0xc8, 0x4b, 0x50, 0x2c, 0xa1, 0x9d, 0x4a, 0x16, 0x8c, 0xb5, 0x1c, 0xb5, 0x5a
#define TX_CHR_UUID			0xc1, 0x86, 0x2a, 0xa9, 0xf7, 0xf5, 0x30, 0xa8, 0x49, 0x47, 0x0c, 0xed, 0xec, 0x90, 0x71, 0x06

const uint8_t SerialPort_service_uuid[16] = {SRVC_UUID};
const uint8_t SerialPort_TX_char_uuid[16] = {TX_CHR_UUID};
const uint8_t SerialPort_RX_char_uuid[16] = {RX_CHR_UUID};

BLE_GATT_SRV_CCCD_DECLARE(tx, NUM_LINKS, BLE_GATT_SRV_PERM_ENCRY_WRITE,
                     BLE_GATT_SRV_OP_MODIFIED_EVT_ENABLE_FLAG);


/* Serial Port TX (notification), RX(write without response) characteristics definition */
static const ble_gatt_chr_def_t serial_chars[] = {
    {
        .properties = BLE_GATT_SRV_CHAR_PROP_NOTIFY,
        .permissions = BLE_GATT_SRV_PERM_ENCRY_READ|BLE_GATT_SRV_PERM_ENCRY_WRITE,
        .min_key_size = BLE_GATT_SRV_MAX_ENCRY_KEY_SIZE,
        .uuid = BLE_UUID_INIT_128(TX_CHR_UUID),
        .descrs = {
            .descrs_p = &BLE_GATT_SRV_CCCD_DEF_NAME(tx),
            .descr_count = 1U,
        },
    },
    {
        .properties = BLE_GATT_SRV_CHAR_PROP_WRITE | BLE_GATT_SRV_CHAR_PROP_WRITE_NO_RESP,
        .permissions = BLE_GATT_SRV_PERM_ENCRY_WRITE,
        .min_key_size = BLE_GATT_SRV_MAX_ENCRY_KEY_SIZE,
        .uuid = BLE_UUID_INIT_128(RX_CHR_UUID),
    },
};

/* Serial Port Service definition */
static const ble_gatt_srv_def_t serial_service = {
   .type = BLE_GATT_SRV_PRIMARY_SRV_TYPE,
   .uuid = BLE_UUID_INIT_128(SRVC_UUID),
   .chrs = {
       .chrs_p = (ble_gatt_chr_def_t *)serial_chars,
       .chr_count = 2U,
   },
};

uint16_t TXCharHandle, RXCharHandle;


/*******************************************************************************
* Function Name  : Add_SerialPort_Service
* Description    : Add the Serial Port service. This service has one characteristic with
*                  notify property, that is used to send data to the client.
* Input          : None
* Return         : Status.
*******************************************************************************/
uint8_t Add_SerialPort_Service(void)
{
  uint8_t ret;
  
  ret = aci_gatt_srv_add_service((ble_gatt_srv_def_t *)&serial_service);
  if (ret != BLE_STATUS_SUCCESS)
  {
    goto fail;
  }
  TXCharHandle = aci_gatt_srv_get_char_decl_handle((ble_gatt_chr_def_t *)&serial_chars[0]);
  RXCharHandle = aci_gatt_srv_get_char_decl_handle((ble_gatt_chr_def_t *)&serial_chars[1]);
  
  //printf("Serial Port Service added: TX Char Handle %04X, RX Char Handle %04X\n\r",
  //       TXCharHandle, RXCharHandle);
  
  return BLE_STATUS_SUCCESS; 

fail:
  printf("Error while adding Serial Port service.\n\r");
  return BLE_STATUS_ERROR ;
}
