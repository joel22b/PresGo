#include "btc_controller.h"

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "config.h"

#include "bluenrg_lp_evb_led.h"

static const uint8_t btc_adv_handle = 0x00;
static uint8_t* btc_adv_data;
static uint16_t btc_adv_data_len = 0;

static uint8_t adv_data[] = {
  /* Advertising data: Flags AD Type */
  0x02, 0x01, 0x06,
  /* Advertising data: manufacturer specific data */
  26, //len
  AD_TYPE_MANUFACTURER_SPECIFIC_DATA,  //manufacturer type
  0x00, 0x00, //Company identifier code
  0x02,       // ID
  0x15,       //Length of the remaining payload
  0xE2, 0x0A, 0x39, 0xF4, 0x73, 0xF5, 0x4B, 0xC4, //Location UUID
  0xA1, 0x2F, 0x17, 0xD1, 0xAD, 0x07, 0xA9, 0x61,
  0x00, 0x05, // Major number
  0x00, 0x07, // Minor number
  (uint8_t)-56,         // Tx power measured at 1 m of distance (in dBm)
  15,       // Length of following AD data
  0x09,'B','U','S','C','O','N','N','E','C','T','I','O','N','1'
};

void btc_adv_init() {
	tBleStatus ret = aci_gap_set_scan_configuration(DUPLICATE_FILTER_DISABLED, SCAN_ACCEPT_ALL, LE_1M_PHY_BIT, ACTIVE_SCAN, BTC_SCAN_INTERVAL, BTC_SCAN_WINDOW);
	if (ret) {
		printf("Failed to set scan config: 0x%02X\n\r", ret);
		return;
	}

	// Create advertisement data
	btc_adv_data_len = BTC_ADV_BASE_LEN + 1 + BTC_ADV_NAME_LEN;
	btc_adv_data = malloc(btc_adv_data_len);
	// Copy in the base advertisement data
	memcpy(btc_adv_data, BTC_ADV_BASE, BTC_ADV_BASE_LEN);
	// Copy in device name data
	btc_adv_data[BTC_ADV_BASE_LEN] = BTC_ADV_NAME_START;
	memcpy(&btc_adv_data[BTC_ADV_BASE_LEN+1], BTC_ADV_NAME, BTC_ADV_NAME_LEN);

	// Configure the advertisements
	ret = aci_gap_set_advertising_configuration(
			btc_adv_handle, // Advertising handle
			GAP_MODE_GENERAL_DISCOVERABLE, // General discoverable mode
			ADV_PROP_NONE,//0x0013, // Connectable, Scannable, Legacy
			BTC_ADV_INTERVAL,
			BTC_ADV_INTERVAL,
	        ADV_CH_ALL,
	        0, NULL, // No peer address
	        ADV_NO_WHITE_LIST_USE,
	        0, // No preference for TX power
			LE_1M_PHY, // Primary_Advertising_PHY (not used for legacy adv)
	        0, // Secondary_Advertising_Max_Skip (not used for legacy adv)
			LE_1M_PHY,//LE_1M_PHY, //  Secondary_Advertising_PHY (not used for legacy adv)
	        0, // Advertising_SID (not used for legacy adv)
	        0); // No scan request notification
	if(ret) {
		printf("Failed to configure advertising: 0x%02X\n\r", ret);
		return;
	}

	ret = aci_gap_set_advertising_data(
			btc_adv_handle, // Advertising handle
			ADV_COMPLETE_DATA,
			btc_adv_data_len,
			btc_adv_data);
	if(ret) {
		printf("Failed to set advertising data: 0x%02X\n\r", ret);
		return;
	}

	btc_adv_start();

	//printf("Initialization: BLE Advertisement complete\n\r");
}

void btc_adv_start() {
	Advertising_Set_Parameters_t Advertising_Set_Parameters = {
			.Advertising_Handle = btc_adv_handle,
			.Duration = 0,
			.Max_Extended_Advertising_Events = 0,
	};
	tBleStatus ret = aci_gap_set_advertising_enable(ENABLE,1,&Advertising_Set_Parameters);
	if (ret) {
		printf("Failed to start advertising: 0x%02X\n", ret);
	}
}

