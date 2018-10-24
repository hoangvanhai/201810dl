################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Sources/subdir.mk
-include KSDK_Files/subdir.mk
-include Debug_Console/subdir.mk
-include BSP_Files/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: pre-build main-build

# Main-build Target
main-build: sdcard.elf secondary-outputs

# Tool invocations
sdcard.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: Cross ARM C Linker'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall -Wno-missing-braces  -g3 -gdwarf-2 -gstrict-dwarf -gstrict-dwarf -gstrict-dwarf -T C:/Freescale/KSDK_1.3.0/middleware/filesystem/mfs/examples/sdcard/build/kds/sdcard_twrk64f120m/../../../../../../../../platform/devices/MK64F12/linker/gcc/MK64FN1M0xxx12_flash.ld -nostartfiles -nodefaultlibs -Xlinker --gc-sections -LC:/Freescale/KSDK_1.3.0/middleware/filesystem/mfs/examples/sdcard/build/kds/sdcard_twrk64f120m/../../../../../../../../lib/ksdk_mqx_lib/kds/K64F12/debug -LC:/Freescale/KSDK_1.3.0/middleware/filesystem/mfs/examples/sdcard/build/kds/sdcard_twrk64f120m/../../../../../lib/twrk64f120m.kds/debug/mfs -LC:/Freescale/KSDK_1.3.0/middleware/filesystem/mfs/examples/sdcard/build/kds/sdcard_twrk64f120m/../../../../../../../../rtos/mqx/lib/twrk64f120m.kds/debug/nshell -LC:/Freescale/KSDK_1.3.0/middleware/filesystem/mfs/examples/sdcard/build/kds/sdcard_twrk64f120m/../../../../../../../../rtos/mqx/lib/twrk64f120m.kds/debug/mqx -LC:/Freescale/KSDK_1.3.0/middleware/filesystem/mfs/examples/sdcard/build/kds/sdcard_twrk64f120m/../../../../../../../../rtos/mqx/lib/twrk64f120m.kds/debug/mqx_stdlib -Wl,-Map,"sdcard.map" -Xlinker --cref -Xlinker -static  -Xlinker -z -Xlinker muldefs  -Xlinker --undefined=__isr_vector  -Xlinker --defsym=__stack_size__=0x400  -Xlinker --defsym=__heap_size__=0x200 -o "sdcard.elf" -Xlinker --start-group $(OBJS) $(USER_OBJS) $(LIBS) -Xlinker --end-group
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(ASM_DEPS)$(S_UPPER_DEPS) sdcard.elf
	-@echo ' '

pre-build:
	-C:/Freescale/KSDK_1.3.0/middleware/filesystem/mfs/examples/sdcard/build/kds/sdcard_twrk64f120m/makedir.bat "C:/Freescale/KSDK_1.3.0/middleware/filesystem/mfs/examples/sdcard/build/kds/sdcard_twrk64f120m/int flash debug"
	-@echo ' '

secondary-outputs:

.PHONY: all clean dependents
.SECONDARY: main-build pre-build

-include ../makefile.targets
