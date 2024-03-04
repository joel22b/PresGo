################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/Joel/SimplicityStudio/SDKs/gecko_sdk_4.4.0/platform/common/src/sl_assert.c \
C:/Users/Joel/SimplicityStudio/SDKs/gecko_sdk_4.4.0/platform/common/src/sl_syscalls.c 

OBJS += \
./gecko_sdk_4.4.1/platform/common/src/sl_assert.o \
./gecko_sdk_4.4.1/platform/common/src/sl_syscalls.o 

C_DEPS += \
./gecko_sdk_4.4.1/platform/common/src/sl_assert.d \
./gecko_sdk_4.4.1/platform/common/src/sl_syscalls.d 


# Each subdirectory must supply rules for building sources it contributes
gecko_sdk_4.4.1/platform/common/src/sl_assert.o: C:/Users/Joel/SimplicityStudio/SDKs/gecko_sdk_4.4.0/platform/common/src/sl_assert.c gecko_sdk_4.4.1/platform/common/src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m33 -mthumb -std=c99 '-DDEBUG=1' '-DDEBUG_EFM=1' '-DEFR32BG22C224F512IM40=1' '-DSL_COMPONENT_CATALOG_PRESENT=1' -I"C:\Braun_Area\FYDP\PresGo\Fob\FareFare\config" -I"C:\Braun_Area\FYDP\PresGo\Fob\FareFare\autogen" -I"C:\Braun_Area\FYDP\PresGo\Fob\FareFare" -I"C:/Users/Joel/SimplicityStudio/SDKs/gecko_sdk_4.4.0//platform/Device/SiliconLabs/EFR32BG22/Include" -I"C:/Users/Joel/SimplicityStudio/SDKs/gecko_sdk_4.4.0//app/common/util/app_log" -I"C:/Users/Joel/SimplicityStudio/SDKs/gecko_sdk_4.4.0//platform/common/inc" -I"C:/Users/Joel/SimplicityStudio/SDKs/gecko_sdk_4.4.0//platform/CMSIS/Core/Include" -I"C:/Users/Joel/SimplicityStudio/SDKs/gecko_sdk_4.4.0//platform/driver/debug/inc" -I"C:/Users/Joel/SimplicityStudio/SDKs/gecko_sdk_4.4.0//platform/service/device_init/inc" -I"C:/Users/Joel/SimplicityStudio/SDKs/gecko_sdk_4.4.0//platform/emlib/inc" -I"C:/Users/Joel/SimplicityStudio/SDKs/gecko_sdk_4.4.0//platform/service/iostream/inc" -I"C:/Users/Joel/SimplicityStudio/SDKs/gecko_sdk_4.4.0//platform/common/toolchain/inc" -I"C:/Users/Joel/SimplicityStudio/SDKs/gecko_sdk_4.4.0//platform/service/system/inc" -I"C:/Users/Joel/SimplicityStudio/SDKs/gecko_sdk_4.4.0//platform/service/sleeptimer/inc" -Os -Wall -Wextra -mno-sched-prolog -fno-builtin -ffunction-sections -fdata-sections -imacrossl_gcc_preinclude.h -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mcmse --specs=nano.specs -c -fmessage-length=0 -MMD -MP -MF"gecko_sdk_4.4.1/platform/common/src/sl_assert.d" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

gecko_sdk_4.4.1/platform/common/src/sl_syscalls.o: C:/Users/Joel/SimplicityStudio/SDKs/gecko_sdk_4.4.0/platform/common/src/sl_syscalls.c gecko_sdk_4.4.1/platform/common/src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m33 -mthumb -std=c99 '-DDEBUG=1' '-DDEBUG_EFM=1' '-DEFR32BG22C224F512IM40=1' '-DSL_COMPONENT_CATALOG_PRESENT=1' -I"C:\Braun_Area\FYDP\PresGo\Fob\FareFare\config" -I"C:\Braun_Area\FYDP\PresGo\Fob\FareFare\autogen" -I"C:\Braun_Area\FYDP\PresGo\Fob\FareFare" -I"C:/Users/Joel/SimplicityStudio/SDKs/gecko_sdk_4.4.0//platform/Device/SiliconLabs/EFR32BG22/Include" -I"C:/Users/Joel/SimplicityStudio/SDKs/gecko_sdk_4.4.0//app/common/util/app_log" -I"C:/Users/Joel/SimplicityStudio/SDKs/gecko_sdk_4.4.0//platform/common/inc" -I"C:/Users/Joel/SimplicityStudio/SDKs/gecko_sdk_4.4.0//platform/CMSIS/Core/Include" -I"C:/Users/Joel/SimplicityStudio/SDKs/gecko_sdk_4.4.0//platform/driver/debug/inc" -I"C:/Users/Joel/SimplicityStudio/SDKs/gecko_sdk_4.4.0//platform/service/device_init/inc" -I"C:/Users/Joel/SimplicityStudio/SDKs/gecko_sdk_4.4.0//platform/emlib/inc" -I"C:/Users/Joel/SimplicityStudio/SDKs/gecko_sdk_4.4.0//platform/service/iostream/inc" -I"C:/Users/Joel/SimplicityStudio/SDKs/gecko_sdk_4.4.0//platform/common/toolchain/inc" -I"C:/Users/Joel/SimplicityStudio/SDKs/gecko_sdk_4.4.0//platform/service/system/inc" -I"C:/Users/Joel/SimplicityStudio/SDKs/gecko_sdk_4.4.0//platform/service/sleeptimer/inc" -Os -Wall -Wextra -mno-sched-prolog -fno-builtin -ffunction-sections -fdata-sections -imacrossl_gcc_preinclude.h -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mcmse --specs=nano.specs -c -fmessage-length=0 -MMD -MP -MF"gecko_sdk_4.4.1/platform/common/src/sl_syscalls.d" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


