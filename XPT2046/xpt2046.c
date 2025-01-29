/*
 *  Copyright (C) 2019, VadRov, all right reserved.
 *
 *  Драйвер для контроллера XPT2046 (HR2046 и т.п.)
 *
 *  Для микроконтроллеров stm32G0x
 *
 *  Функции и возможности:
 *  - Опрос тачскрина (в прерывании и вне прерывания) с получением информации о координатах и длительности касания;
 * 	- Измерение температуры встроенным датчиком;
 * 	- Измерение напряжения батареи на входе VBAT контроллера (7 вывод м/с в корпусе TSSOP-16);
 * 	- Измерение напряжения на входе AUX контроллера (8 вывод м/с в корпусе TSSOP-16);
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

#include <string.h>
#include "xpt2046.h"

XPT2046_Handler *touch = 0;

static uint8_t get_touch(XPT2046_Handler *t);
static int get_adc_in(XPT2046_Handler *t, uint8_t command, uint8_t repeat);
static void connect_on(XPT2046_ConnectionData *cnt_data, uint32_t *param);
static void connect_off(XPT2046_ConnectionData *cnt_data, uint32_t *param);

/*
 * Инициализация обработчика тачскрина
 * t - указатель на обработчик, созданный пользователем
 * timer_period - период счета внутреннего таймера обработчика
 * cnt_data - указатель на данные подключения XPT2046
 */
void XPT2046_InitTouch(XPT2046_Handler *t, uint32_t timer_period, XPT2046_ConnectionData *cnt_data)
{
	memset(t, 0, sizeof(XPT2046_Handler));  //Инициализация памяти, занятой обработчиком.
	t->timer_period = timer_period; 		//Инициализация периода счета внутреннего таймера обработчика.
	t->cnt_data = *cnt_data;				//Данные подключения XPT2046.
	touch = t;								//Присвоение глобальному указателю на обработчик
											//тачскрина адреса переданного обработчика.
											//Это присвоение запускает в работу обработчики прерываний:
											//- XPT2046_EXTICallback - в обработчике прерывания EXTI;
											//- XPT2046_TIMCallback - в обработчике прерывания системного таймера.
	int v;									//Ожидание успешного обмена данными с XPT2046.
	do {									//Отправка такого управляющего байта/команды для преобразования,
		v = get_adc_in(t, XPT2046_INI, 1);	//после которого отключится АЦП и будет разрешена линия PENIRQ XPT2046.
	} while (v == -100000);					//Это позволит генерировать прерывания микроконтроллеру по спадающему фронту
											//сигнала на входе T_IRQ.
}

/*
 * Процедура опроса тачскрина.
 * Используется при опросе тачскрина, когда получение координат
 * касания тачскрина в прерывании запрещено.
 * Возвращает 3 статуса:
 * 0 - опрос тачскрина прошел успешно;
 * 1 - опрос тачскрина не произведен, т.к. нет касания тачскрина;
 * 2 - опрос тачскрина не произведен, т.к. spi занято.
 * Если опрос тачскрина в прерывании разрешен, то всегда возвращает 0.
 */
uint8_t XPT2046_GetTouch(XPT2046_Handler *t)
{
	if (!t->fl_interrupt) { 		//Если запрещено обновление координат тачскрина в прерывании,
		uint8_t res = get_touch(t); //то запуск процедуры опроса тачскрина.
		if (res == 1) {				//При возникновении ошибки "нет касания тачскрина" (1):
			t->click = 0; 								//- сбросим флаг касания тачскрина;
			t->status = XPT2046_STATUS_NOCLICK;			//- статус "нет клика"
			t->last_click_time = t->click_time_counter;	//- переопределяем длительность "последнего касания";
			t->fl_wait_exti = 1;						//- установим флаг ожидания разрешения прерывания exti;
			t->timer_counter = t->timer_period;			//- перезагружаем счетчик внутреннего таймера обработчика
														//для указания времени ожидания разрешения прерывания exti.
			return 1;				//- возврат ошибки "нет касания тачскрина".
		}							//Если опрос тачскрина завершился с ошибкой "spi занято" (2)
		return res;					//или без ошибок (0), то возврат этих кодов завершения опроса.
	}
	return 0;						//Если опрос тачскрина в прерывании разрешен:
									//без запуска процедуры опроса всегда возврат 0
}

/*
 * Обработчик прерывания EXTI по спадающему фронту сигнала на входе T_IRQ
 * (добавить в соответствующий обработчик EXTIx_IRQHandler - см. файл stm32g0xx_it.c)
 */
