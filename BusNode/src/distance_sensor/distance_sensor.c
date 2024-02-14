/*
 * distance_sensor.c
 *
 *  Created on: Feb. 9, 2024
 *      Author: Joel
 */

#include "distance_sensor.h"

#include "stdio.h"

#include "rf_driver_hal.h"
#include "bluenrg_lp_evb_config.h"

#include "core/VL53L1X_api.h"
#include "distance_sensor_defs.h"
#include "protocol_serial.h"

/* Size of Transmission buffer */
#define TXBUFFERSIZE                      (COUNTOF(aTxBuffer) - 1)
/* Size of Reception buffer */
#define RXBUFFERSIZE                      TXBUFFERSIZE

/* I2C TIMING in Fast Mode Plus */
__IO uint32_t timing = __LL_I2C_CONVERT_TIMINGS(0x01, 0x03, 0x02, 0x03, 0x09);

/* Buffer used for transmission */
uint8_t aTxBuffer[] = " ****I2C_TwoBoards communication based on IT****  ****I2C_TwoBoards communication based on IT****  ****I2C_TwoBoards communication based on IT**** ";

void Error_Handler(void)
{
  /* User can add his own implementation to report the HAL error return state */
  /* Turn LED3 on */
  BSP_LED_On(BSP_LED3);
  printf("ERROR\n\r");
  while(1)
  {
  }
}

#define COUNTOF(__BUFFER__)   (sizeof(__BUFFER__) / sizeof(*(__BUFFER__)))

#define DS_ADDRESS 0x52

I2C_HandleTypeDef hi2cx;
uint8_t is_init = 0;

uint16_t Distance;
uint16_t SignalRate;
uint16_t AmbientRate;
uint16_t SpadNum;
uint8_t RangeStatus;

uint8_t inDoorway;

