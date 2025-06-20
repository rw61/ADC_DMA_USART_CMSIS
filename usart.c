#include "main.h"

#define SR_TC (1U<<6)
uint8_t g_uart_cmplt;


void USART2_IRQHandler(void)
{
		g_uart_cmplt = 1;
		/*Clear TC interrupt flag*/
		USART2->SR &=~SR_TC;
}

static void USART2_Init(void){
	USART2->BRR = 0x1B2; // USARTDIV = 0x1B2 ,dlya 115200 baud, pri f(APB1)=50MHz
	USART2->CR1 = USART_CR1_RE | USART_CR1_TE | USART_CR1_UE | USART_CR1_RXNEIE | USART_CR1_IDLEIE;
	MODIFY_REG(GPIOA->MODER, GPIO_MODER_MODER2_0 | GPIO_MODER_MODER3_0, GPIO_MODER_MODER2_1 | GPIO_MODER_MODER3_1); //ALTERNATIVE FUNC PIN2/3 PORTA
	USART2->CR3 |= USART_CR3_DMAR | USART_CR3_DMAT;
	GPIOA->AFR[0]	|= (7 << GPIO_AFRL_AFSEL2_Pos) ; // That alt fn is alt 7 for PA2
	GPIOA->AFR[0] 	|= (7 << GPIO_AFRL_AFSEL3_Pos) ; // Alt fn for PA3 is same as for PA2
																																		 //выставляем режим работы AF на PIN6 PORTB, смотрим в таблицу 
																																		 //Table 9. Alternate function mapping, находим нужный пин, нужную функцию(TIM4 CH1),
																																		 //и видим, что данная функция обозначена как AF02. AFR[0] - Значит, что работаем с низшими битами от 0 до 7
	NVIC_EnableIRQ(USART2_IRQn);
}


static void USART2_Send(char chr)   
{
	while(!(USART2->SR & USART_SR_TC));
	USART2->DR = chr;
}

static void USART2_Send_String(char* str)
{
	uint8_t i = 0;
	while(str[i]){
		USART2_Send(str[i++]);
	}
}

static void CMSIS_USART_Transmit(USART_TypeDef *USART, uint32_t *data, uint16_t Size) {
	for (uint16_t i = 0; i < Size; i++) {
		while (READ_BIT(USART2->SR, USART_SR_TXE) == 0) ; //Ждем, пока линия не освободится
		USART->DR = *data++; //Кидаем данные  
	}	
}