
/*  КРАТКО СУТЬ: АЦП РАБОТАЕТ С ТРЕМЯ КАНАЛАМИ(2 - НАПРЯЖЕНИЕ И 1 -ТЕМПЕРАТУРА). ДЕЛАЕТСЯ ПО 100 ЗАМЕРОВ КАЖДОГО ЗНАЧЕНИЯ.
	  РЕЗУЛЬТАТ ПЕРЕДАЁТСЯ ЧЕРЕЗ DMA2 В БУФЕР, ЗНАЧЕНИЯ С КАЖДОГО КАНАЛА АЦП СУММИРУЮТСЯ И ПОСЛЕ 100 ИЗМЕРЕНИЙ ВЫЧИСЛЯЮТСЯ
    СРЕДНИЕ ЗНАЧЕНИЯ. В КОЛБЭКЕ ВЫЧИСЛЯЮТСЯ ФИЗИЧЕСКИЕ ВЕЛИЧИНЫ, ИСХОЛЯ ИЗ ПОЛУЧЕННЫХ СРЕДНИХ ЗНАЧЕНИЙ.
    СРЕДНИЕ ЗНАЧЕНИЯ , ПЕРЕЗАПИСАННЫЕ В ОТДЕЛЬНЫЙ БУФЕР ADC_to_USART, ПЕРЕДАЮТСЯ ЧЕРЕЗ DMA1 В USART2_DR ,
		ЗАТЕМ ОТПРАВЛЯЮТСЯ В ТЕРМИНАЛ.
		В ОСНОВНОМ ЦИКЛЕ ФУНКЦИЯ sprintf КОПИРУЕТ СТРОКУ, А ВМЕСТЕ С НЕЙ И СРЕДНИЕ ЗНАЧЕНИЯ С АЦП, И ПОМЕЩАЮТ ЕЁ В БУФЕР
		msg_buff. ПОСЛЕ ЭТОГО msg_buff ИСПОЛЬЗУЕТСЯ ФУНКЦИЕЙ DMA1_TX_func КАК ИСТОЧНИК ДАННЫХ(ПЕРЕДАЧА MEMORY TO PERIPHERIAL),
		ПЕРЕДАЧИ ЧЕРЕЗ DMA1, МЕСТО НАЗНАЧЕНИЯ ДАННЫХ - USART2_DR.
		ФУНКЦИЯ ПРИЁМА ДАННЫХ DMA1_RX_func() РАБОТАЕТ ПО СХЕМЕ PERIPHERIAL TO MEMORY : ПЕРЕДАЕМ СТРОКУ В ТЕРМИНАЛЕ, НО НЕ БОЛЕЕ,
    ЧЕМ УКАЗАНО В UART_DATA_BUFF_SIZE, И ОНА СОХРАНЯЕТСЯ В 	uart_data_buffer. ДАЛЕЕ КОПИРУЕМ ЭТУ СТРОКУ В msg_buff, И ЭТУ
    СТРОКУ ДАЛЕЕ ИСПОЛЬЗУЕТ ФУНКЦИЯ DMA1_TX_func.		
		
		
*/
#include "main.h"

extern uint8_t g_rx_cmplt;
extern uint8_t g_tx_cmplt;
extern char uart_data_buffer[UART_DATA_BUFF_SIZE];
//char msg_buff[150] ={'\0'}; 
uint8_t g_uart_cmplt;
char msg_buff[150] ={'\0'}; 


 int main(void){
	 
	 RCC_Init();
	 DMA2_Init();
	 DMA1_Init();
	 DMA1_RX_func();
	 TIM3_Init();
	 TIM2_Init();
	 GPIO_Init();
	 ADC_Init();
	 USART2_Init();
	 

//	 sprintf(msg_buff,"Initialization...cmplt\n\r"); //копируем в буфер
//	 DMA1_TX_func((uint32_t)msg_buff,strlen(msg_buff));//передаем
//	 while(!g_tx_cmplt){}
		 

	 while(1){
		
		 sprintf(msg_buff,"Value 1 is %d \t U1 =  %d mV\n\rValue 2 is %d \t U2 =  %d mV\n\rValue 3 is %d \t U_temp =  %d mV\n\r\r",(uint32_t)ADC_to_USART[0], ((uint32_t)ADC_to_USART[0])*3300/4096, (uint32_t)ADC_to_USART[1], ((uint32_t)ADC_to_USART[1])*3300/4096, (uint32_t)ADC_to_USART[2], ((uint32_t)ADC_to_USART[2])*3300/4096  ); //копируем в буфер
	   DMA1_TX_func((uint32_t)msg_buff,strlen(msg_buff));
		 while(!g_tx_cmplt){}
			 			g_tx_cmplt = 0;
			      g_uart_cmplt = 0;
		   delay_mS(1500);
		 
//			sprintf(msg_buff, "Message received : %s \r\n",uart_data_buffer); //КОПИРУЕМ ТО , ЧТО ОТПРАВИЛИ ПО ТЕРМИНАЛУ И СОХРАНЯЕМ В msg_buff
//			g_rx_cmplt = 0;
//			g_tx_cmplt = 0;
//			g_uart_cmplt = 0;
//			dma1_stream6_uart_tx_config((uint32_t)msg_buff,strlen(msg_buff));	
//			while(!g_tx_cmplt){}
		 
//		 GPIOA->BSRR |= GPIO_BSRR_BS5;
//		 delay_mS(1000);
//		 GPIOA->BSRR |= GPIO_BSRR_BR5;
//		 delay_mS(1000);
	
 }
 
}
 
 void GPIO_Init(void){
		GPIOA->MODER |= GPIO_MODER_MODE5_0;                              //LED
		GPIOA->MODER &= ~GPIO_MODER_MODE5_1;
	 
	  GPIOA->MODER |= GPIO_MODER_MODER0_0 | GPIO_MODER_MODER0_1;       //pin0 и pin7 на ADC
		GPIOA->MODER |= GPIO_MODER_MODER7_0 | GPIO_MODER_MODER7_1;       
		
	  MODIFY_REG(GPIOB->MODER, GPIO_MODER_MODE6, GPIO_MODER_MODE6_1);  //PORTB PIN6 - ЭТО CH1 ДЛЯ ТАЙМЕРА 4, НАСТРАИВАЕМ НА AF
		GPIOB->AFR[0]	|= (2 << GPIO_AFRL_AFSEL6_Pos) ;                   //выставляем режим работы AF на PIN6 PORTB - это 10 бита MODER6, смотрим в таблицу 
																																		 //Table 9. Alternate function mapping, находим нужный пин, нужную функцию(TIM4 CH1),
																																		 //и видим, что данная функция обозначена как AF02. AFR[0] - Значит, что работаем с низшими битами от 0 до 7
		MODIFY_REG(GPIOB->MODER, GPIO_MODER_MODE8, GPIO_MODER_MODE8_1);  //для 8 пина порта B тоже вкл.режим альтерн. функции
		GPIOB->AFR[1]	|= (2 << GPIO_AFRH_AFSEL8_Pos);                    //для пина pb8 альтернативная функция TIM4 CH3, так же 
																																		 //будет обозначена как AF02
 }
