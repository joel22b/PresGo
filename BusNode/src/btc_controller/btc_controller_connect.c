#include "btc_controller.h"

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "ble_const.h"
#include "gatt_db.h"

#include "protocol_serial.h"
#include "config.h"

static btc_connection_t btc_connections[BTC_CONNECTIONS_NUM];

void btc_connect_init() {
	for (uint8_t i = 0; i < BTC_CONNECTIONS_NUM; i++) {
		btc_connect_cleanup(&btc_connections[i]);
		btc_connections[i].timer.callback = btc_connect_timeout;
		btc_connections[i].timer.userData = (void*)&(btc_connections[i]);
		//printf("Pointer address: 0x%08X\n\r", &btc_connections[i]);
	}

	/* Since we need to transfer notifications of 244 bytes in a single packet, the LL payload must be
	244 bytes for application data + 3 bytes for ATT header + 4 bytes for L2CAP header. */
	tBleStatus ret = hci_le_write_suggested_default_data_length(251, 2120);
	if (ret) {
		printf("Failed to set default data length: 0x%02X\n\r", ret);
		return;
	}

	ret = aci_gatt_srv_init();
	if (ret != BLE_STATUS_SUCCESS) {
	    printf("Error in aci_gatt_srv_init(): 0x%02x\r\n", ret);
	    return;
	}

	uint16_t service_handle;
	uint16_t dev_name_char_handle;
	uint16_t appearance_char_handle;
	ret = aci_gap_init(GAP_BROADCASTER_ROLE, 0x00, 0x08, PUBLIC_ADDR, &service_handle, &dev_name_char_handle, &appearance_char_handle);
	if (ret != BLE_STATUS_SUCCESS) {
		printf("Error in aci_gap_init() 0x%02x\r\n", ret);
	    return;
	}

	aci_gap_set_io_capability(IO_CAP_NO_INPUT_NO_OUTPUT);

	// Init GATT TODO
	ret = Add_SerialPort_Service();
	if (ret != BLE_STATUS_SUCCESS) {
	    printf("Error in Add_Serial Port_Service 0x%02x\r\n", ret);
	    return;
	}

	aci_gap_set_authentication_requirement(BONDING, MITM_PROTECTION_NOT_REQUIRED, 0, 0, 7, 16, DONOT_USE_FIXED_PIN_FOR_PAIRING, 0);

	aci_gap_configure_white_and_resolving_list(0x01);

	ret = aci_gap_set_connection_configuration(LE_1M_PHY_BIT, BTC_CONN_INTERVAL_MIN, BTC_CONN_INTERVAL_MAX, 0, BTC_SUPERVISION_TIMEOUT, BTC_CE_LENGTH, BTC_CE_LENGTH);
	if (ret) {
		printf("Failed to set connection config: 0x%02X\n\r", ret);
		return;
	}

	printf("Initialization: BLE Connect complete\n\r");
}

btc_connection_t* btc_connect_get(uint8_t index) {
	if (index >= BTC_CONNECTIONS_NUM) {
		printf("Error: get connection out of range: %d\n\r", index);
		return 0;
	}
	return &btc_connections[index];
}

btc_connection_t* btc_connect_get_connection(uint16_t connection) {
	for (uint8_t i = 0; i < BTC_CONNECTIONS_NUM; i++) {
		if (btc_connections[i].connection == connection) {
			return &btc_connections[i];
		}
	}
	return NULL;
}

void btc_connect_request(uint8_t reqId, uint8_t* addr) {
	printf("Connection requested: %d\n\r", reqId);
	for (uint8_t i = 0; i < BTC_CONNECTIONS_NUM; i++) {
		if (btc_connections[i].state == btc_connect_state_empty) {
			btc_connections[i].state = btc_connect_state_scanning;
			btc_connections[i].reqId = reqId;
			HAL_VTIMER_StartTimerMs(&btc_connections[i].timer, RSP_TIMEOUT);
			printf("Address: ");
			for (uint8_t j = 0; j < BTC_ADDRESS_LEN; j++) {
				printf("0x%02X ", addr[j]);
				btc_connections[i].address[j] = addr[j];
			}
			printf("\n\r");
			break;
		}
	}
}

