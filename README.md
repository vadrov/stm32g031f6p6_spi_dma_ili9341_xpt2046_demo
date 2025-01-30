Copyright (C) 2019, VadRov, all right reserved / www.youtube.com/@VadRov / www.dzen.ru/vadrov
# XPT2046 controller driver (stm32g0 series, ili9341, xpt2046, dma, irq, temperature, voltage)
 XPT2046 controller driver (HR2046 and other compatible). Supports all the functionality of the controller: work with the touchscreen, temperature measurement by an internal sensor, voltage measurement at the VBAT and AUX inputs.
 For STM32G0 Series
 
**Functions and features:**
- Touch screen polling (in and out of the interrupt) with information about the coordinates and duration of the touch;
- Determination of the status of the current touch of the touchscreen: no touch, click, hold;
- Temperature measurement by built-in sensor;
- Measurement of battery voltage at the input of the VBAT controller (7 pin m/s in the TSSOP-16 housing);
- Measurement of the voltage at the AUX input of the controller (8 pin m/s in the TSSOP-16 housing).
- Allows operation on one spi of several devices.

![Image](https://github.com/user-attachments/assets/02b638e7-df36-41ff-a0b8-3ab262609c03)

**The project is built for the stm32G031F6P6 microcontroller ([STM32G0xx Core Board](https://github.com/WeActStudio/WeActStudio.STM32G0xxC0xxCoreBoard)).**
**Connection table of the debug board based on the stm32g031f6p6 display module with the ili9341 touchscreen:**
```
ILI9341 module      MCU stm32g031f6p6
T_IRQ                 PB7
T_OUT                 PA6
T_DIN, SDI            PA2
T_CS                  PA7
T_CLK, SCK            PA1
SDO                   ---
LED                   PB4 (in board PB6)
D/C                   PA0
RESET                 PA5
CS                    PA3
GND                   GND
VCC                   3V3
```
## Usage:
- In the STM32CubeIDE environment, create a new project for your stm32G0x MCU. Perform general project setup (clocking, debugging, etc.). Define spi for working with the controller. If another device, such as a display, is connected to this spi, then configure spi (set spi parameters) for the display used. Remember that XPT2046 requires a 4-wire spi (full-duplex mode). Define the T_CS (output, GPIO mode -> Output Push Pull) and T_IRQ (input with external interrupt on falling edge, GPIO mode -> External Interrupt Mode with Falling edge trigger detection) MCU pins. Pull the T_CS pin to the power supply using the internal pull-up (GPIO Pull-up/Pull-down -> Pull-up). If several devices are connected to one spi, then pull the CS pins of other devices to the power supply as well. In addition, set the high level on all CS outputs (GPIO output level -> High). Pull the T_IRQ pin to the power supply with the internal pull-up (GPIO Pull-up/Pull-down -> Pull-up), if your board does not have a resistor that pulls the PENIRQ pin of the XPT2046 controller to the power supply. Set the pin speed to High, the spi line speed to Very High.
- In file stm32g0xx_it.c
1. Include driver header file
```c
#include "xpt2046.h"
```
2. Add a call to the SysTick_Handler handler:
```c
 XPT2046_TIMCallback(touch);
```
3. Add the following call to the external interrupt handler EXTIxx_IRQHandler (xx defines the interrupt line):
```c
 XPT2046_EXTICallback(touch);
 ```
- Define the parameters for connecting the MK to the XPT2046 controller:
```c
 XPT2046_ConnectionData cnt_touch = {	.spi 	  = SPI1, //Used spi
					.speed 	  = 4, //Speed ​​spi 0...7 (0 - clk/2, 1 - clk/4, ..., 7 - clk/256)
					.cs_port  = T_CS_GPIO_Port, //T_CS control port
					.cs_pin	  = T_CS_Pin, //T_CS control pin
					.irq_port = T_IRQ_GPIO_Port, //T_IRQ control port
					.irq_pin  = T_IRQ_Pin, //T_IRQ control pin
					.exti_irq = T_IRQ_EXTI_IRQn //External interrupt channel for T_IRQ
};
```
- Declare the XPT2046 handler variable:
```c
 XPT2046_Handler touch1;
```
- Initialize XPT2046 handler:
```c
 XPT2046_InitTouch(&touch1, 20, &cnt_touch);
```
Parameter 20 defines the counting period of the internal handler timer. The handler timer is tied to the system timer, which should be set to generate interrupts 1000 times per second. Thus, the touchscreen will be polled in the interrupt 1000/20 = 50 times per second.
- Calibrate the touchscreen:
```c
  XPT2046_CalibrateTouch(&touch1, lcd); //Start the calibration procedure
```
The touchscreen can be polled in the driver interrupt (touch1.fl_interrupt = 1) or in the user program (touch1.fl_interrupt = 0). If the poll is performed outside the driver interrupt, it is necessary to call in the user program before receiving the status and touch coordinates:
```c
uint8_t res = XPT2046_GetTouch(&touch1); //res = 0 - poll successful, res = 1 - no touch, res = 2 - spi busy
```
- To get touch coordinates:
```c
tPoint point_d; //touch coordinates on the display: x corresponds to point_d.x, and y corresponds to point_d.y
XPT2046_ConvertPoint(&point_d, &touch1.point, &touch1.coef); //Convert touchscreen coordinates to display coordinates
```
The current touch status is determined by the touch1.status parameter. If touch1.status = XPT2046_STATUS_NOCLICK, then there is no click; touch1.status = XPT2046_STATUS_CLICK - there is a click, touch1.status = XPT2046_STATUS_HOLD - there is a hold. The time it takes for the touch to transition from the "click" status to the "hold" status is determined by the XPT2046_TOUCH_HOLD_TIME parameter (file xpt2046.h).

Author: **VadRov**

Contacts: [Youtube](https://www.youtube.com/@VadRov) [Дзен](https://dzen.ru/vadrov) [VK](https://vk.com/vadrov) [Telegram](https://t.me/vadrov_channel)

Donate [donate.yoomoney](https://yoomoney.ru/to/4100117522443917)
