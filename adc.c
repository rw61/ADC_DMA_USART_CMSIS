#include "main.h"

 
/* TIMER 2 TRIGGERS THE START OF ADC CONVERTION */
void TIM2_Init(void)
 {
	 WRITE_REG(TIM2->PSC, 49);				//ПРИ ЧАСТОТЕ НА APB1 - 50 MHz, И ДЕЛИТЕЛЕ ЧАСТОТЫ ТАЙМЕРА 2 В 49, ЧАСТОТА НА ТАЙМЕРЕ 2 БУДЕТ РАВНА 1 MHz
	 WRITE_REG(TIM2->ARR, 99999);     //ТАЙМЕР СЧИТАЕТ ДО 100К И ПРОИСХОДИТ ТРИГГЕР ДЛЯ АЦП - ПРОИСХОДИТ НОВОЕ ПРЕОБРАЗОВАНИЕ, ТО ЕСТЬ ЗА 1 СЕКУНДУ ТРИГГЕР СРАБАТЫВАЕТ 10 РАЗ = 10 ПРЕОБРАЗОВАНИЙ.  В ДАННОЙ ПРОГЕ КАЖДЫЙ КАНАЛ АЦП ИЗ 3х ОПРАШИВАЕТСЯ 100 РАЗ, А ЗАТЕМ ДЕЛАЕТСЯ УСРЕДНЁННОЕ ЗНАЧЕНИЕ - ИТОГО 300 ПРЕОБРАЗОВАНИЙ ЗА 30 СЕКУНД.												
	 TIM2->DIER |= TIM_DIER_UIE;
	 
	 /* Reset the MMS Bits */
   TIM2->CR2 &= (uint16_t)~TIM_CR2_MMS;
   /* Select the TRGO source */
   TIM2->CR2 |=  TIM_CR2_MMS_1; //UPDATE EVENT
	 
	 TIM2->CR1 |= TIM_CR1_URS;         //Only counter overflow/underflow generates an update interrupt or DMA request if enabled.
	 TIM2->EGR |= TIM_EGR_UG;          //ВАЖНО! РЕИНИЦИАЛИЗАЦИЯ ТАЙМЕРА И ОБНОВЛЕНИЕ РЕГИСТРОВ. ПОСЛЕ АКТИВАЦИИ ТАМЕРА СЧЁТЧИК НАЧИНАЕТСЧИТАТЬ С НУЛЯ! С ЭТИМ БИТОМ ВСЁ РАБОТАЕТ КОРРЕКТНО ТОЛЬКО В ПАРЕ С БИТОМ CR1_URS
   TIM2->CR1 |= TIM_CR1_CEN;
//	 NVIC_EnableIRQ (TIM2_IRQn);        //разрешение прерывания для таймера 2 , IRQ - Interrupt request	 
 }
 
/* ADC INITIALIZATION FUNCTION */
void ADC_Init(void){ 
	 
	 ADC->CCR |= ADC_CCR_ADCPRE_0;  //ДЕЛИТЕЛЬ ЧАСТОТЫ ДЛЯ АЦП = 4, ПРИ f(APB2) = 50 MHz, f(ADC) = 50/4 = 12,5 MHz//
	 ADC1->CR1 = 0;
   ADC1->CR1 |= ADC_CR1_SCAN;
	 
   ADC1->CR2 |=  ADC_CR2_ADON;
	 ADC1->CR2 &= ~ADC_CR2_ALIGN;
	 ADC1->CR2 |= ADC_CR2_DMA;
	 ADC1->CR2 |= ADC_CR2_DDS;          // после каждого преоьразования АЦП -> запрос к DMA на передачу результата, только для single conversion
   ADC1->CR2 &= ~ADC_CR2_EXTSEL;                				//очистка бита EXTSEL
	 ADC1->CR2 |= ADC_CR2_EXTSEL_1 | ADC_CR2_EXTSEL_2;//| ADC_CR2_CONT;   //установка 0110 - TIM2 TRGO(Timer Trigger Output event) - одно из событий, при котором возникает прерывание по таймеру и запускается ADC - это update event
	 ADC1->CR2 |= ADC_CR2_EXTEN_0;
	 
		 
                  /* НАСТРОЙКА СЭМПЛИРОВАНИЯ */
	 ADC1->SMPR2 &= ~ADC_SMPR2_SMP0;
	 ADC1->SMPR2 |= ADC_SMPR2_SMP0_0 | ADC_SMPR2_SMP0_2;                //112 CYCLES
	 ADC1->SMPR2 |= ADC_SMPR2_SMP7_0 | ADC_SMPR2_SMP7_2;
	 ADC1->SMPR1 |= ADC_SMPR1_SMP16_0 | ADC_SMPR1_SMP16_2;
	
									/* НАСТРОЙКА SEQUENCE */
	 ADC1->SQR1 |= ADC_SQR1_L_1;              //первый бит в 1 = 0010 - 3 конверсии
	 ADC1->SQR3 = 0;
	 ADC1->SQR3 |= ADC_SQR3_SQ1_0 | ADC_SQR3_SQ1_1 | ADC_SQR3_SQ1_2; //111 - ЭТО 7, ЗНАЧИТ СЕДЬМОЙ КАНАЛ БУДЕТ ОБРАБАТЫВАТЬСЯ ПЕРВЫМ
	 MODIFY_REG(ADC1->SQR3, ADC_SQR3_SQ2_Msk, 0 << ADC_SQR3_SQ2_Pos); //SQ2-ВТОРАЯ ОЧЕРЁДНОСТЬ. НУЛЕВОЙ КАНАЛ - ВТОРОЙ НА ОБРАБОТКУ
   ADC1->SQR3 |= ADC_SQR3_SQ3_4;                                    //SQ3 - ТРЕТЬЯ ОЧЕРЁДНОСТЬ, 4-ый бит в 1 -> 10000 - это 16 канал - датчик температуры 
									/* TEMPERATURE SENSOR */
   ADC->CCR |= ADC_CCR_TSVREFE;   //ВКЛ.ТЕМП ДАТЧИКА
	 

	 
 	 //NVIC_EnableIRQ(ADC_IRQn);
	
 }

