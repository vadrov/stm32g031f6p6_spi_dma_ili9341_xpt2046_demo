/*
 *  Author: VadRov
 *  Copyright (C) 2019, VadRov, all right reserved.
 *
 *  Драйвер для контроллера XPT2046 (HR2046 и т.п.)
 *
 *  Для микроконтроллеров stm32G0x
 *
 *  Функции и возможности:
 *  - Опрос тачскрина (в прерывании и вне прерывания) с получением информации о координатах и длительности касания;
 *  - Определение статуса текущего касания тачскрина: нет касания, клик, удержание;
 * 	- Измерение температуры встроенным датчиком;
 * 	- Измерение напряжения батареи на входе VBAT контроллера (7 вывод м/с в корпусе TSSOP-16);
 * 	- Измерение напряжения на входе AUX контроллера (8 вывод м/с в корпусе TSSOP-16).
 * 	- Допускает работу на одном spi нескольких устройств.
 *
 *  Допускается свободное распространение без целей коммерческого использования.
 *  При коммерческом использовании необходимо согласование с автором.
 *  Распространятся по типу "как есть", то есть использование осуществляете на свой страх и риск.
 *  Автор не предоставляет никаких гарантий.
 *
 *  https://www.youtube.com/@VadRov
 *  https://dzen.ru/vadrov
 *  https://vk.com/vadrov
 *  https://t.me/vadrov_channel
 *
 */

#ifndef INC_XPT2046_H_
#define INC_XPT2046_H_

#include "main.h"

/*************************************** Настройки пользователя ***************************************/
/*
 * Количество повторов измерений напряжений, соответствующих координатам x и у точки касания тачскрина.
 * Этот параметр включает одно "холостое" измерение, неиспользуемое в расчете среднего (отбрасывается).
 * Рекомендуется не менее 9 повторов измерений (8 "полезных" + 1 "холостое")
 */
#define XPT2046_TOUCH_REPEATED_MEASUREMENTS		17

/*
 * Длительность текущего касания тачскрина (в миллисекундах), при которой
 * такое касание будет распознаваться, как удержание
 */
#define XPT2046_TOUCH_HOLD_TIME					1500

/*
 * Количество повторов измерений напряжений для определения других параметров (температура, напряжение на
 * входах VBAT и AUX).
 * Этот параметр включает одно "холостое" измерение, неиспользуемое в расчете среднего (отбрасывается).
 * Рекомендуется не менее 9 повторов измерений (8 "полезных" + 1 "холостое")
 */
#define XPT2046_OTHER_REPEATED_MEASUREMENTS		17

/*
 * Опорное напряжение, мВ.
 * В контроллере XPT2046 есть внутренний источник опорного напряжения 2500 мВ (+/- 50 мВ).
 * Это напряжение может быть использовано АЦП при преобразовании.
 * Подключение внутреннего источника опорного напряжения определяет бит PD1 в управляющей команде.
 * Возможно подключение и использование (путем отключения внутреннего источника опорного напряжения
 * битом PD1 в управляющей команде) внешнего источника опорного напряжения (см. схему своей платы,
 * 9 вывод микросхемы в корпусе TSSOP-16).
 * В спецификации XPT2046 указано, что для обеспечения совместимости с контроллером тачскрина ADS7843
 * (не имеет внутреннего источника опорного напряжения) внутренее опорное напряжение XPT2046 должно
 * быть отключено при измерении напряжений на сенсоре (PD1 = 0).
 * В этом драйвере может быть использован, как внутренний, так и внешний источники опорного напряжения.
 */
#define XPT2046_INTERNAL_VREF //Используем внутренний источник опорного напряжения.
							  //Если используется внешний источник опорного напряжения,
							  //то строку закомментируйте.

#ifdef XPT2046_INTERNAL_VREF
#define XPT2046_VREF  2550 //Напряжение внутреннего источника опорного напряжения.
						   //Оно определено в спецификации на контроллер XPT2046 (2500 мВ +/- 50 мВ).