void XPT2046_EXTICallback(XPT2046_Handler *t)
{
    if (t) {													  //Если обработчик инициализирован:
   		if (!(t->cnt_data.irq_port->IDR & t->cnt_data.irq_pin)) { //Если на линии T_IRQ низкий уровень:
   			t->fl_timer = 1; 									  //- таймер обработчика разрешен;
   			t->timer_counter = t->timer_period;			  		  //- инициализация счетчика таймера обработчика;
   			t->click_time_counter = 0; 							  //- инициализация счетчика длительности текущего касания тачскрина;
   			NVIC_ClearPendingIRQ(t->cnt_data.exti_irq);			  //- удаление отложенного прерывания для канала exti;
   			NVIC_DisableIRQ(t->cnt_data.exti_irq);				  //- запрещение прерывания для канала exti.
   		}
    }
	__DSB(); //Барьер синхронизации данных
}

/*
 * Обработчик прерывания системного таймера
 * (добавить в обработчик SysTick_Handler - см. файл stm32g0xx_it.c)
 */
void XPT2046_TIMCallback(XPT2046_Handler *t)
{
	if (t) {												//Если инициализирован обработчик:
		if (t->fl_timer) { 									//Если таймер обработчика разрешен,
			t->timer_counter--;								//то уменьшение счетчика таймера обработчика.
			if (t->timer_counter) return;					//Если счетчик таймера не достиг нуля, то выход.
			t->timer_counter = t->timer_period;				//Перезагрузка счетчика таймера обработчика.
			if (t->fl_wait_exti) {							//Если флаг ожидания exti установлен,
				t->fl_wait_exti = 0;						//то сброс этого флага
				t->fl_timer = 0;							//и запрет таймер обработчика.
				NVIC_ClearPendingIRQ(t->cnt_data.exti_irq);	//Удаление отложенного прерывания для канала exti.
				NVIC_EnableIRQ(t->cnt_data.exti_irq); 		//Разрешение прерывания для канала exti.
				__DSB();									//Барьер синхронизации данных.
				return;										//Выход из обработчика.
			}
			if (t->fl_interrupt) {			//Если разрешено обновление координат тачскрина в прерывании,
				uint8_t res = get_touch(t); //то запуск процедуры опроса тачскрина.
				if (res == 1) { 			//При возникновении ошибки при опросе тачскрина:
					t->click = 0; 								//- сбросим флаг касания тачскрина;
					t->last_click_time = t->click_time_counter;	//- переопределяем длительность "последнего касания";
					t->fl_wait_exti = 1;						//- установим флаг ожидания разрешения прерывания exti;
					t->timer_counter = t->timer_period;			//- перезагружаем счетчик внутреннего таймера обработчика
																//для указания времени ожидания разрешения прерывания exti.
					__DSB();  				//Барьер синхронизации данных.
					return;					//Выход из обработчика.
				}
			}
			t->click_time_counter++; 		//Увеличение счетчика длительности текущего касания тачскрина.
		}
	}
	__DSB();								//Барьер синхронизации данных.
}

/*
 * Измерение температуры встроенным в XPT2046 датчиком.
 * Возвращает температуру в градусах Цельсия.
 * При ошибке возвращает -100000 (минус сто тысяч).
 */
int XPT2046_GetTemp(XPT2046_Handler *t)
{
  	//Первое измерение напряжения
	int v1 = get_adc_in(t, XPT2046_TEMP0, XPT2046_OTHER_REPEATED_MEASUREMENTS);
	//Второе измерение напряжения (с повышенным в 91 раз током)
	int v2 = get_adc_in(t, XPT2046_TEMP1, XPT2046_OTHER_REPEATED_MEASUREMENTS);
	if (v1 == -100000 || v2 == -100000) return -100000; //При занятости spi возвращаем ошибку
	//Расчет температуры по формуле (см. спецификацию на XPT2046):
	//                t = 2.573 * (v2 - v1) - 273
	return ((((2573 * XPT2046_VREF) / 1000) * (v2 - v1)) >> 12) - 273;
}

/*
 * Измерение напряжения аккумуляторной батареи на входе VBAT XPT2046 (7 вывод м/с в корпусе TSSOP-16)
 * Возвращает напряжение в милливольтах.
 * При ошибке возвращает -100000 (минус сто тысяч).
 */
int XPT2046_GetVbat(XPT2046_Handler *t)
{
	int v = get_adc_in(t, XPT2046_VBAT, XPT2046_OTHER_REPEATED_MEASUREMENTS);
	if (v == -100000) return -100000;
											//На входе VBAT XPT2046 предусмотрен делитель
	return 4 * ((v * XPT2046_VREF) / 4096); //напряжения, который делит входное напряжения на 4.
											//Именно поэтому для наглядности я здесь записал  сначала деление на 4096,
											//а затем умножение на 4, хотя, можно было просто написать деление на 1024
}

