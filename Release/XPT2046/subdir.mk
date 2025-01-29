################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../XPT2046/calibrate_touch.c \
../XPT2046/demo.c \
../XPT2046/xpt2046.c 

OBJS += \
./XPT2046/calibrate_touch.o \
./XPT2046/demo.o \
./XPT2046/xpt2046.o 

C_DEPS += \
./XPT2046/calibrate_touch.d \
./XPT2046/demo.d \
./XPT2046/xpt2046.d 


# Each subdirectory must supply rules for building sources it contributes
XPT2046/%.o XPT2046/%.su XPT2046/%.cyclo: ../XPT2046/%.c XPT2046/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0plus -std=gnu11 -DSTM32G031xx -DUSE_FULL_LL_DRIVER -DHSE_VALUE=8000000 -DHSE_STARTUP_TIMEOUT=100 -DLSE_STARTUP_TIMEOUT=5000 -DLSE_VALUE=32768 -DEXTERNAL_CLOCK_VALUE=12288000 -DHSI_VALUE=16000000 -DLSI_VALUE=32000 -DVDD_VALUE=3300 -DPREFETCH_ENABLE=1 -DINSTRUCTION_CACHE_ENABLE=1 -DDATA_CACHE_ENABLE=1 -c -I../Core/Inc -I../Drivers/STM32G0xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32G0xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/Vadim/STM32Cube/Repository/stm32g031f6p6_spi_dma_ili9341_xpt2046_demo/Display" -I"C:/Users/Vadim/STM32Cube/Repository/stm32g031f6p6_spi_dma_ili9341_xpt2046_demo/XPT2046" -O2 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-XPT2046

clean-XPT2046:
	-$(RM) ./XPT2046/calibrate_touch.cyclo ./XPT2046/calibrate_touch.d ./XPT2046/calibrate_touch.o ./XPT2046/calibrate_touch.su ./XPT2046/demo.cyclo ./XPT2046/demo.d ./XPT2046/demo.o ./XPT2046/demo.su ./XPT2046/xpt2046.cyclo ./XPT2046/xpt2046.d ./XPT2046/xpt2046.o ./XPT2046/xpt2046.su

.PHONY: clean-XPT2046