#else
#define XPT2046_VREF  2900 //Напряжение внешнего источника опорного напряжения.
						   //Измеряется на 9 выводе микросхемы XPT2046 в корпусе TSSOP-16.
						   //Здесь напряжение для моей платы. У вас может быть немного иным.
#endif
/*********************************** Конец настроек пользователя *************************************/


/*************************** Структура управляющей команды контроллера XPT2046 *****************************
 * BIT7    BIT6    BIT5    BIT4    BIT3    BIT2    BIT1    BIT0
 *   S      A2      A1      A0     MODE   SER/DFR   PD1     PD0
 * S - Стартовый бит. Управляющая команда идентифицируется именно по установленному в 1 биту S.
 * A2 - A0 - биты выбора канала мультиплексора АЦП (какой вход подключать к АЦП).
 * MODE - бит выбора разрядности преобразования (0 - 12-битное, 1 - 8-битное).
 * SER/DFR - бит выбора типа входа АЦП (0 - дифференциальный, 1 - несимметричный).
 * Для получения данных с тачскрина следует использовать дифференциальный вход.
 * Для измерения абсолютных значений напряжения на внутреннем датчике температуры и
 * входах VBAT и AUX следует использовать несимметричный вход АЦП.
 * PD1 - бит включения и выключения внутреннего опорного напряжения:
 *    0 - выключено, используется внешнее опорное напряжение (подключается к 9 выводу XPT2046 в корпусе TSSOP-16),
 *    1 - включено, используется внутреннее опорное напряжение (встроенный в XPT2046 источник VREF на 2500 мВ +/- 50 мВ).
 * PD0 - бит управления выключением АЦП после преобразования (0 - выключать, 1 - не выключать).
 */

/************************** Команды контроллера XPT2046 для 8-битного spi ********************************/
#define XPT2046_NOP 	0x00 //00000000 - "холостая" команда (стартовый бит S сброшен)
#define XPT2046_INI		0x80 //10000000 - выключение АЦП и вн. опорного напряжения после преобразования
							 //для разрешения PENIRQ - т.н. "инициализация".
#ifdef XPT2046_INTERNAL_VREF
/* 12 битное преобразование, внутренний источник опорного напряжения, дифференциальный вход АЦП,
 * АЦП не выключается после преобразования */
#define XPT2046_Y 		0x93 //10010011 - измерение напряжения на сенсоре для координаты Y
#define XPT2046_X 		0xD3 //11010011 - измерение напряжения на сенсоре для координаты X
/* 12 битное преобразование, внутренний источник опорного напряжения, несимметричный вход АЦП,
 * АЦП не выключается после преобразования */
#define XPT2046_TEMP0 	0x87 //10000111 - первое измерение напряжения для определения температуры
#define XPT2046_TEMP1 	0xF7 //11110111 - второе измерение напряжения (с увеличенным током)
#define XPT2046_VBAT	0xA7 //10100111 - измерение напряжения на входе VBAT
#define XPT2046_AUX		0xE7 //11100111 - измерение напряжения на входe AUX
#else
/* 12 битное преобразование, внешний источник опорного напряжения, дифференциальный вход АЦП,
 * АЦП не выключается после преобразования */
#define XPT2046_Y 		0x91 //10010001 - измерение напряжения на сенсоре для координаты Y
#define XPT2046_X 		0xD1 //11010001 - измерение напряжения на сенсоре для координаты X
/* 12 битное преобразование, внешний источник опорного напряжения, несимметричный вход АЦП,
 * АЦП не выключается после преобразования */
#define XPT2046_TEMP0 	0x85 //10000101 - первое измерение напряжения для определения температуры
#define XPT2046_TEMP1 	0xF5 //11110101 - второе измерение напряжения (с увеличенным током)
#define XPT2046_VBAT	0xA5 //10100101 - измерение напряжения на входе VBAT
#define XPT2046_AUX		0xE5 //11100101 - измерение напряжения на входe AUX
#endif
/*********************************************************************************************************/

