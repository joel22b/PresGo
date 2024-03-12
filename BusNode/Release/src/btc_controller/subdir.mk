################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/btc_controller/btc_controller.c \
../src/btc_controller/btc_controller_adv.c \
../src/btc_controller/btc_controller_connect.c \
../src/btc_controller/btc_controller_event.c 

OBJS += \
./src/btc_controller/btc_controller.o \
./src/btc_controller/btc_controller_adv.o \
./src/btc_controller/btc_controller_connect.o \
./src/btc_controller/btc_controller_event.o 

C_DEPS += \
./src/btc_controller/btc_controller.d \
./src/btc_controller/btc_controller_adv.d \
./src/btc_controller/btc_controller_connect.d \
./src/btc_controller/btc_controller_event.d 


# Each subdirectory must supply rules for building sources it contributes
src/btc_controller/%.o: ../src/btc_controller/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -mfloat-abi=soft -Os -ffunction-sections -fdata-sections -g3 -DCONFIG_DEVICE_BLUENRG_LPS -DSTEVAL_IDB012V1 -DBLE_STACK_CUSTOM_CONF -DCONFIG_HW_HSE_TUNE=32 -DCONFIG_HW_LS_XTAL -DCONFIG_HW_SMPS_10uH -DCONFIG_NUM_MAX_LINKS=4 -DMAX_NUM_MASTERS=2 -DMAX_NUM_SLAVES=3 -DPROFILE_ROLE=ROLE_COLLECTOR -DUSE_FULL_LL_DRIVER -DUSER_BUTTON=BSP_PUSH1 -DEXTENDED_ADV -DLEGACY_ADV -I"C:/Braun_Area/FYDP/PresGo/BusNode/inc" -I"C:/Braun_Area/FYDP/PresGo/BusNode/src" -I"C:/ST/BlueNRG-LP_LPS DK 1.4.0/Drivers/Peripherals_Drivers/Inc" -I"C:/ST/BlueNRG-LP_LPS DK 1.4.0/Drivers/CMSIS/Device/ST/BlueNRG_LP/Include" -I"C:/ST/BlueNRG-LP_LPS DK 1.4.0/Middlewares/ST/hal/Inc" -I"C:/ST/BlueNRG-LP_LPS DK 1.4.0/Middlewares/ST/PKAMGR/Inc" -I"C:/ST/BlueNRG-LP_LPS DK 1.4.0/Middlewares/ST/RNGMGR/Inc" -I"C:/ST/BlueNRG-LP_LPS DK 1.4.0/Middlewares/ST/AESMGR/Inc" -I"C:/ST/BlueNRG-LP_LPS DK 1.4.0/Middlewares/ST/BLECNTR/Inc" -I"C:/ST/BlueNRG-LP_LPS DK 1.4.0/Middlewares/ST/NVMDB/Inc" -I"C:/ST/BlueNRG-LP_LPS DK 1.4.0/Drivers/CMSIS/Include" -I"C:/ST/BlueNRG-LP_LPS DK 1.4.0/Drivers/BSP/Inc" -I"C:/ST/BlueNRG-LP_LPS DK 1.4.0/Drivers/BSP/Components/lps22hh_STdC/driver" -I"C:/ST/BlueNRG-LP_LPS DK 1.4.0/Drivers/BSP/Components/lsm6dsox_STdC/driver" -I"C:/ST/BlueNRG-LP_LPS DK 1.4.0/Middlewares/ST/BLE_Application/layers_inc" -I"C:/ST/BlueNRG-LP_LPS DK 1.4.0/Middlewares/ST/BLE_Application/OTA/Inc" -I"C:/ST/BlueNRG-LP_LPS DK 1.4.0/Middlewares/ST/Bluetooth_LE/Inc" -I"C:/ST/BlueNRG-LP_LPS DK 1.4.0/Middlewares/ST/BLE_Application/Profiles/Inc" -I"C:/ST/BlueNRG-LP_LPS DK 1.4.0/Middlewares/ST/cryptolib/Inc" -I"C:/ST/BlueNRG-LP_LPS DK 1.4.0/Middlewares/ST/cryptolib/Inc/Common" -I"C:/ST/BlueNRG-LP_LPS DK 1.4.0/Middlewares/ST/cryptolib/Inc/AES" -I"C:/Braun_Area/FYDP/PresGo/BusNode/src/btc_controller" -I"C:/Braun_Area/FYDP/PresGo/BusNode/src/distance_sensor" -I"C:/Braun_Area/FYDP/PresGo/BusNode/src/protocol_serial" -I"C:/ST/BlueNRG-LP_LPS DK 1.4.0/Middlewares/ST/hal/Inc" -I"C:/Braun_Area/FYDP/PresGo/BusNode/src/rf_device" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


