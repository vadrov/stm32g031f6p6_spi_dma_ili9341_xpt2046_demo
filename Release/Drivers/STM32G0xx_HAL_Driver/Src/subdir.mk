################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_ll_dma.c \
../Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_ll_exti.c \
../Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_ll_gpio.c \
../Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_ll_pwr.c \
../Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_ll_rcc.c \
../Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_ll_spi.c \
../Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_ll_tim.c \
../Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_ll_utils.c 

OBJS += \
./Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_ll_dma.o \
./Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_ll_exti.o \
./Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_ll_gpio.o \
./Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_ll_pwr.o \
./Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_ll_rcc.o \
./Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_ll_spi.o \
./Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_ll_tim.o \
./Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_ll_utils.o 

C_DEPS += \
./Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_ll_dma.d \
./Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_ll_exti.d \
./Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_ll_gpio.d \
./Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_ll_pwr.d \
./Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_ll_rcc.d \
./Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_ll_spi.d \
./Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_ll_tim.d \
./Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_ll_utils.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/STM32G0xx_HAL_Driver/Src/%.o Drivers/STM32G0xx_HAL_Driver/Src/%.su Drivers/STM32G0xx_HAL_Driver/Src/%.cyclo: ../Drivers/STM32G0xx_HAL_Driver/Src/%.c Drivers/STM32G0xx_HAL_Driver/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0plus -std=gnu11 -DSTM32G031xx -DUSE_FULL_LL_DRIVER -DHSE_VALUE=8000000 -DHSE_STARTUP_TIMEOUT=100 -DLSE_STARTUP_TIMEOUT=5000 -DLSE_VALUE=32768 -DEXTERNAL_CLOCK_VALUE=12288000 -DHSI_VALUE=16000000 -DLSI_VALUE=32000 -DVDD_VALUE=3300 -DPREFETCH_ENABLE=1 -DINSTRUCTION_CACHE_ENABLE=1 -DDATA_CACHE_ENABLE=1 -c -I../Core/Inc -I../Drivers/STM32G0xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32G0xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/Vadim/STM32Cube/Repository/stm32g031f6p6_spi_dma_ili9341_xpt2046_demo/Display" -I"C:/Users/Vadim/STM32Cube/Repository/stm32g031f6p6_spi_dma_ili9341_xpt2046_demo/XPT2046" -O2 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Drivers-2f-STM32G0xx_HAL_Driver-2f-Src

clean-Drivers-2f-STM32G0xx_HAL_Driver-2f-Src:
	-$(RM) ./Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_ll_dma.cyclo ./Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_ll_dma.d ./Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_ll_dma.o ./Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_ll_dma.su ./Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_ll_exti.cyclo ./Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_ll_exti.d ./Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_ll_exti.o ./Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_ll_exti.su ./Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_ll_gpio.cyclo ./Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_ll_gpio.d ./Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_ll_gpio.o ./Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_ll_gpio.su ./Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_ll_pwr.cyclo ./Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_ll_pwr.d ./Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_ll_pwr.o ./Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_ll_pwr.su ./Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_ll_rcc.cyclo ./Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_ll_rcc.d ./Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_ll_rcc.o ./Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_ll_rcc.su ./Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_ll_spi.cyclo ./Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_ll_spi.d ./Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_ll_spi.o ./Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_ll_spi.su ./Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_ll_tim.cyclo ./Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_ll_tim.d ./Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_ll_tim.o ./Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_ll_tim.su ./Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_ll_utils.cyclo ./Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_ll_utils.d ./Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_ll_utils.o ./Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_ll_utils.su

.PHONY: clean-Drivers-2f-STM32G0xx_HAL_Driver-2f-Src