/* Статусы тачскрина */
typedef enum {
	XPT2046_STATUS_NOCLICK = 0,  //нет клика
	XPT2046_STATUS_CLICK,		 //клик
	XPT2046_STATUS_HOLD			 //удержание
} XPT2046_status;

/* Данные подключения XPT2046 */
typedef struct {
	SPI_TypeDef *spi;		//spi
	uint8_t speed;			//Скорость spi (от 0 - clk/2 до 7 - clk/256)
	GPIO_TypeDef *cs_port;	//Порт с выводом T_CS
	uint16_t cs_pin;		//Номер вывода порта c T_CS (настроен как выход)
	GPIO_TypeDef *irq_port;	//Порт с выводом T_IRQ
	uint16_t irq_pin;		//Номер вывода порта с T_IRQ (настроен как вход с разрешением прерывания EXTI)
	IRQn_Type exti_irq;		//Внешнеее прерывание EXTI для вывода T_IRQ
} XPT2046_ConnectionData;

/* Данные с координатами точки касания */
typedef struct {
	int x, y;
} tPoint;

/* Коэффициенты для преобразования координат тачскрина в дисплейные координаты */
typedef struct {
	int64_t	Dx1, Dx2, Dx3, Dy1, Dy2, Dy3, D;
} tCoef;

/* Обработчик тачскрина */
typedef struct {
	XPT2046_ConnectionData cnt_data;//Данные подключения XPT2046
	tPoint point;					//Координаты последнего считанного касания (в координатах тачскрина)
	uint32_t last_click_time;   	//Длительность последнего касания, выраженная в количестве прерываний системного таймера,
									//произошедших от момента последнего касания тачскрина до момента отпускания тачскрина
	uint32_t click_time_counter;	//Счетчик длительности текущего касания (также выражается в количестве прерываний системного таймера)
	uint8_t fl_interrupt;			//Флаг разрешения обновления координат тачскрина в прерывании
	uint8_t click;					//Флаг, показывающий, есть или нет в текущий момент касание тачскрина
	tCoef coef;						//Коэффициенты преобразования координат тачскина в дисплейные координаты
	uint8_t fl_timer;				//Флаг, который разрешает или запрещает счет внутреннего таймера обработчика
	uint32_t timer_counter;			//Счетчик внутреннего таймера обработчика
	uint32_t timer_period;			//Период счетчика внутреннего таймера обработчика
	uint8_t fl_wait_exti;			//Флаг ожидания разрешения прерывания exti
	XPT2046_status status;			//Статус опроса тачскрина: нет клика, клик, удержание
} XPT2046_Handler;

/* Глобальная переменная с обработчиком тачскрина */
extern XPT2046_Handler *touch;

/* Обработчик прерывания EXTI
   (добавить в соответствующий обработчик EXTIx_IRQHandler - см. файл stm32f4xx_it.c) */
void XPT2046_EXTICallback(XPT2046_Handler *t);

/* Обработчик прерывания системного таймера
   (добавить в обработчик SysTick_Handler - см. файл stm32f4xx_it.c) */
void XPT2046_TIMCallback(XPT2046_Handler *t);

/* Инициализация обработчика тачскрина */
void XPT2046_InitTouch(XPT2046_Handler *t, uint32_t timer_update_period, XPT2046_ConnectionData *cnt_data);

/* Опрос тачскрина */
uint8_t XPT2046_GetTouch(XPT2046_Handler *t);

/* Измерение температуры встроенным в XPT2046 датчиком */
int XPT2046_GetTemp(XPT2046_Handler *t);

/* Измерение напряжения на входе AUX (8 вывод м/с в корпусе TSSOP-16) */
int XPT2046_GetAux(XPT2046_Handler *t);

/* Измерение напряжения батареи на входе VBAT (7 вывод м/с в корпусе TSSOP-16) */
int XPT2046_GetVbat(XPT2046_Handler *t);

#endif /* INC_XPT2046_H_ */
