#include "btc_controller.h"

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "ble_const.h"
#include "gatt_db.h"

#include "protocol_serial.h"
#include "config.h"

#include "bluenrg_lp_evb_led.h"

static btc_connection_t btc_connections[BTC_CONNECTIONS_NUM];

void btc_connect_init() {
	for (uint8_t i = 0; i < BTC_CONNECTIONS_NUM; i++) {
		btc_connect_cleanup(&btc_connections[i]);
		btc_connections[i].timer_connect.callback = btc_connect_timeout_connect;
		btc_connections[i].timer_connect.userData = (void*)&(btc_connections[i]);
		btc_connections[i].timer_fare.callback = btc_connect_timeout_fare;
		btc_connections[i].timer_fare.userData = (void*)&(btc_connections[i]);
		//printf("Pointer address: 0x%08X\n\r", &btc_connections[i]);
	}

	/* Since we need to transfer notifications of 244 bytes in a single packet, the LL payload must be
	244 bytes for application data + 3 bytes for ATT header + 4 bytes for L2CAP header. */
	tBleStatus ret = hci_le_write_suggested_default_data_length(BTC_MAX_DATA_LENGTH + 3 + 4, 2120);
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

	//printf("Initialization: BLE Connect complete\n\r");
}

void btc_connect_tick() {
	//BSP_LED_Off(BSP_LED3);
	uint8_t index;
	btc_event_t* event = btc_event_next(&index);
	while (event != NULL) {
		//printf("EVENT 0x%02X\n\r", event->id);
		// Process event
		btc_connection_t* conn = NULL;
		switch (event->id) {
			case btc_event_connect:
				btc_connect_finish(event->connect.connection, event->connect.address);
				break;
			case btc_event_disconnect:
				conn = btc_connect_get_connection(event->disconnect.connection);
				if (conn == NULL) {
					continue;
				}
				if (event->disconnect.reason != BLE_ERROR_TERMINATED_REMOTE_USER
						&& event->disconnect.reason != BLE_ERROR_TERMINATED_LOCAL_HOST) {
					printf("Disconnect reason: 0x%02X\n\r", event->disconnect.reason);
				}
				if (conn->reqId_connect != 0 && !conn->ps_rsp_connect) {
					conn->ps_rsp_connect = 1;
					printf("Disconnected State=0x%02X\n\r", conn->state);
					ps_send_rsp_connect(conn->reqId_connect, 1);
				}
				if (conn->reqId_fare != 0 && !conn->ps_rsp_fare) {
					conn->ps_rsp_fare = 1;
					ps_send_rsp_fare(conn->reqId_fare, BTC_UUID_ERROR);
				}
				HAL_VTIMER_StopTimer(&conn->timer_connect);
				HAL_VTIMER_StopTimer(&conn->timer_fare);
				ps_send_ann_disconnect(conn->address);
				btc_connect_cleanup(conn);
				//printf("BYE BYE MOTHERFUCKER\n\r");
				break;
			case btc_event_proc_complete:
				conn = btc_connect_get_connection(event->proc_complete.connection);
				if (conn == NULL) {
					if (event->proc_complete.error != BLE_STATUS_SUCCESS) {
						printf("Procedure terminated with error 0x%02X (0x%04X) could not map connection handle to btc_connections\n\r",
								event->proc_complete.error, event->proc_complete.connection);
					}
					return;
				}
				btc_connect_proc_complete(conn, event->proc_complete.error);
				//BSP_LED_Off(BSP_LED3);
				break;
			case btc_event_rx_data:
				conn = btc_connect_get_connection(event->rx_data.connection);
				if (conn == NULL) {
					printf("No connection handle: 0x%04X\n\r", event->rx_data.connection);
					return;
				}

				if(event->rx_data.attribute == conn->rx + 1) {
					btc_connect_rx_data(conn, event->rx_data.data, event->rx_data.dataLength);
				}
				break;
			default:
				printf("Unknown BTC event: 0x%02X\n\r", event->id);
				break;
		}

		// Cleanup and get next event
		btc_event_free(index);
		event = btc_event_next(&index);
		//BSP_LED_Off(BSP_LED3);
	}
	//BSP_LED_Off(BSP_LED3);
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
	printf("Connection request\n\r");
	for (uint8_t i = 0; i < BTC_CONNECTIONS_NUM; i++) {
		if (btc_address_match(btc_connections[i].address, addr) && btc_connections[i].state != btc_connect_state_empty) {
			// Connection exists
			printf("btc_connect_request Connection exists: i=%d\n\r", i);
			if (btc_connections[i].state == btc_connect_state_connected) {
				ps_send_rsp_connect(reqId, 0);
			}
			else {
				HAL_VTIMER_StopTimer(&btc_connections[i].timer_connect);
				btc_connections[i].reqId_connect = reqId;
				btc_connections[i].ps_rsp_connect = 0;
				HAL_VTIMER_StartTimerMs(&btc_connections[i].timer_connect, RSP_TIMEOUT_CONNECT);
			}
			return;
		}
	}

	//printf("Connection requested: %d\n\r", reqId);
	for (uint8_t i = 0; i < BTC_CONNECTIONS_NUM; i++) {
		if (btc_connections[i].state == btc_connect_state_empty) {
			printf("btc_connect_request Found new: i=%d\n\r", i);
			btc_connections[i].state = btc_connect_state_scanning;
			btc_connections[i].reqId_connect = reqId;
			btc_connections[i].reqId_fare = 0;
			btc_connections[i].ps_rsp_connect = 0;
			btc_connections[i].ps_rsp_fare = 0;
			HAL_VTIMER_StartTimerMs(&btc_connections[i].timer_connect, RSP_TIMEOUT_CONNECT);
			//printf("Address: ");
			for (uint8_t j = 0; j < BTC_ADDRESS_LEN; j++) {
				//printf("0x%02X ", addr[j]);
				btc_connections[i].address[j] = addr[j];
			}
			//printf("\n\r");
			btc_adv_scan_start();
			break;
		}
	}
}

