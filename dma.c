#include "main.h"

   volatile uint16_t a1 = 0;
	 volatile uint16_t a2 = 0;
   volatile uint16_t t = 0;
	 volatile uint32_t Counter_DMA = 3;
	 volatile float Temp = 0;
	 volatile float U_sens = 0;
	 volatile float U = 0;
	 volatile float U2 = 0;
	 
	 volatile uint16_t ADC_Data[20];
	 volatile uint32_t adc_count = 0;
	 volatile uint32_t adc2_count = 0;
	 volatile uint32_t adc_temp_count = 0;
	 volatile uint16_t adc_count_total = 0;
	 volatile uint16_t adc2_count_total = 0;
	 volatile uint16_t adc_temp_count_total = 0;
	 volatile uint16_t callback_count = 0;
   volatile uint16_t ADC_to_USART[3] = {};

 
   void ADC_CallbackFunc();
   void DMA2_Init(void);
  
 
/* DMA2 IRQ HANDLER */ 
void	DMA2_Stream0_IRQHandler(void)
 {
/*********** ПРЕРЫВАНИЕ ПО ПЕРЕДАЧЕ ПОЛОВИНЫ ЭЛЕМЕНТОВ (КОЛИЧЕСТВО ,УКАЗАННОЕ В NDTR/2). ПОСЛЕ ЭТОГО ПРЕРЫВАНИЯ ,МОЖНО ОБРАБОТАТЬ ДАННЫЕ, 
//   КОТОРЫЕ УЖЕ ПРИНЯТЫ, В ЭТО ВРЕМЯ DMA БУДЕТ ПЕРЕДАВАТЬ ВТОРУЮ ПОЛОВИНУ ЭЛЕМЕНТОВ ДАННЫХ
//	 if (READ_BIT(DMA2->LISR, DMA_LISR_HTIF0)) {
//			SET_BIT(DMA2->LIFCR, DMA_LIFCR_CHTIF0); //Сбросим глобальный флаг.
//        ЗДЕСЬ МОЖНО ОБРАБОТАТЬ  ПРИНЯТЫЕ ДАННЫЕ
		}
*/
	 
/************   ПРЕРЫВАНИЕ ПО ПЕРЕДАЧЕ ВСЕХ ЭЛЕМЕНТОВ (КОЛИЧЕСТВО ,УКАЗАННОЕ В NDTR).	************/
	 if (READ_BIT(DMA2->LISR, DMA_LISR_TCIF0)) {

		SET_BIT(DMA2->LIFCR, DMA_LIFCR_CTCIF0); //Сбросим глобальный флаг.
      //CLEAR_BIT(DMA2->LIFCR, DMA_LIFCR_CTCIF0);
		  //eсли включены прерывания по полуслову, то нужно сбросить флаг HT*
		 
		if(Counter_DMA <303){	
				if(Counter_DMA%3 == 0)
				{
					adc_count = adc_count+ADC_Data[0];
					++a1;
				}
				if(Counter_DMA%3 == 1)
				{
					adc2_count = adc2_count+ADC_Data[1];
					++a2;
				}
				if(Counter_DMA%3 == 2)
				{
					adc_temp_count = adc_temp_count+ADC_Data[2];
					++t;
				}
				Counter_DMA++;
			}
		else
		{
			
				adc_count_total = adc_count/a1;
				adc2_count_total = adc2_count/a2;
				adc_temp_count_total = adc_temp_count/t;
			
			  ADC_to_USART[0] = adc_count_total;
        ADC_to_USART[1] = adc2_count_total;
			  ADC_to_USART[2] = adc_temp_count_total;
			
				ADC_CallbackFunc();
				Counter_DMA = 3;
				adc_count_total = 0;
				adc2_count_total = 0;
			  adc_temp_count_total = 0;
				adc_count = 0;
				adc2_count = 0;
				adc_temp_count = 0;
			  a1 = 0;
			  a2 = 0;
			  t = 0;
			
			}
	 }
	
	 else if (READ_BIT(DMA2->LISR, DMA_LISR_TEIF0)) {
		/*Здесь можно сделать какой-то обработчик ошибок*/
		SET_BIT(DMA2->LIFCR, DMA_LIFCR_CTCIF0); //Сбросим глобальный флаг.
	}
}
 
/* CALLBACK FUNCTION */
 void ADC_CallbackFunc(){
//	 adc = ADC1->DR;
//	adc = ADC_Data[0];               //первым в ADC_DR попадает результат преобразования на 7 канале
//	 U = (adc/4096)*3.3;
//	adc2 = ADC_Data[1];              // результат на 0 канале 
//	 U2 = (adc2/4096)*3.3;
//	adc_temp = ADC_Data[2];
//	U_sens = (adc_temp*3.3)/4095;
//	Temp = ((U_sens - 0.76) / 0.0025) + 25;// ФОРМУЛА ПРЕОБРАЗОВАНИЯ НАПРЯЖЕНИЯ , СНЯТОГО С ДАТЧИКА ТЕМПЕРАТУРЫ В ТЕМПЕРАТУРУ
//  DMA2_Stream0->CR &= ~DMA_SxCR_EN;

	U = (float)(adc_count_total)/4096*3.3;
	U2 = (float)(adc2_count_total)/4096*3.3;
	U_sens = (float)(adc_temp_count_total)/4096*3.3;
  Temp = ((U_sens - 0.76) / 0.0025) + 25;
	callback_count++;
 }
 
 /* DMA2 INITIALIZATION FUNCTION */
  void DMA2_Init(void){
	 DMA2_Stream0->PAR = (uint32_t)&(ADC1->DR);     //задаём адрес переферийного устройства, из которого будем забирать данные
	 DMA2_Stream0->M0AR = (uint32_t)ADC_Data;       //задаём адрес в памяти, куда будем помещать данные
	 DMA2_Stream0->NDTR = 3;                        //Настроим количество данных для передачи. После каждого периферийного события это значение будет уменьшаться.
	 
	 
	 DMA2_Stream0->CR = 0;
	 DMA2_Stream0->CR |= DMA_SxCR_TCIE /*| DMA_SxCR_HTIE*/ | DMA_SxCR_TEIE; //прерывания по окончании передачи, ошибке	 DMA2_Stream0->CR |= DMA_SxCR_CIRC;      																	 //циклический режим
	 //DMA2_Stream0->CR |= DMA_SxCR_PL_1;                                       //приоритет в high
	 DMA2_Stream0->CR |= DMA_SxCR_PSIZE_0;																		//размер данных из переферии - half-word(16bits)
	 DMA2_Stream0->CR |= DMA_SxCR_MSIZE_0;																		//аналогично, только для размера памяти, куда будем кидать данные
	 DMA2_Stream0->CR |= DMA_SxCR_CIRC;																//circular mode
	 DMA2_Stream0->CR |= DMA_SxCR_MINC;                               //инкремент ячеек памяти, 
	 DMA2_Stream0->CR |= DMA_SxCR_EN;                                 //запуск
	 NVIC_EnableIRQ(DMA2_Stream0_IRQn);
	 
 }
	
 
 