void ds_init() {
	hi2cx.Instance = I2Cx;
	hi2cx.Init.Timing = timing;
	hi2cx.Init.OwnAddress1 = I2C_ADDRESS;
	hi2cx.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	hi2cx.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	hi2cx.Init.OwnAddress2 = 0;
	hi2cx.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
	hi2cx.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	hi2cx.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	if (HAL_I2C_Init(&hi2cx) != HAL_OK) {
		Error_Handler();
	}

	/** Configure Analogue filter */
	if (HAL_I2CEx_ConfigAnalogFilter(&hi2cx, I2C_ANALOGFILTER_ENABLE) != HAL_OK) {
	    Error_Handler();
	}

	/** Configure Digital filter */
	if (HAL_I2CEx_ConfigDigitalFilter(&hi2cx, 0) != HAL_OK) {
	    Error_Handler();
	}

	/** I2C Enable Fast Mode Plus */
	__HAL_RCC_SYSCFG_CLK_ENABLE() ;
	HAL_I2CEx_EnableFastModePlus(SYSCFG_FASTMODEPLUS_PA0);
	HAL_I2CEx_EnableFastModePlus(SYSCFG_FASTMODEPLUS_PA1);

	int8_t status;
	uint16_t wordData;
	//uint8_t ToFSensor = 1; // 0=Left, 1=Center(default), 2=Right

	/* Those basic I2C read functions can be used to check your own I2C functions */
	status = VL53L1_RdWord(&hi2cx, DS_ADDRESS, 0x010F, &wordData);
	if (status != 0) {
		printf("Failed to discover distance sensor on i2c bus: %d\n\r", status);
		return;
	}
	// Validate the Model ID and Module Type
	if ((wordData >> 8 == DS_MODEL_ID) && (wordData & 0x00FF == DS_MODULE_TYPE)) {
		printf("Invalid Model ID 0x%02X and Module Type 0x%02X\n\r",
				wordData >> 8, wordData & 0x00FF);
		return;
	}

	//printf("VL53L1X: %X\r\n", wordData);

	//	    while(sensorState==0){
	//	  		status = VL53L1X_BootState(DS_ADDRESS, &sensorState);
	//	  	HAL_Delay(2);
	//	    }

	/* This function must to be called to initialize the sensor with the default setting  */
	//printf("Init Sensor");
	status = VL53L1X_SensorInit(&hi2cx, DS_ADDRESS);
	if (status != 0) {
		printf("Failed SensorInit %d\n\r");
		return;
	}
	//printf("Init Sensor Done");
	/* Optional functions to be used to change the main ranging parameters according the application requirements to get the best ranging performances */
	//printf("Set Distance Mode");
	status = VL53L1X_SetDistanceMode(&hi2cx, DS_ADDRESS, 2); /* 1=short, 2=long */
	if (status != 0) {
		printf("Failed SetDistanceMode %d\n\r");
		return;
	}
	//printf("Set Distance Mode Done");
	//printf("Set Timing Budget");
	status = VL53L1X_SetTimingBudgetInMs(&hi2cx, DS_ADDRESS, 100); /* in ms possible values [20, 50, 100, 200, 500] */
	if (status != 0) {
		printf("Failed SetTimingBudget %d\n\r");
		return;
	}
	//printf("Set Timing Budget Done");

	//printf("Set InterMeasurementinMS");
	status = VL53L1X_SetInterMeasurementInMs(&hi2cx, DS_ADDRESS, 100); /* in ms, IM must be > = TB */
	if (status != 0) {
		printf("Failed SetInterMeasurement %d\n\r");
		return;
	}
	//printf("Set InterMeasurementinMS");
	//  status = VL53L1X_SetOffset(DS_ADDRESS,20); /* offset compensation in mm */
	//  status = VL53L1X_SetROI(DS_ADDRESS, 16, 16); /* minimum ROI 4,4 */
	//	status = VL53L1X_CalibrateOffset(DS_ADDRESS, 140, &offset); /* may take few second to perform the offset cal*/
	//	status = VL53L1X_CalibrateXtalk(DS_ADDRESS, 1000, &xtalk); /* may take few second to perform the xtalk cal */
	//printf("VL53L1X Ultra Lite Driver Example running ...\n");
	status = VL53L1X_StartRanging(&hi2cx, DS_ADDRESS);
	if (status != 0) {
		printf("Failed StartRanging %d\n\r");
		return;
	}
	//printf("Finished distance sensor initialization\n\r");

	inDoorway = 0;
	is_init = 1;
}

I2C_HandleTypeDef* ds_get_i2c_handle() {
	if (is_init) {
		return &hi2cx;
	}
	return NULL;
}

void ds_tick() {
	if (!is_init) {
		return;
	}

	uint8_t status;
	uint8_t dataReady = 0;
	while (dataReady == 0){
		status = VL53L1X_CheckForDataReady(&hi2cx, DS_ADDRESS, &dataReady);
	  	HAL_Delay(2);
	}
	//	  	printf("exiting data ready loop");
	status = VL53L1X_GetRangeStatus(&hi2cx, DS_ADDRESS, &RangeStatus);
	status = VL53L1X_GetDistance(&hi2cx, DS_ADDRESS, &Distance);
	status = VL53L1X_GetSignalRate(&hi2cx, DS_ADDRESS, &SignalRate);
	status = VL53L1X_GetAmbientRate(&hi2cx, DS_ADDRESS, &AmbientRate);
	status = VL53L1X_GetSpadNb(&hi2cx, DS_ADDRESS, &SpadNum);
	status = VL53L1X_ClearInterrupt(&hi2cx, DS_ADDRESS); /* clear interrupt has to be called to enable next interrupt*/
	//printf("RangeStatus: %u, Distance: %u, SignalRate: %u, AmbientRate: %u,SpadNum: %u\r\n", RangeStatus, Distance, SignalRate, AmbientRate,SpadNum);

	if (inDoorway && Distance > DS_DISTANCE_IN_DOORWAY_THESHOLD) {
		inDoorway = 0;
		ps_send_ann_door(inDoorway);
	}
	else if (!inDoorway && Distance < DS_DISTANCE_IN_DOORWAY_THESHOLD) {
		inDoorway = 1;
		ps_send_ann_door(inDoorway);
	}
}