void btc_connect_start(uint8_t addrType, uint8_t* addr) {
	printf("Starting connection\n\r");
	tBleStatus ret = aci_gap_create_connection(LE_1M_PHY_BIT, addrType, addr);
	if (ret != BLE_STATUS_SUCCESS) {
		printf("Failed to start connection 0x%02X\n\r", ret);
	}

	for (uint8_t i = 0; i < BTC_CONNECTIONS_NUM; i++) {
		if (btc_connections[i].state == btc_connect_state_scanning
				&& btc_address_match(btc_connections[i].address, addr)) {
			btc_connections[i].state = btc_connect_state_connecting;
			break;
		}
	}
}

void btc_connect_finish(uint16_t connection, uint8_t* addr) {
	for (uint8_t i = 0; i < BTC_CONNECTIONS_NUM; i++) {
		if (btc_connections[i].state == btc_connect_state_connecting
				&& btc_address_match(btc_connections[i].address, addr)) {
			printf("Finished creating connection\n\r");
			btc_connections[i].state = btc_connect_state_discover_config;
			btc_connections[i].connection = connection;

			tBleStatus ret = aci_gatt_clt_exchange_config(connection);
			if(ret != 0) {
				btc_connect_discover_services(&btc_connections[i]);
			}
		}
	}
}

void btc_connect_discover_services(btc_connection_t* conn) {
	if (conn->state != btc_connect_state_discover_config) {
		printf("Attempted to discover service without state discover config, state=0x%02X\n\r", conn->state);
		return;
	}

	conn->service_start = 0;
	conn->service_end = 0;
	tBleStatus ret = aci_gatt_clt_disc_all_primary_services(conn->connection);
	if(ret == 0){
		conn->state = btc_connect_state_discover_service;
	}
}

void btc_connect_discover_characteristics(btc_connection_t* conn) {
	if (conn->state != btc_connect_state_discover_service) {
		printf("Attempted to discover characteristics without state discover service, state=0x%02X\n\r", conn->state);
		return;
	}

	conn->tx = 0;
	conn->rx = 0;
	tBleStatus ret = aci_gatt_clt_disc_all_char_of_service(conn->connection, conn->service_start, conn->service_end);
	if(ret == 0){
		conn->state = btc_connect_state_discover_characteristic;
	}
}

void btc_connect_enable_notifications(btc_connection_t* conn) {
	if (conn->state != btc_connect_state_discover_characteristic) {
		printf("Attempted to enable notifications without state discover characteristic, state=0x%02X\n\r", conn->state);
		return;
	}

	uint8_t client_char_conf_data[] = {0x01, 0x00}; // Enable notifications
	tBleStatus ret = aci_gatt_clt_write(conn->connection, conn->tx+2, 2, client_char_conf_data);
	if (ret == 0) {
		conn->state = btc_connect_state_enable_notifications;
	}
}

void btc_connect_tx_request(btc_connection_t* conn, pt_req_t reqType) {
	pt_msg_t msg;
	msg.type = pt_msg_req_type;
	msg.data.request.req = reqType;

	uint16_t len = pt_sizeof(&msg);
	uint8_t* data = malloc(len);
	for (uint8_t i = 0; i < len; i++) {
		data[i] = ((uint8_t*)&msg)[i];
	}
	btc_connect_tx_data(conn, data, len);
}

void btc_connect_tx_data(btc_connection_t* conn, uint8_t* data, uint16_t len) {
	if (conn->state == btc_connect_state_connected) {
		tBleStatus ret = aci_gatt_clt_write_without_resp(conn->connection,conn->rx+1, len, data);
		if(ret != BLE_STATUS_SUCCESS) {
			printf("Error sending data to slave: 0x%02X (handle 0x%04X)\n\r", ret, conn->connection);
		}
	}
	else {
		printf("Attempted to send data without state connected, state=0x%02X\n\r", conn->state);
	}
}

