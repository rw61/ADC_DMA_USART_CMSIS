#include "main.h"

#define DMA1EN (1U<<21)
#define DMA_SCR_EN (1U<<0)
#define DMA_SCR_MINC (1U<<10)
#define DMA_SCR_PINC (1U<<9)
#define DMA_SCR_CIRC (1U<<8)
#define DMA_SCR_TCIE (1U<<4)
#define DMA_SCR_TEIE (1U<<2)
#define DMA_SFCR_DMDIS (1U<<2)
#define HIFCR_CDMEIF5 (1U<<8)
#define HIFCR_CTEIF5 (1U<<9)
#define HIFCR_CTCIF5 (1U<<11)
#define HIFCR_CDMEIF6 (1U<<18)
#define HIFCR_CTEIF6 (1U<<19)
#define HIFCR_CTCIF6 (1U<<21)
#define HIFSR_TCIF5 (1U<<11)
#define HIFSR_TCIF6 (1U<<21)
#define UART_DATA_BUFF_SIZE 10

char uart_data_buffer[UART_DATA_BUFF_SIZE];
uint8_t g_rx_cmplt;
uint8_t g_tx_cmplt;



void DMA1_Stream6_IRQHandler(void)
{
		if((DMA1->HISR) & HIFSR_TCIF6)     //transpher complete interrupt flag
		{
			
		//do_ssomething
		g_tx_cmplt = 1;
		/*Clear the flag*/
		DMA1->HIFCR |= HIFCR_CTCIF6;  //Stream x clear transfer complete interrupt flag///Writing 1 to this bit clears the corresponding TCIFx flag in the DMA_HISR register
		}
}

void DMA1_Init(void){
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;
}


void DMA1_Stream5_IRQHandler(void)
{
		if((DMA1->HISR) & HIFSR_TCIF5)     //transpher complete interrupt flag
		{
		g_rx_cmplt = 1;
		/*Clear the flag*/
		DMA1->HIFCR |= HIFCR_CTCIF5;      //Stream x clear transfer complete interrupt flag///Writing 1 to this bit clears the corresponding TCIFx flag in the DMA_HISR register
		}
}

void DMA1_TX_func(uint32_t msg_to_snd, uint32_t msg_len){
	/*Disable DMA stream*/
	DMA1_Stream6->CR &=~DMA_SCR_EN;
	/*Wait till DMA Stream is disabled*/
	while((DMA1_Stream6->CR & DMA_SCR_EN)){}
	/*Clear interrupt flags for stream 6*/
	DMA1->HIFCR = HIFCR_CDMEIF6 |HIFCR_CTEIF6|HIFCR_CTCIF6;
	/*Set periph address*/
	DMA1_Stream6->PAR = (uint32_t)(&(USART2->DR));
//		/*Set mem address*/
  	DMA1_Stream6->M0AR = msg_to_snd;
		//circular mode
//		DMA2_Stream6->CR |= DMA_SxCR_CIRC;																
	/*Set number of transfer*/
	DMA1_Stream6->NDTR = msg_len;
	/*Select Channel 4*/
	DMA1_Stream6->CR &= ~(1u<<25);    //100 - stream 6 channel 4 = USART2_TX
	DMA1_Stream6->CR &= ~(1u<<26);
	DMA1_Stream6->CR |= (1u<<27);
	/*Enable memory addr increment*/
	DMA1_Stream6->CR |=DMA_SCR_MINC;
	/*Set transfer direction :Mem to Periph*/
	DMA1_Stream6->CR |=(1U<<6);
	DMA1_Stream6->CR &=~(1U<<7);
	/*Set transfer complete interrupt*/
	DMA1_Stream6->CR |= DMA_SCR_TCIE;
	/*Enable DMA stream*/
	DMA1_Stream6->CR |= DMA_SCR_EN;
	  
	NVIC_EnableIRQ(DMA1_Stream6_IRQn);
}


 
 void DMA1_RX_func(void)
{
	/*Disable DMA stream*/
	DMA1_Stream5->CR &=~ DMA_SCR_EN;
	/*Wait till DMA Stream is disabled*/
	while((DMA1_Stream5->CR & DMA_SCR_EN)){}
	/*Clear interrupt flags for stream 5*/
	DMA1->HIFCR = HIFCR_CDMEIF5 |HIFCR_CTEIF5|HIFCR_CTCIF5;
	/*Set periph address*/
	DMA1_Stream5->PAR = (uint32_t)(&(USART2->DR));
	/*Set mem address*/
	DMA1_Stream5->M0AR = (uint32_t)(&uart_data_buffer);
	/*Set number of transfer*/
	DMA1_Stream5->NDTR = (uint16_t)UART_DATA_BUFF_SIZE;
		/*Select Channel 4*/
	DMA1_Stream5->CR &= ~(1u<<25);     //100 - 4ый канал stream5 DMA1 = USART2_RX
	DMA1_Stream5->CR &= ~(1u<<26);
	DMA1_Stream5->CR |= (1u<<27);
	/*Enable memory addr increment*/
	DMA1_Stream5->CR |= DMA_SCR_MINC;
	/*Enable transfer complete interrupt*/
	DMA1_Stream5->CR |= DMA_SCR_TCIE;
	/*Enable Circular mode*/
	DMA1_Stream5->CR |= DMA_SCR_CIRC;
	/*Set transfer direction : Periph to Mem*/
	DMA1_Stream5->CR &=~(1U<<6);
	DMA1_Stream5->CR &=~(1U<<7);
	/*Enable DMA stream*/
	DMA1_Stream5->CR |= DMA_SCR_EN;
	/*Enable DMA Stream5 Interrupt in NVIC*/
	NVIC_EnableIRQ(DMA1_Stream5_IRQn);
	
}


 
