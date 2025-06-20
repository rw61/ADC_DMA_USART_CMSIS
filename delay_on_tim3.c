#include "main.h"


#define TIM_Enable_IT_UPDATE(TIMx) TIMx->DIER|=TIM_DIER_UIE // макрос на разрешение прерывания: используя его внутри программы, подставляя вместо x в (TIMx) номер таймера, мы можем разрешать прерывания в любом таймере
#define TIM_Enable_Countetimr(TIMx) TIMx->CR1|=TIM_CR1_CEN // макрос на включение таймера, заменяя x на номер таймера - включаем нужный таймер
#define TIM_Disable_Countetimr(TIMx) TIMx->CR1 &= ~TIM_CR1_CEN

static void TIM3_Init(void);
volatile static int Tim3_count = 0;
//static void delay_uS(int uS);
static void delay_mS(int mS);



void TIM3_IRQHandler(void){
	    TIM3->SR &= ~TIM_SR_UIF;
	    
	
	       Tim3_count++;            //ТАЙМЕР ДОСЧИТАЛ - ИНКРЕМЕНТ ПЕРЕМЕННОЙ
			
}










static void TIM3_Init(void)
 {
	  
	  WRITE_REG(TIM3->PSC, 99);				//устанавливаем делитель на 1599, фоормула расчета частоты таймера х = F(частота проца, в нашем случае это 16Мгц)/значение . записанное в регистр предделителя TIMx_PSC(в нашем случае это 1599)+1 = 16000000/1600 = 10000 тактов в секунду
																			// WRITE_REG - это макрос для записи числа в регистр, он прописан в файле для ядра stm32f4xx.h(если это серия f4) б или аналогичном для других ерий мк
	  WRITE_REG(TIM3->ARR, 999);       //чтобы получить время счета таймера в 1 секундуб записываем в регистр AUTO RELOAD REG число 10000. 1 секунда получается так: частота счета таймера равна 10000 тактов в секунду
																			//потому как частота МК равна 16МГцб, а Prescaler равен 1599+1, и в итоге частота равна 10000. Когда мы записываем в ARR 10000, то таймер считает до 10000 с частотой 10000 Гц. время счета будет 1 секунда
	  //TIM3->PSC = 1599;							//предыдущие две строки можно записать и так
	  // TIM3->ARR = 10000;
	  
	 TIM3->CR1 |= TIM_CR1_URS;       //прерыание только по переполнению/или когда счетчик = 0, смотря. куда считаем. В IRQ_HANDLER МОЖНО НЕ ПРОВЕРЯТЬ СТАТУС UIF
	 TIM_Enable_IT_UPDATE(TIM3);     // разрешение прерывания при событии обновления
	 TIM3->EGR |= TIM_EGR_UG;        //реинициализация таймера и обновление регистров
	 
	 NVIC_EnableIRQ (TIM3_IRQn);        //разрешение прерывания для таймера 3 , IRQ - Interrupt request
	 
 }
 
 static void delay_mS(int mS){
	 
	 TIM_Enable_Countetimr(TIM3);
	 Tim3_count = 0;
	 while(Tim3_count<mS){}
	 TIM_Disable_Countetimr(TIM3);

	 
 }

 
 
 // static void delay_mS(int mS){           функция задержки в мс, если есть фуекция для мкс
//	 
//	 TIM_Enable_Countetimr(TIM3);
//	 Tim3_count = 0;
//	 while(Tim3_count<(mS*1000)){}
//	 TIM_Disable_Countetimr(TIM3);
// }
 
 