void btc_connect_fare_request(uint8_t reqId, uint8_t* addr) {
	//BSP_LED_Off(BSP_LED3);
	for (uint8_t i = 0; i < BTC_CONNECTIONS_NUM; i++) {
		if (btc_address_match(btc_connections[i].address, addr)) {
			btc_connections[i].reqId_fare = reqId;
			btc_connections[i].ps_rsp_fare = 0;
			HAL_VTIMER_StartTimerMs(&btc_connections[i].timer_fare, RSP_TIMEOUT_FARE);
			if (btc_connections[i].state == btc_connect_state_connected) {
				btc_connect_tx_request(&btc_connections[i], pt_req_fare_id);
			}
			return;
		}
	}

	// If here, not connect exists
	// Start new connection
	//printf("No connection found, create new one\n\r");
	btc_connect_request(0, addr);
	for (uint8_t i = 0; i < BTC_CONNECTIONS_NUM; i++) {
		if (btc_address_match(btc_connections[i].address, addr)) {
			btc_connections[i].reqId_fare = reqId;
			btc_connections[i].ps_rsp_fare = 0;
			HAL_VTIMER_StartTimerMs(&btc_connections[i].timer_fare, RSP_TIMEOUT_FARE);
		}
	}
}

void btc_connect_start(uint8_t addrType, uint8_t* addr) {
	btc_adv_scan_stop();
	//printf("Starting connection\n\r");
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
			btc_connections[i].state = btc_connect_state_discover_config;
			btc_connections[i].connection = connection;
			//printf("Connection=0x%04X i=%d\n\r", connection, i);

			tBleStatus ret = aci_gatt_clt_exchange_config(connection);
			if(ret != 0) {
				btc_connect_discover_services(&btc_connections[i]);
			}
		}
	}

	// Determine if scanning needs to be restarted
	if (btc_connect_should_scan()) {
		btc_adv_scan_start();
	}
}

void btc_connect_disconnect(btc_connection_t* conn) {
	tBleStatus ret = hci_disconnect(conn->connection, 0x13);
	if (ret) {
		printf("Failed to disconnect: 0x%02X\n\r", ret);
	}
}

