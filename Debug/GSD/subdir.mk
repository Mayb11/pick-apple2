################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../GSD/GSD.c 

C_DEPS += \
./GSD/GSD.d 

OBJS += \
./GSD/GSD.o 


# Each subdirectory must supply rules for building sources it contributes
GSD/%.o: ../GSD/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM C Compiler 5'
	armcc --cpu=Cortex-A9.no_neon.no_vfp --arm -DHARDWARE_OPTIMISED -I"C:\Users\haozhe\Desktop\MSC\ELEC5620M\github\ELEC5620M-Resources\Drivers" -O0 -g --md --depend_format=unix_escaped --no_depend_system_headers --depend_dir="GSD" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


