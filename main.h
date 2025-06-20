#ifndef MAIN_H__
#define MAIN_H__

#include "stm32f411xe.h"
#include "stm32f4xx.h"

#include <string.h>
#include <stdio.h>
#include <stdint.h>

#include "rcc.c"
#include "delay_on_tim3.c"
#include "adc.c"
#include "dma.c"
#include "usart.c"
#include "dma1.c"



void GPIO_Init(void);
void ADC_Init(void);

void DMA1_RX_func(void);
#endif