void btc_connect_proc_complete(btc_connection_t* conn, uint8_t error) {
	if(error != BLE_STATUS_SUCCESS){
		printf("Procedure terminated with error 0x%02X (0x%04X) state=0x%02X.\r\n",
				error, conn->connection, conn->state);
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
			if (conn->reqId_connect != 0 && !conn->ps_rsp_connect) {
				BSP_LED_On(BSP_LED3);
				// TODO: Add state output
				conn->ps_rsp_connect = 1;
				ps_send_rsp_connect(conn->reqId_connect, 0);
				if (conn->reqId_fare != 0) {
					btc_connect_tx_request(conn, pt_req_fare_id);
					HAL_VTIMER_StopTimer(&conn->timer_connect);
				}
				//BSP_LED_Off(BSP_LED3);
			}
			break;
		case btc_connect_state_connected:
			// Response to write
			// TODO: Add logic maybe?
			break;
		default:
			printf("Unknown procedure completed: 0x%02X\n\r", conn->state);
			break;
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
	tBleStatus ret = aci_gatt_clt_disc_all_char_of_service(conn->connection,
			conn->service_start, conn->service_end);
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
	tBleStatus ret = aci_gatt_clt_write(conn->connection, conn->rx+2, 2, client_char_conf_data);
	if (ret == 0) {
		conn->state = btc_connect_state_enable_notifications;
	}
}

void btc_connect_tx_request(btc_connection_t* conn, pt_req_t reqType) {
	pt_msg_t msg;
	msg.type = pt_msg_req_type;
	msg.data.request.req = reqType;

	uint16_t len = pt_sizeof(&msg);
	uint8_t* data = (uint8_t*)&msg;
	btc_connect_tx_data(conn, data, len);
}

void btc_connect_tx_data(btc_connection_t* conn, uint8_t* data, uint16_t len) {
	//printf("TX Data: conn=[0x%08X] connection=[0x%04X] rx=[0x%04X]\n\r", conn, conn->connection, conn->rx);
	if (conn->state == btc_connect_state_connected) {
		tBleStatus ret = aci_gatt_clt_write_without_resp(conn->connection,conn->rx+1, len, data);
		//tBleStatus ret = aci_gatt_clt_write(conn->connection,conn->rx+1, len, data);
		if(ret != BLE_STATUS_SUCCESS) {
			printf("Failed to send data: 0x%02X [0x%02X", ret, data[0]);
			for (uint16_t i = 1; i < len; i++) {
				printf(" 0x%02X", data[i]);
			}
			printf("]\n\r");
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
	    	if (!conn->ps_rsp_fare && conn->reqId_fare != 0) {
	    		conn->ps_rsp_fare = 1;
	    		ps_send_rsp_fare(conn->reqId_fare, msg->data.fare_id.uuid);
	    		HAL_VTIMER_StopTimer(&conn->timer_fare);
	    	}
	    	//btc_connect_disconnect(conn);
	    	printf("Sending done msg\n\r");
	    	btc_connect_tx_request(conn, pt_req_done);
	    	break;

	    default:
	    	printf("Unknown message type: 0x%08X\n\r", msg->type);
	    	break;
	}
}

void btc_connect_cleanup(btc_connection_t* conn) {
	//BSP_LED_Off(BSP_LED3);
	//printf("Cleanup: 0x%02X\n\r", conn->state);
	HAL_VTIMER_StopTimer(&conn->timer_connect);
	HAL_VTIMER_StopTimer(&conn->timer_fare);
	conn->connection = 0;
	for (uint8_t i = 0; i < BTC_ADDRESS_LEN; i++) {
		conn->address[i] = 0;
	}
	conn->reqId_connect = 0;
	conn->reqId_fare = 0;
	conn->service_start = 0;
	conn->service_end = 0;
	conn->tx = 0;
	conn->rx = 0;
	conn->state = btc_connect_state_empty;
	conn->ps_rsp_connect = 0;
	conn->ps_rsp_fare = 0;
	//BSP_LED_Off(BSP_LED3);
}

void btc_connect_timeout_connect(void* data) {
	uint8_t* data_ptr = data-0x10;
	//printf("Timeout connect: 0x%08X\n\r", data_ptr);
	btc_connection_t* conn = (btc_connection_t*)(data_ptr);
	//printf("handle=0x%04X\n\r", conn->connection);
	if (conn->connection == 0) {
		// No advertisement matching this address was found
		if (conn->reqId_connect != 0 && !conn->ps_rsp_connect) {
			conn->ps_rsp_connect = 1;
			ps_send_rsp_connect(conn->reqId_connect, 4);
			btc_connect_cleanup(conn);
		}
		return;
	}

	if (conn->state != btc_connect_state_connected) {
		if (conn->reqId_connect != 0 && !conn->ps_rsp_connect) {
			conn->ps_rsp_connect = 1;
			ps_send_rsp_connect(conn->reqId_connect, 2);
			printf("Connection was in state=0x%02X\n\r", conn->state);
			btc_connect_disconnect(conn);
		}
	}
}

void btc_connect_timeout_fare(void* data) {
	uint8_t* data_ptr = data-0x28;
	//printf("Timeout fare: 0x%08X\n\r", data_ptr);
	btc_connection_t* conn = (btc_connection_t*)(data_ptr);
	//printf("handle=0x%04X\n\r", conn->connection);
	if (conn->state != btc_connect_state_empty) {
		//printf("Timeout connection 0x%04X: state=0x%02X\n\r", conn->connection, conn->state);
		if (!conn->ps_rsp_connect && conn->reqId_connect != 0) {
			conn->ps_rsp_connect = 1;
			ps_send_rsp_fare(conn->reqId_connect, BTC_UUID_ERROR);
		}
		if (!conn->ps_rsp_fare && conn->reqId_fare != 0) {
			conn->ps_rsp_fare = 1;
			ps_send_rsp_fare(conn->reqId_fare, BTC_UUID_ERROR);
		}

		btc_connect_disconnect(conn);
	}
}

uint8_t btc_connect_should_scan() {
	for (uint8_t i = 0; i < BTC_CONNECTIONS_NUM; i++) {
		if (btc_connections[i].state == btc_connect_state_scanning) {
			printf("Scanning needed i=%d\n\r", i);
			return 1;
		}
	}
	return 0;
}

void btc_connect_disconnect_scanning() {
	for (uint8_t i = 0; i < BTC_CONNECTIONS_NUM; i++) {
		if (btc_connections[i].state == btc_connect_state_scanning) {
			ps_send_rsp_connect(btc_connections[i].reqId_connect, 5);
			btc_connect_cleanup(&btc_connections[i]);
		}
	}
}

/* =================================
 * Called from the system
 *
 * Interrupts
================================= */
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
					//printf("Tx handle: 0x%04X\n\r", conn->tx);
				}
				else if(memcmp(&Handle_Value_Pair_Data[i+5], BTC_GATT_CHR_RX_UUID, 16) == 0){
					conn->rx = handle;
					//printf("Rx handle: 0x%04X\n\r", conn->rx);
				}
			}
		}
	}
}