/*
 * Измерение напряжения на входе AUX XPT2046 (8 вывод м/с в корпусе TSSOP-16).
 * Возвращает напряжение в милливольтах.
 * При ошибке возвращает -100000 (минус сто тысяч).
 */
int XPT2046_GetAux(XPT2046_Handler *t)
{
	int v = get_adc_in(t, XPT2046_AUX, XPT2046_OTHER_REPEATED_MEASUREMENTS);
	if (v == -100000) return -100000;
	return (v * XPT2046_VREF) / 4096;
}

/*
 * Измерение напряжения АЦП XPT2046 на заданном входе (определяется командой).
 * Возвращает результат измерения в виде 12 битного цифрового значения, соответствующего измеряемому напряжению.
 * Если необходимо, то, зная опорное напряжение АЦП, измеренный параметр можно привести к напряжению в
 * вольтах/милливольтах. Например, для 12 битного АЦП XPT2046 и значения опорного напряжения VREF: Vv = Vb * VREF / 4096,
 * где: Vb - измеренное 12-битное значение, возвращенное данной процедурой; Vм - рассчитанное напряжение в
 * вольтах/милливольтах; VREF - опорное напряжение АЦП в вольтах/милливольтах.
 * В случае ошибки возвращается -100000 (минус сто тысяч)
 * Параметр command определяет управляющий байт (команду) для выбора входа АЦП и параметров преобразования XPT2046
 * Параметр repeat определяет количество измерений в целях последующего вычисления среднего значения.
 * Первое измерение игнорируется (может содержать излишние шумы)
 */
static int get_adc_in(XPT2046_Handler *t, uint8_t command, uint8_t repeat)
{
	SPI_TypeDef *spi = t->cnt_data.spi;
	if (spi->CR1 & SPI_CR1_SPE ) { return -100000; }
	LCD_CS_GPIO_Port->BSRR = LCD_CS_Pin;
	uint32_t param_spi[2];
	connect_on(&t->cnt_data, param_spi);
	uint32_t irq_exti_enable = NVIC_GetEnableIRQ(t->cnt_data.exti_irq);
	if (irq_exti_enable) {
		NVIC_DisableIRQ(t->cnt_data.exti_irq);
	}
	int avr = 0, fl_no_use = 0, fl_first = 0;
	uint16_t response = 0;
	for (int i = 0; i < repeat; i++) {
		while (!(spi->SR & SPI_SR_TXE)) { ; }
		*((volatile uint8_t*)&spi->DR) = command;
		while (!(spi->SR & SPI_SR_RXNE)) { ; }
		if (!fl_no_use) {
			(void)*((volatile uint8_t*)&spi->DR);
			fl_no_use = 1;
		}
		else {
			response |= *((volatile uint8_t*)&spi->DR);
			if (fl_first) {
				avr += (response >> 3) & 0xfff;
			}
			else {
				fl_first = 1;
			}
		}
		while (!(spi->SR & SPI_SR_TXE)) { ; }
		*((volatile uint8_t*)&spi->DR) = XPT2046_NOP;
		while (!(spi->SR & SPI_SR_RXNE)) { ; }
		response = (*((volatile uint8_t*)&spi->DR)) << 8;
		if (i == repeat - 1) {
			while (!(spi->SR & SPI_SR_TXE)) { ; }
			*((volatile uint8_t*)&spi->DR) = XPT2046_NOP;
			while (!(spi->SR & SPI_SR_RXNE)) { ; }
			response |= *((volatile uint8_t*)&spi->DR);
			if (fl_first) {
				avr += (response >> 3) & 0xfff;
			}
		}
	}
	while (!(spi->SR & SPI_SR_TXE)) { ; }
	*((volatile uint8_t*)&spi->DR) = XPT2046_INI;
	while (!(spi->SR & SPI_SR_RXNE)) { ; }
	(void)*((volatile uint8_t*)&spi->DR);
	while (!(spi->SR & SPI_SR_TXE)) { ; }
	*((volatile uint8_t*)&spi->DR) = XPT2046_NOP;
	while (!(spi->SR & SPI_SR_RXNE)) { ; }
	(void)*((volatile uint8_t*)&spi->DR);
	while (!(spi->SR & SPI_SR_TXE)) { ; }
	*((volatile uint8_t*)&spi->DR) = XPT2046_NOP;
	while (!(spi->SR & SPI_SR_RXNE)) { ; }
	(void)*((volatile uint8_t*)&spi->DR);
	if (repeat >= 2) {
		avr /= (repeat - 1);
	}
	connect_off(&t->cnt_data, param_spi);
	if (irq_exti_enable) {
		NVIC_EnableIRQ(t->cnt_data.exti_irq);
	}
	return avr;
}

