################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/adc.c \
../Core/Src/bms_message.c \
../Core/Src/bms_safety_manager.c \
../Core/Src/bms_sensor.c \
../Core/Src/car.c \
../Core/Src/delay.c \
../Core/Src/direction.c \
../Core/Src/dma.c \
../Core/Src/gas2.c \
../Core/Src/gpio.c \
../Core/Src/i2c.c \
../Core/Src/ina219_bms.c \
../Core/Src/main.c \
../Core/Src/speed.c \
../Core/Src/statemachine.c \
../Core/Src/stm32f4xx_hal_msp.c \
../Core/Src/stm32f4xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32f4xx.c \
../Core/Src/temp.c \
../Core/Src/tim.c \
../Core/Src/trace.c \
../Core/Src/ultrasonic.c \
../Core/Src/usart.c 

OBJS += \
./Core/Src/adc.o \
./Core/Src/bms_message.o \
./Core/Src/bms_safety_manager.o \
./Core/Src/bms_sensor.o \
./Core/Src/car.o \
./Core/Src/delay.o \
./Core/Src/direction.o \
./Core/Src/dma.o \
./Core/Src/gas2.o \
./Core/Src/gpio.o \
./Core/Src/i2c.o \
./Core/Src/ina219_bms.o \
./Core/Src/main.o \
./Core/Src/speed.o \
./Core/Src/statemachine.o \
./Core/Src/stm32f4xx_hal_msp.o \
./Core/Src/stm32f4xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32f4xx.o \
./Core/Src/temp.o \
./Core/Src/tim.o \
./Core/Src/trace.o \
./Core/Src/ultrasonic.o \
./Core/Src/usart.o 

C_DEPS += \
./Core/Src/adc.d \
./Core/Src/bms_message.d \
./Core/Src/bms_safety_manager.d \
./Core/Src/bms_sensor.d \
./Core/Src/car.d \
./Core/Src/delay.d \
./Core/Src/direction.d \
./Core/Src/dma.d \
./Core/Src/gas2.d \
./Core/Src/gpio.d \
./Core/Src/i2c.d \
./Core/Src/ina219_bms.d \
./Core/Src/main.d \
./Core/Src/speed.d \
./Core/Src/statemachine.d \
./Core/Src/stm32f4xx_hal_msp.d \
./Core/Src/stm32f4xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32f4xx.d \
./Core/Src/temp.d \
./Core/Src/tim.d \
./Core/Src/trace.d \
./Core/Src/ultrasonic.d \
./Core/Src/usart.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/adc.cyclo ./Core/Src/adc.d ./Core/Src/adc.o ./Core/Src/adc.su ./Core/Src/bms_message.cyclo ./Core/Src/bms_message.d ./Core/Src/bms_message.o ./Core/Src/bms_message.su ./Core/Src/bms_safety_manager.cyclo ./Core/Src/bms_safety_manager.d ./Core/Src/bms_safety_manager.o ./Core/Src/bms_safety_manager.su ./Core/Src/bms_sensor.cyclo ./Core/Src/bms_sensor.d ./Core/Src/bms_sensor.o ./Core/Src/bms_sensor.su ./Core/Src/car.cyclo ./Core/Src/car.d ./Core/Src/car.o ./Core/Src/car.su ./Core/Src/delay.cyclo ./Core/Src/delay.d ./Core/Src/delay.o ./Core/Src/delay.su ./Core/Src/direction.cyclo ./Core/Src/direction.d ./Core/Src/direction.o ./Core/Src/direction.su ./Core/Src/dma.cyclo ./Core/Src/dma.d ./Core/Src/dma.o ./Core/Src/dma.su ./Core/Src/gas2.cyclo ./Core/Src/gas2.d ./Core/Src/gas2.o ./Core/Src/gas2.su ./Core/Src/gpio.cyclo ./Core/Src/gpio.d ./Core/Src/gpio.o ./Core/Src/gpio.su ./Core/Src/i2c.cyclo ./Core/Src/i2c.d ./Core/Src/i2c.o ./Core/Src/i2c.su ./Core/Src/ina219_bms.cyclo ./Core/Src/ina219_bms.d ./Core/Src/ina219_bms.o ./Core/Src/ina219_bms.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/speed.cyclo ./Core/Src/speed.d ./Core/Src/speed.o ./Core/Src/speed.su ./Core/Src/statemachine.cyclo ./Core/Src/statemachine.d ./Core/Src/statemachine.o ./Core/Src/statemachine.su ./Core/Src/stm32f4xx_hal_msp.cyclo ./Core/Src/stm32f4xx_hal_msp.d ./Core/Src/stm32f4xx_hal_msp.o ./Core/Src/stm32f4xx_hal_msp.su ./Core/Src/stm32f4xx_it.cyclo ./Core/Src/stm32f4xx_it.d ./Core/Src/stm32f4xx_it.o ./Core/Src/stm32f4xx_it.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32f4xx.cyclo ./Core/Src/system_stm32f4xx.d ./Core/Src/system_stm32f4xx.o ./Core/Src/system_stm32f4xx.su ./Core/Src/temp.cyclo ./Core/Src/temp.d ./Core/Src/temp.o ./Core/Src/temp.su ./Core/Src/tim.cyclo ./Core/Src/tim.d ./Core/Src/tim.o ./Core/Src/tim.su ./Core/Src/trace.cyclo ./Core/Src/trace.d ./Core/Src/trace.o ./Core/Src/trace.su ./Core/Src/ultrasonic.cyclo ./Core/Src/ultrasonic.d ./Core/Src/ultrasonic.o ./Core/Src/ultrasonic.su ./Core/Src/usart.cyclo ./Core/Src/usart.d ./Core/Src/usart.o ./Core/Src/usart.su

.PHONY: clean-Core-2f-Src

