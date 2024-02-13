################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/BSP/bluenrg_lp_evb_button.c \
../Drivers/BSP/bluenrg_lp_evb_com.c \
../Drivers/BSP/bluenrg_lp_evb_i2c.c \
../Drivers/BSP/bluenrg_lp_evb_io.c \
../Drivers/BSP/bluenrg_lp_evb_led.c \
../Drivers/BSP/bluenrg_lp_evb_spi.c \
../Drivers/BSP/lps22hh_reg.c \
../Drivers/BSP/lsm6dsox_reg.c 

OBJS += \
./Drivers/BSP/bluenrg_lp_evb_button.o \
./Drivers/BSP/bluenrg_lp_evb_com.o \
./Drivers/BSP/bluenrg_lp_evb_i2c.o \
./Drivers/BSP/bluenrg_lp_evb_io.o \
./Drivers/BSP/bluenrg_lp_evb_led.o \
./Drivers/BSP/bluenrg_lp_evb_spi.o \
./Drivers/BSP/lps22hh_reg.o \
./Drivers/BSP/lsm6dsox_reg.o 

C_DEPS += \
./Drivers/BSP/bluenrg_lp_evb_button.d \
./Drivers/BSP/bluenrg_lp_evb_com.d \
./Drivers/BSP/bluenrg_lp_evb_i2c.d \
./Drivers/BSP/bluenrg_lp_evb_io.d \
./Drivers/BSP/bluenrg_lp_evb_led.d \
./Drivers/BSP/bluenrg_lp_evb_spi.d \
./Drivers/BSP/lps22hh_reg.d \
./Drivers/BSP/lsm6dsox_reg.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/BSP/%.o: ../Drivers/BSP/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -mfloat-abi=soft -Os -ffunction-sections -fdata-sections -g3 -DCONFIG_DEVICE_BLUENRG_LPS -DSTEVAL_IDB012V1 -DBLE_STACK_CUSTOM_CONF -DCONFIG_HW_HSE_TUNE=32 -DCONFIG_HW_LS_XTAL -DCONFIG_HW_SMPS_10uH -DCONFIG_NUM_MAX_LINKS=5 -DMAX_NUM_MASTERS=2 -DMAX_NUM_SLAVES=3 -DPROFILE_ROLE=ROLE_COLLECTOR -DUSE_FULL_LL_DRIVER -DUSER_BUTTON=BSP_PUSH1 -I"C:/Braun_Area/FYDP/PresGo/BusNode/inc" -I"C:/Braun_Area/FYDP/PresGo/BusNode/src" -I"C:/ST/BlueNRG-LP_LPS DK 1.4.0/Drivers/Peripherals_Drivers/Inc" -I"C:/ST/BlueNRG-LP_LPS DK 1.4.0/Drivers/CMSIS/Device/ST/BlueNRG_LP/Include" -I"C:/ST/BlueNRG-LP_LPS DK 1.4.0/Middlewares/ST/hal/Inc" -I"C:/ST/BlueNRG-LP_LPS DK 1.4.0/Middlewares/ST/PKAMGR/Inc" -I"C:/ST/BlueNRG-LP_LPS DK 1.4.0/Middlewares/ST/RNGMGR/Inc" -I"C:/ST/BlueNRG-LP_LPS DK 1.4.0/Middlewares/ST/AESMGR/Inc" -I"C:/ST/BlueNRG-LP_LPS DK 1.4.0/Middlewares/ST/BLECNTR/Inc" -I"C:/ST/BlueNRG-LP_LPS DK 1.4.0/Middlewares/ST/NVMDB/Inc" -I"C:/ST/BlueNRG-LP_LPS DK 1.4.0/Drivers/CMSIS/Include" -I"C:/ST/BlueNRG-LP_LPS DK 1.4.0/Drivers/BSP/Inc" -I"C:/ST/BlueNRG-LP_LPS DK 1.4.0/Drivers/BSP/Components/lps22hh_STdC/driver" -I"C:/ST/BlueNRG-LP_LPS DK 1.4.0/Drivers/BSP/Components/lsm6dsox_STdC/driver" -I"C:/ST/BlueNRG-LP_LPS DK 1.4.0/Middlewares/ST/BLE_Application/layers_inc" -I"C:/ST/BlueNRG-LP_LPS DK 1.4.0/Middlewares/ST/BLE_Application/OTA/Inc" -I"C:/ST/BlueNRG-LP_LPS DK 1.4.0/Middlewares/ST/Bluetooth_LE/Inc" -I"C:/ST/BlueNRG-LP_LPS DK 1.4.0/Middlewares/ST/BLE_Application/Profiles/Inc" -I"C:/ST/BlueNRG-LP_LPS DK 1.4.0/Middlewares/ST/cryptolib/Inc" -I"C:/ST/BlueNRG-LP_LPS DK 1.4.0/Middlewares/ST/cryptolib/Inc/Common" -I"C:/ST/BlueNRG-LP_LPS DK 1.4.0/Middlewares/ST/cryptolib/Inc/AES" -I"C:/Braun_Area/FYDP/PresGo/BusNode/src/btc_controller" -I"C:/Braun_Area/FYDP/PresGo/BusNode/src/distance_sensor" -I"C:/Braun_Area/FYDP/PresGo/BusNode/src/protocol_serial" -I"C:/ST/BlueNRG-LP_LPS DK 1.4.0/Middlewares/ST/hal/Inc" -I"C:/Braun_Area/FYDP/PresGo/BusNode/src/rf_device" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


