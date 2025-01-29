################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../Core/Startup/startup_stm32g031f6px.s 

OBJS += \
./Core/Startup/startup_stm32g031f6px.o 

S_DEPS += \
./Core/Startup/startup_stm32g031f6px.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Startup/%.o: ../Core/Startup/%.s Core/Startup/subdir.mk
	arm-none-eabi-gcc -mcpu=cortex-m0plus -c -I"C:/Users/Vadim/STM32Cube/Repository/stm32g031f6p6_spi_dma_ili9341_xpt2046_demo/Display" -I"C:/Users/Vadim/STM32Cube/Repository/stm32g031f6p6_spi_dma_ili9341_xpt2046_demo/XPT2046" -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@" "$<"

clean: clean-Core-2f-Startup

clean-Core-2f-Startup:
	-$(RM) ./Core/Startup/startup_stm32g031f6px.d ./Core/Startup/startup_stm32g031f6px.o

.PHONY: clean-Core-2f-Startup