void btc_connect_rx_data(btc_connection_t* conn, uint8_t* data, uint16_t len) {
	if (len < sizeof(uint32_t)) {
		// The message is too short
	    printf("Message received is too short: real size [%d] minimum size [%d]\n\r",
	    		len, sizeof(uint32_t));
	}
	pt_msg_t* msg = (pt_msg_t*)data;

	switch (msg->type) {
		case pt_msg_req_type:
			// Unhandled in Node
			break;

	    case pt_msg_fare_id_type:
	    	ps_send_rsp_fare(conn->reqId, msg->data.fare_id.uuid);
	    	btc_connect_tx_request(conn, pt_req_done);
	    	break;

	    default:
	    	printf("Unknown message type: 0x%08X\n\r", msg->type);
	    	break;
	}
}

void btc_connect_cleanup(btc_connection_t* conn) {
	printf("Cleanup: 0x%02X\n\r", conn->state);
	conn->connection = 0;
	for (uint8_t i = 0; i < BTC_ADDRESS_LEN; i++) {
		conn->address[i] = 0;
	}
	conn->reqId = 99;
	conn->service_start = 0;
	conn->service_end = 0;
	conn->tx = 0;
	conn->rx = 0;
	conn->state = btc_connect_state_empty;
}

void btc_connect_timeout(void* data) {
	//printf("Timeout: 0x%08X\n\r", data);
	btc_connection_t* conn = (btc_connection_t*)(data-0x10);
	if (conn->state != btc_connect_state_empty) {
		printf("Timeout connection 0x%04X: state=0x%02X\n\r", conn->connection, conn->state);
		ps_send_rsp_fare(conn->reqId, BTC_UUID_ERROR);
		btc_connect_cleanup(conn);
	}
}

/* =================================
 * Called from the system
 *
 * Interrupts
================================= */
void hci_le_connection_complete_event(uint8_t Status,
                                      uint16_t Connection_Handle,
                                      uint8_t Role,
                                      uint8_t Peer_Address_Type,
                                      uint8_t Peer_Address[6],
                                      uint16_t Conn_Interval,
                                      uint16_t Conn_Latency,
                                      uint16_t Supervision_Timeout,
                                      uint8_t Master_Clock_Accuracy)

{
	if(Status == 0){
		btc_connect_finish(Connection_Handle, Peer_Address);
	}
	else if(Status == BLE_ERROR_UNKNOWN_CONNECTION_ID){
		printf("Connection canceled.\r\n");
	}
}

void hci_le_enhanced_connection_complete_event(uint8_t Status,
                                               uint16_t Connection_Handle,
                                               uint8_t Role,
                                               uint8_t Peer_Address_Type,
                                               uint8_t Peer_Address[6],
                                               uint8_t Local_Resolvable_Private_Address[6],
                                               uint8_t Peer_Resolvable_Private_Address[6],
                                               uint16_t Conn_Interval,
                                               uint16_t Conn_Latency,
                                               uint16_t Supervision_Timeout,
                                               uint8_t Master_Clock_Accuracy)
{

  hci_le_connection_complete_event(Status,
                                   Connection_Handle,
                                   Role,
                                   Peer_Address_Type,
                                   Peer_Address,
                                   Conn_Interval,
                                   Conn_Latency,
                                   Supervision_Timeout,
                                   Master_Clock_Accuracy);
}

void hci_disconnection_complete_event(uint8_t Status,
                                      uint16_t Connection_Handle,
                                      uint8_t Reason)
{
	if(Status != 0){
		return;
	}

	btc_connection_t* conn = btc_connect_get_connection(Connection_Handle);
	if (conn == NULL) {
		return;
	}

	btc_connect_cleanup(conn);
}

