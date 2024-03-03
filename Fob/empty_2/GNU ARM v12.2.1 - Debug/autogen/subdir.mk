################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../autogen/sl_board_default_init.c \
../autogen/sl_device_init_clocks.c \
../autogen/sl_event_handler.c \
../autogen/sl_iostream_handles.c 

OBJS += \
./autogen/sl_board_default_init.o \
./autogen/sl_device_init_clocks.o \
./autogen/sl_event_handler.o \
./autogen/sl_iostream_handles.o 

C_DEPS += \
./autogen/sl_board_default_init.d \
./autogen/sl_device_init_clocks.d \
./autogen/sl_event_handler.d \
./autogen/sl_iostream_handles.d 


# Each subdirectory must supply rules for building sources it contributes
autogen/sl_board_default_init.o: ../autogen/sl_board_default_init.c autogen/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m33 -mthumb -std=c99 '-DDEBUG=1' '-DDEBUG_EFM=1' '-DEFR32BG22C224F512IM40=1' '-DSL_BOARD_NAME="BRD4184B"' '-DSL_BOARD_REV="A04"' '-DSL_COMPONENT_CATALOG_PRESENT=1' -I"C:\Users\Andy\SimplicityStudio\v5_workspace\empty_2\config" -I"C:\Users\Andy\SimplicityStudio\v5_workspace\empty_2\autogen" -I"C:\Users\Andy\SimplicityStudio\v5_workspace\empty_2" -I"C:/Users/Andy/SimplicityStudio/SDKs/gecko_sdk//platform/Device/SiliconLabs/EFR32BG22/Include" -I"C:/Users/Andy/SimplicityStudio/SDKs/gecko_sdk//app/common/util/app_log" -I"C:/Users/Andy/SimplicityStudio/SDKs/gecko_sdk//hardware/board/inc" -I"C:/Users/Andy/SimplicityStudio/SDKs/gecko_sdk//platform/CMSIS/Core/Include" -I"C:/Users/Andy/SimplicityStudio/SDKs/gecko_sdk//platform/driver/debug/inc" -I"C:/Users/Andy/SimplicityStudio/SDKs/gecko_sdk//platform/service/device_init/inc" -I"C:/Users/Andy/SimplicityStudio/SDKs/gecko_sdk//platform/emlib/inc" -I"C:/Users/Andy/SimplicityStudio/SDKs/gecko_sdk//platform/common/inc" -I"C:/Users/Andy/SimplicityStudio/SDKs/gecko_sdk//platform/service/iostream/inc" -I"C:/Users/Andy/SimplicityStudio/SDKs/gecko_sdk//hardware/driver/mx25_flash_shutdown/inc/sl_mx25_flash_shutdown_usart" -I"C:/Users/Andy/SimplicityStudio/SDKs/gecko_sdk//platform/common/toolchain/inc" -I"C:/Users/Andy/SimplicityStudio/SDKs/gecko_sdk//platform/service/system/inc" -I"C:/Users/Andy/SimplicityStudio/SDKs/gecko_sdk//platform/service/udelay/inc" -Os -Wall -Wextra -mno-sched-prolog -fno-builtin -ffunction-sections -fdata-sections -imacrossl_gcc_preinclude.h -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mcmse -c -fmessage-length=0 -MMD -MP -MF"autogen/sl_board_default_init.d" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

autogen/sl_device_init_clocks.o: ../autogen/sl_device_init_clocks.c autogen/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m33 -mthumb -std=c99 '-DDEBUG=1' '-DDEBUG_EFM=1' '-DEFR32BG22C224F512IM40=1' '-DSL_BOARD_NAME="BRD4184B"' '-DSL_BOARD_REV="A04"' '-DSL_COMPONENT_CATALOG_PRESENT=1' -I"C:\Users\Andy\SimplicityStudio\v5_workspace\empty_2\config" -I"C:\Users\Andy\SimplicityStudio\v5_workspace\empty_2\autogen" -I"C:\Users\Andy\SimplicityStudio\v5_workspace\empty_2" -I"C:/Users/Andy/SimplicityStudio/SDKs/gecko_sdk//platform/Device/SiliconLabs/EFR32BG22/Include" -I"C:/Users/Andy/SimplicityStudio/SDKs/gecko_sdk//app/common/util/app_log" -I"C:/Users/Andy/SimplicityStudio/SDKs/gecko_sdk//hardware/board/inc" -I"C:/Users/Andy/SimplicityStudio/SDKs/gecko_sdk//platform/CMSIS/Core/Include" -I"C:/Users/Andy/SimplicityStudio/SDKs/gecko_sdk//platform/driver/debug/inc" -I"C:/Users/Andy/SimplicityStudio/SDKs/gecko_sdk//platform/service/device_init/inc" -I"C:/Users/Andy/SimplicityStudio/SDKs/gecko_sdk//platform/emlib/inc" -I"C:/Users/Andy/SimplicityStudio/SDKs/gecko_sdk//platform/common/inc" -I"C:/Users/Andy/SimplicityStudio/SDKs/gecko_sdk//platform/service/iostream/inc" -I"C:/Users/Andy/SimplicityStudio/SDKs/gecko_sdk//hardware/driver/mx25_flash_shutdown/inc/sl_mx25_flash_shutdown_usart" -I"C:/Users/Andy/SimplicityStudio/SDKs/gecko_sdk//platform/common/toolchain/inc" -I"C:/Users/Andy/SimplicityStudio/SDKs/gecko_sdk//platform/service/system/inc" -I"C:/Users/Andy/SimplicityStudio/SDKs/gecko_sdk//platform/service/udelay/inc" -Os -Wall -Wextra -mno-sched-prolog -fno-builtin -ffunction-sections -fdata-sections -imacrossl_gcc_preinclude.h -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mcmse -c -fmessage-length=0 -MMD -MP -MF"autogen/sl_device_init_clocks.d" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

