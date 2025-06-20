#include "main.h"
int RCC_Init(void);
static __IO int StartUpCounter;

/*    НАСТРАИВАЕМ ЧАСТОТУ НА 50 MHz: (8(HSE) / 8(PLLM) * 200(PLLN) / 4(PLLP)) = 50 MHz */


int RCC_Init(void){
		
	   RCC->CR |= RCC_CR_HSION;
	   while(READ_BIT(RCC->CR, RCC_CR_HSIRDY) == 0){}
	   CLEAR_BIT(RCC->CR, RCC_CR_HSEBYP);
	   SET_BIT(RCC->CR, RCC_CR_HSEON);
		 //ожидание флага готовности НЕ по-простому, на случай выхода из строя КВАРЦА HSE
		 //Ждем успешного запуска или окончания тайм-аута
				for(StartUpCounter=0; ; StartUpCounter++)
				{
			     //Если успешно запустилось, то выходим из цикла
					if(RCC->CR & (1<<RCC_CR_HSERDY_Pos))
						break;
		        //Если не запустилось, то отключаем все, что включили и возвращаем ошибку
					if(StartUpCounter > 0x1000)
					{
						RCC->CR &= ~RCC_CR_HSEON; //Останавливаем HSE
						return 1;
					}
				}
			 
    SET_BIT(RCC->CR, RCC_CR_CSSON); //Включим CSS 
		MODIFY_REG(RCC->CFGR, RCC_CFGR_SW, RCC_CFGR_SW_HSE);		
	  MODIFY_REG(RCC->CFGR, RCC_CFGR_HPRE, RCC_CFGR_HPRE_DIV1); //AHB Prescaler /1/ НА AHB ТАКАЯ ЖЕ ЧАСТОТА КАК НА ЯДРЕ
		MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE1, RCC_CFGR_PPRE1_DIV1); //APB1 Prescaler /2, т.к. PCLK1 max 50MHz
		MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE2, RCC_CFGR_PPRE2_DIV1); //APB2 Prescaler /1. Тут нас ничего не ограничивает. 100MHz max 		
		RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC;       //PLL SOURCE - HSE			
		MODIFY_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLLM, RCC_PLLCFGR_PLLM_3);  //ДЕЛИТЕЛИ-УМНОЖИТЕЛИ M/N/P
		MODIFY_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLLN, RCC_PLLCFGR_PLLN_3 | RCC_PLLCFGR_PLLN_6 | RCC_PLLCFGR_PLLN_7);
		MODIFY_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLLP, RCC_PLLCFGR_PLLP_0);	 
		MODIFY_REG(FLASH->ACR, FLASH_ACR_LATENCY, FLASH_ACR_LATENCY_2WS);   //ВАЖНО! These bits represent the ratio of the CPU clock period to the Flash memory access time. 2wS = 2 wait states		
		
		RCC->CR |= RCC_CR_PLLON; 
		MODIFY_REG(RCC->CFGR, RCC_CFGR_SW, RCC_CFGR_SW_PLL); //выбираем PLL как источник тактирования
				//ожидание флага готовности НЕ по-простому, на случай выхода из строя КВАРЦА HSE
				//Ждем успешного запуска или окончания тайм-аута
				for(StartUpCounter=0; ; StartUpCounter++)
				{
			    //Если успешно запустилось, то выходим из цикла
					if(RCC->CR & (1<<RCC_CR_PLLRDY_Pos))
						break;
			    //Если не запустилось, то отключаем все, что включили и возвращаем ошибку
					if(StartUpCounter > 0x1000)
					{
						RCC->CR &= ~RCC_CR_HSEON_Pos; //Останавливаем HSE
						RCC->CR &= ~RCC_CR_PLLON_Pos; //Останавливаем PLL
						return 2;
					}
				}
						RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN;
						RCC->APB1ENR |= RCC_APB1ENR_TIM3EN | RCC_APB1ENR_TIM2EN | RCC_APB1ENR_TIM4EN | RCC_APB1ENR_USART2EN;
						RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;   //тактирование для ADC1
						RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;  //тактирование DMA2(с его помощью будет передача данных от ADC1
						
				return 0;
		}