// GATT Services callback
void aci_att_clt_read_by_group_type_resp_event(uint16_t Connection_Handle,
                                           uint8_t Attribute_Data_Length,
                                           uint16_t Data_Length,
                                           uint8_t Attribute_Data_List[])
{
	btc_connection_t* conn = btc_connect_get_connection(Connection_Handle);
	if (conn == NULL) {
		return;
	}

	if (conn->state == btc_connect_state_discover_service) {
		if(Attribute_Data_Length == 20){ // Only 128bit UUIDs
			for(int i = 0; i < Data_Length; i += Attribute_Data_Length){
				if(memcmp(&Attribute_Data_List[i+4], BTC_GATT_SVC_UUID, UUID_LEN) == 0){
					memcpy(&conn->service_start, &Attribute_Data_List[i], 2);
					memcpy(&conn->service_end, &Attribute_Data_List[i+2], 2);
				}
			}
		}
	}
}

// GATT Characteristics callback
void aci_att_clt_read_by_type_resp_event(uint16_t Connection_Handle,
                                     uint8_t Handle_Value_Pair_Length,
                                     uint16_t Data_Length,
                                     uint8_t Handle_Value_Pair_Data[])
{
	btc_connection_t* conn = btc_connect_get_connection(Connection_Handle);
	if (conn == NULL) {
		return;
	}

	if (conn->state == btc_connect_state_discover_characteristic) {
		for(int i = 0; i < Data_Length; i += Handle_Value_Pair_Length){
			if(Handle_Value_Pair_Length == 21){ // 128-bit UUID
				uint16_t handle = LE_TO_HOST_16(&Handle_Value_Pair_Data[i]);
				//print_uuid(&Handle_Value_Pair_Data[i+5]);
				if(memcmp(&Handle_Value_Pair_Data[i+5], BTC_GATT_CHR_TX_UUID, 16) == 0){
					conn->tx = handle;
					//PRINTF("TX Char handle for slave %d: 0x%04X\r\n", slave_index, handle);
				}
				else if(memcmp(&Handle_Value_Pair_Data[i+5], BTC_GATT_CHR_RX_UUID, 16) == 0){
					conn->rx = handle;
					//PRINTF("RX Char Handle for slave %d: 0x%04X\r\n", slave_index, handle);
				}
			}
		}
	}
}

// General procedure complete callback
void aci_gatt_clt_proc_complete_event(uint16_t Connection_Handle,
                                  uint8_t Error_Code)
{
	btc_connection_t* conn = btc_connect_get_connection(Connection_Handle);
	if (conn == NULL) {
		return;
	}

	if(Error_Code != BLE_STATUS_SUCCESS){
		printf("Procedure terminated with error 0x%02X (0x%04X) state=0x%02X.\r\n", Error_Code, Connection_Handle, conn->state);
		conn->state = btc_connect_state_failed;
		return;
	}

	switch (conn->state) {
		case btc_connect_state_discover_config:
			btc_connect_discover_services(conn);
			break;
		case btc_connect_state_discover_service:
			if (conn->service_start != 0) {
				btc_connect_discover_characteristics(conn);
			}
			break;
		case btc_connect_state_discover_characteristic:
			if (conn->tx != 0) {
				btc_connect_enable_notifications(conn);
			}
			break;
		case btc_connect_state_enable_notifications:
			conn->state = btc_connect_state_connected;
			btc_connect_tx_request(conn, pt_req_fare_id);
			break;
		default:
			printf("Unknown procedure completed: 0x%02X\n\r", conn->state);
			break;
	}
}

void aci_gatt_clt_notification_event(uint16_t Connection_Handle,
                                 uint16_t Attribute_Handle,
                                 uint16_t Attribute_Value_Length,
                                 uint8_t Attribute_Value[])
{
	btc_connection_t* conn = btc_connect_get_connection(Connection_Handle);
	if (conn == NULL) {
		return;
	}

	if(Attribute_Handle == conn->tx + 1) {
		btc_connect_rx_data(conn, Attribute_Value, Attribute_Value_Length);
	}
}