autogen/sl_event_handler.o: ../autogen/sl_event_handler.c autogen/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m33 -mthumb -std=c99 '-DDEBUG=1' '-DDEBUG_EFM=1' '-DEFR32BG22C224F512IM40=1' '-DSL_BOARD_NAME="BRD4184B"' '-DSL_BOARD_REV="A04"' '-DSL_COMPONENT_CATALOG_PRESENT=1' -I"C:\Users\Andy\SimplicityStudio\v5_workspace\empty_2\config" -I"C:\Users\Andy\SimplicityStudio\v5_workspace\empty_2\autogen" -I"C:\Users\Andy\SimplicityStudio\v5_workspace\empty_2" -I"C:/Users/Andy/SimplicityStudio/SDKs/gecko_sdk//platform/Device/SiliconLabs/EFR32BG22/Include" -I"C:/Users/Andy/SimplicityStudio/SDKs/gecko_sdk//app/common/util/app_log" -I"C:/Users/Andy/SimplicityStudio/SDKs/gecko_sdk//hardware/board/inc" -I"C:/Users/Andy/SimplicityStudio/SDKs/gecko_sdk//platform/CMSIS/Core/Include" -I"C:/Users/Andy/SimplicityStudio/SDKs/gecko_sdk//platform/driver/debug/inc" -I"C:/Users/Andy/SimplicityStudio/SDKs/gecko_sdk//platform/service/device_init/inc" -I"C:/Users/Andy/SimplicityStudio/SDKs/gecko_sdk//platform/emlib/inc" -I"C:/Users/Andy/SimplicityStudio/SDKs/gecko_sdk//platform/common/inc" -I"C:/Users/Andy/SimplicityStudio/SDKs/gecko_sdk//platform/service/iostream/inc" -I"C:/Users/Andy/SimplicityStudio/SDKs/gecko_sdk//hardware/driver/mx25_flash_shutdown/inc/sl_mx25_flash_shutdown_usart" -I"C:/Users/Andy/SimplicityStudio/SDKs/gecko_sdk//platform/common/toolchain/inc" -I"C:/Users/Andy/SimplicityStudio/SDKs/gecko_sdk//platform/service/system/inc" -I"C:/Users/Andy/SimplicityStudio/SDKs/gecko_sdk//platform/service/udelay/inc" -Os -Wall -Wextra -mno-sched-prolog -fno-builtin -ffunction-sections -fdata-sections -imacrossl_gcc_preinclude.h -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mcmse -c -fmessage-length=0 -MMD -MP -MF"autogen/sl_event_handler.d" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

autogen/sl_iostream_handles.o: ../autogen/sl_iostream_handles.c autogen/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m33 -mthumb -std=c99 '-DDEBUG=1' '-DDEBUG_EFM=1' '-DEFR32BG22C224F512IM40=1' '-DSL_BOARD_NAME="BRD4184B"' '-DSL_BOARD_REV="A04"' '-DSL_COMPONENT_CATALOG_PRESENT=1' -I"C:\Users\Andy\SimplicityStudio\v5_workspace\empty_2\config" -I"C:\Users\Andy\SimplicityStudio\v5_workspace\empty_2\autogen" -I"C:\Users\Andy\SimplicityStudio\v5_workspace\empty_2" -I"C:/Users/Andy/SimplicityStudio/SDKs/gecko_sdk//platform/Device/SiliconLabs/EFR32BG22/Include" -I"C:/Users/Andy/SimplicityStudio/SDKs/gecko_sdk//app/common/util/app_log" -I"C:/Users/Andy/SimplicityStudio/SDKs/gecko_sdk//hardware/board/inc" -I"C:/Users/Andy/SimplicityStudio/SDKs/gecko_sdk//platform/CMSIS/Core/Include" -I"C:/Users/Andy/SimplicityStudio/SDKs/gecko_sdk//platform/driver/debug/inc" -I"C:/Users/Andy/SimplicityStudio/SDKs/gecko_sdk//platform/service/device_init/inc" -I"C:/Users/Andy/SimplicityStudio/SDKs/gecko_sdk//platform/emlib/inc" -I"C:/Users/Andy/SimplicityStudio/SDKs/gecko_sdk//platform/common/inc" -I"C:/Users/Andy/SimplicityStudio/SDKs/gecko_sdk//platform/service/iostream/inc" -I"C:/Users/Andy/SimplicityStudio/SDKs/gecko_sdk//hardware/driver/mx25_flash_shutdown/inc/sl_mx25_flash_shutdown_usart" -I"C:/Users/Andy/SimplicityStudio/SDKs/gecko_sdk//platform/common/toolchain/inc" -I"C:/Users/Andy/SimplicityStudio/SDKs/gecko_sdk//platform/service/system/inc" -I"C:/Users/Andy/SimplicityStudio/SDKs/gecko_sdk//platform/service/udelay/inc" -Os -Wall -Wextra -mno-sched-prolog -fno-builtin -ffunction-sections -fdata-sections -imacrossl_gcc_preinclude.h -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mcmse -c -fmessage-length=0 -MMD -MP -MF"autogen/sl_iostream_handles.d" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


