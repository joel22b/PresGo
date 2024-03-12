#include "btc_controller.h"

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "bluenrg_lp_evb_led.h"
#include "protocol_serial/protocol_serial.h"

#define BTC_EVENT_EMPTY 0x00
#define BTC_EVENT_READY	0x01
#define BTC_EVENT_BUSY	0x02

btc_event_t btc_events[BTC_EVENTS_NUM];
uint8_t btc_events_indexes[BTC_EVENTS_NUM];
//uint8_t btc_event_index_read;
//uint8_t btc_event_index_write;

void btc_event_init() {
	for (uint8_t i = 0; i < BTC_EVENTS_NUM; i++) {
		btc_events_indexes[i] = BTC_EVENT_EMPTY;
	}
	//printf("size=%d size2=%d\n\r", sizeof(btc_event_t), sizeof(btc_event_rx_data_t));
}

btc_event_t* btc_event_next(uint8_t* index) {
	for (uint8_t i = 0; i < BTC_EVENTS_NUM; i++) {
		if (btc_events_indexes[i] == BTC_EVENT_READY) {
			btc_events_indexes[i] = BTC_EVENT_BUSY;
			*index = i;
			return &btc_events[i];
		}
	}

	return NULL;
}

void btc_event_free(uint8_t index) {
	btc_events_indexes[index] = BTC_EVENT_EMPTY;
}

void btc_event_add(btc_event_t* event) {
	BSP_COM_SetCb(ps_recv_callback);
	for (uint8_t i = 0; i < BTC_EVENTS_NUM; i++) {
		if (btc_events_indexes[i] == BTC_EVENT_EMPTY) {
			//printf("Event a 0x%02X ", event->id);
			//BSP_COM_PrintCb();
			//printf("a=0x%08X b=0x%08X\n\r", &btc_events[i], event);
			memcpy(&btc_events[i], event, sizeof(btc_event_t));
			btc_events_indexes[i] = BTC_EVENT_READY;
			//printf("Event b 0x%02X ", event->id);
			//BSP_COM_PrintCb();
			//printf("Added: 0x%02X\n\r", event->id);
			return;
		}
	}

	// Queue full
	printf("Event queue full, skipping event=0x%02X\n\r", event->id);
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
		btc_event_t event;
		event.id = btc_event_connect;
		event.connect.connection = Connection_Handle;
		memcpy(event.connect.address, Peer_Address, BTC_ADDRESS_LEN);
		btc_event_add(&event);
		//btc_connect_finish(Connection_Handle, Peer_Address);
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

	btc_event_t event;
	event.id = btc_event_disconnect;
	event.disconnect.connection = Connection_Handle;
	event.disconnect.reason = Reason;
	btc_event_add(&event);
	/*btc_connection_t* conn = btc_connect_get_connection(Connection_Handle);
	if (conn == NULL) {
		return;
	}
	//printf("BYE BYE MOTHERFUCKER\n\r");

	btc_connect_cleanup(conn);*/
}

// General procedure complete callback
void aci_gatt_clt_proc_complete_event(uint16_t Connection_Handle,
                                  uint8_t Error_Code)
{
	btc_event_t event;
	event.id = btc_event_proc_complete;
	event.proc_complete.connection = Connection_Handle;
	event.proc_complete.error = Error_Code;
	btc_event_add(&event);
}

void aci_gatt_clt_notification_event(uint16_t Connection_Handle,
                                 uint16_t Attribute_Handle,
                                 uint16_t Attribute_Value_Length,
                                 uint8_t Attribute_Value[])
{
	if (Attribute_Value_Length > BTC_MAX_DATA_LENGTH) {
		printf("Received message too big for buffer, skipping: size=%d buffer=%d\n\r",
				Attribute_Value_Length, BTC_MAX_DATA_LENGTH);
		return;
	}
	//printf("Att=0x%08X 0x%08X\n\r", Attribute_Value, &Attribute_Value[0]);
	//printf("RX ");
	//BSP_COM_PrintCb();

	btc_event_t event;
	event.id = btc_event_rx_data;
	event.rx_data.connection = Connection_Handle;
	event.rx_data.attribute = Attribute_Handle;
	event.rx_data.dataLength = Attribute_Value_Length;
	memcpy(event.rx_data.data, Attribute_Value, Attribute_Value_Length);
	btc_event_add(&event);

	/*btc_connection_t* conn = btc_connect_get_connection(Connection_Handle);
	if (conn == NULL) {
		return;
	}

	if(Attribute_Handle == conn->tx + 1) {
		btc_connect_rx_data(conn, Attribute_Value, Attribute_Value_Length);
	}*/
}