void aci_gatt_eatt_proc_timeout_event(uint16_t Connection_Handle,
                                      uint16_t CID)
{
	printf("GATT timeout event: Handle=0x%04X CID=0x%04X\n\r", Connection_Handle, CID);
}

void aci_gatt_clt_error_resp_event(uint16_t Connection_Handle,
                               uint8_t Req_Opcode,
                               uint16_t Attribute_Handle,
                               uint8_t Error_Code)
{
	if (Req_Opcode == 0x08 && Error_Code == 0x0A) {
		return;
	}
	printf("aci_gatt_clt_error_resp_event handle=0x%04X req_opcode=0x%02X att_handle=0x%04X error=0x%02X\n\r",
		  Connection_Handle, Req_Opcode, Attribute_Handle, Error_Code);
}

void aci_gatt_clt_disc_read_char_by_uuid_resp_event(uint16_t Connection_Handle,
                                                    uint16_t Attribute_Handle,
                                                    uint8_t Attribute_Value_Length,
                                                    uint8_t Attribute_Value[]) {
	printf("aci_gatt_clt_disc_read_char_by_uuid_resp_event handle=0x%04X att=0x%04X\n\r",
			Connection_Handle, Attribute_Handle);
}

void aci_gap_proc_complete_event(uint8_t Procedure_Code,
                                 uint8_t Status,
                                 uint8_t Data_Length,
                                 uint8_t Data[])
{
  //APP_FLAG_CLEAR(SCANNING);
  //printf("aci_gap_proc_complete_event\n\r");
}

void aci_gatt_srv_write_event(uint16_t Connection_Handle,
                                 uint8_t Resp_Needed,
                                 uint16_t Attribute_Handle,
                                 uint16_t Data_Length,
                                 uint8_t Data[])
{
	printf("aci_gatt_srv_write_event\n\r");
}

/*void hci_command_status_event() {

}*/
