################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/haozhe/Desktop/MSC/ELEC5620M/github/ELEC5620M-Resources/Drivers/HPS_I2C/HPS_I2C.c 

C_DEPS += \
./Drivers/HPS_I2C/HPS_I2C.d 

OBJS += \
./Drivers/HPS_I2C/HPS_I2C.o 


# Each subdirectory must supply rules for building sources it contributes
Drivers/HPS_I2C/HPS_I2C.o: C:/Users/haozhe/Desktop/MSC/ELEC5620M/github/ELEC5620M-Resources/Drivers/HPS_I2C/HPS_I2C.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM C Compiler 5'
	armcc --cpu=Cortex-A9.no_neon.no_vfp --arm -DHARDWARE_OPTIMISED -I"C:\Users\haozhe\Desktop\MSC\ELEC5620M\github\ELEC5620M-Resources\Drivers" -O0 -g --md --depend_format=unix_escaped --no_depend_system_headers --depend_dir="Drivers/HPS_I2C" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