/* Подключение к контроллеру XPT2046 с сохранением параметров spi */
static void connect_on(XPT2046_ConnectionData *cnt_data, uint32_t *param)
{
	SPI_TypeDef *spi = cnt_data->spi;
	param[0] = spi->CR1; 			//Запоминаем параметры spi
	param[1] = spi->CR2;
	//--------------------------------- Параметры spi для XPT2046 ------------------------------------
	spi->CR1 &= ~ (SPI_CR1_BIDIMODE |  	//Режим работы с однонаправленной передачей по линиям данных
				   SPI_CR1_RXONLY |   	//full duplex — передача и прием
				   SPI_CR1_CRCEN | 		//Аппаратный расчет CRC выключен
				   SPI_CR1_BR_Msk |		//Маска скорости подключения
				   SPI_CR1_LSBFIRST |	//MSB - старший бит первый
				   SPI_CR1_CPOL | 		//полярность синхронизации (low)
				   SPI_CR1_CPHA);		//фаза синхронизации (1edge)
	spi->CR1 |= SPI_CR1_MSTR;			//МК ведущее устройство, XPT2046 ведомое
	spi->CR1 |= (uint32_t)((cnt_data->speed & 7) << SPI_CR1_BR_Pos); //Установим скорость spi для XPT2046
	spi->CR2 &= ~SPI_CR2_DS_Msk; 	  	//Маска ширины кадра spi
	spi->CR2 |= 7UL << SPI_CR2_DS_Pos; 	//ширины кадра spi 8 бит
	//-------------------------------------------------------------------------------------------------
	spi->CR1 |= SPI_CR1_SPE; //spi включаем
	cnt_data->cs_port->BSRR = (uint32_t)cnt_data->cs_pin << 16U; //Подключаем XPT2046 к spi (низкий уровень на T_CS)
}

/*
 * Отключение от контроллера XPT2046 с восстановлением параметров spi
 * в состояние "до подключения"
 */
static void connect_off(XPT2046_ConnectionData *cnt_data, uint32_t *param)
{
	SPI_TypeDef *spi = cnt_data->spi;
	while (spi->SR & SPI_SR_BSY) { __NOP(); }   //Ждем, когда spi освободится
	cnt_data->cs_port->BSRR = cnt_data->cs_pin; //Отключаем XPT2046 от spi (высокий уровень на T_CS)
	spi->CR1 &= ~SPI_CR1_SPE;					//spi выключаем и
	spi->CR1 = param[0]; 						//восстанавливаем прежние параметры
	spi->CR2 = param[1];
}

/*
 * Считывание напряжений, соответствующих точке касания тачскрина
 */
static uint8_t get_touch(XPT2046_Handler *t)
{
	//Проверка наличия касания тачскрина до проведения измерений
	//(измерения некорректны, если нет касания тачскрина)
	if (t->cnt_data.irq_port->IDR & t->cnt_data.irq_pin) { //Если высокий уровень на линии T_IRQ,
		return 1; 										   //то нет касания тачскрина
	}
	//Получаем напряжения (в цифровом виде), соотвестсвующие
	//точке касания тачскрина.
	int x = get_adc_in(t, XPT2046_X, XPT2046_TOUCH_REPEATED_MEASUREMENTS);
	int y = get_adc_in(t, XPT2046_Y, XPT2046_TOUCH_REPEATED_MEASUREMENTS);
	if (x == -100000 || y == -100000) {					   //spi занято
		return 2;
	}
	//Проверка наличия касания тачскрина после проведения измерений
	//(измерения некорректны, если воздействие на тачскрин снялось в ходе проведения измерений)
	if (t->cnt_data.irq_port->IDR & t->cnt_data.irq_pin) { //Если высокий уровень на линии T_IRQ,
			return 1; 									   //то нет касания тачскрина.
	}
	t->click = 1;	//Установка флага касания тачскрина.
	//Определяем статус тачскрина
	if (t->status == XPT2046_STATUS_NOCLICK) { 					//Статус "клик"
		t->status = XPT2046_STATUS_CLICK;
	}
	else if (t->status == XPT2046_STATUS_CLICK &&				//Статус "удержание"
			//Под удержанием понимается касание тачскрина, длительность которого составляет
			//не менее XPT2046_TOUCH_HOLD_TIME миллисекунд
			t->click_time_counter * t->timer_period >= XPT2046_TOUCH_HOLD_TIME) {
		t->status = XPT2046_STATUS_HOLD;
	}
	t->point.x = x; //Обновляем координаты тачскрина
	t->point.y = y;
	return 0;
}