void btc_adv_stop() {
	Advertising_Set_Parameters_t Advertising_Set_Parameters = {
			.Advertising_Handle = btc_adv_handle,
			.Duration = 0,
			.Max_Extended_Advertising_Events = 0,
	};
	tBleStatus ret = aci_gap_set_advertising_enable(DISABLE,1,&Advertising_Set_Parameters);
	if (ret) {
		printf("Failed to stop advertising: 0x%02X\n", ret);
	}
}

void btc_adv_scan_start() {
	tBleStatus ret = aci_gap_start_procedure(GAP_GENERAL_CONNECTION_ESTABLISHMENT_PROC, LE_1M_PHY_BIT, 0, 0);

	if (ret != BLE_STATUS_SUCCESS && ret != BLE_ERROR_COMMAND_DISALLOWED) {
		printf("Error while starting scanning: 0x%02X\n\r", ret);
		if (ret == BLE_ERROR_MEMORY_CAPACITY_EXCEEDED) {
			btc_connect_disconnect_scanning();
		}
	}
}

void btc_adv_scan_stop() {
	//printf("btc_adv_scan_stop\n\r");
	tBleStatus ret = aci_gap_terminate_proc(GAP_GENERAL_CONNECTION_ESTABLISHMENT_PROC);
	if (ret != BLE_STATUS_SUCCESS) {
		printf("Error while stopping scanning: 0x%02X\n\r", ret);
	}
}

void btc_adv_callback(uint8_t Num_Reports, Advertising_Report_t Advertising_Report[]) {
	//printf("Adv cb\n\r");
	for (uint8_t i = 0; i < Num_Reports; i++) {
		if (btc_adv_match(Advertising_Report[i].Address)) {
			//printf("Adv ");
			//BSP_COM_PrintCb();
			//printf("Good Adv found!\n\r");
			// Device found!
			//btc_adv_scan_stop();
			btc_event_t event;
			event.id = btc_event_adv_found;
			event.adv_found.addressType = Advertising_Report[i].Address_Type;
			memcpy(event.adv_found.address, Advertising_Report[i].Address, BTC_ADDRESS_LEN);
			btc_event_add(&event);
			//btc_connect_start(Advertising_Report[i].Address_Type, Advertising_Report[i].Address);
			return;
		}
	}
}

uint8_t btc_adv_match(uint8_t* addr) {
	for (uint8_t scan_adv = 0; scan_adv < BTC_CONNECTIONS_NUM; scan_adv++) {
		btc_connection_t* conn = btc_connect_get(scan_adv);
		if (conn->state != btc_connect_state_scanning) {
			continue;
		}

		uint8_t match = 1;
		for (uint8_t i = 0; i < BTC_ADDRESS_LEN; i++) {
			if (addr[i] != conn->address[i]) {
				match = 0;
				break;
			}
		}
		if (match) {
			btc_connect_get(scan_adv)->state = btc_connect_state_connecting;
			return 1;
		}
	}
	return 0;
}

/* =================================
 * Called from the system
================================= */
void hci_le_advertising_report_event(uint8_t Num_Reports,
                                     Advertising_Report_t Advertising_Report[])
{
	btc_adv_callback(Num_Reports, Advertising_Report);
}

void hci_le_extended_advertising_report_event(uint8_t Num_Reports,
                                              Extended_Advertising_Report_t Extended_Advertising_Report[])
{
  Advertising_Report_t Advertising_Report;
  Advertising_Report.Address_Type = Extended_Advertising_Report[0].Address_Type;
  memcpy(Advertising_Report.Address, Extended_Advertising_Report[0].Address, 6);
  Advertising_Report.Data_Length = Extended_Advertising_Report[0].Data_Length;
  Advertising_Report.Data = Extended_Advertising_Report[0].Data;
  Advertising_Report.RSSI = Extended_Advertising_Report[0].RSSI;
  hci_le_advertising_report_event(1, &Advertising_Report);
}
