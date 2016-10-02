/*
 * adc.c
 *
 * Created: June-16-16, 8:41:20 AM
 *  Author: K. C. Lee
 * Copyright (c) 2016 by K. C. Lee
 
	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.

	If not, see http://www.gnu.org/licenses/gpl-3.0.en.html
 */ 
 
#include "hardware.h"

void ADC_Init(void)
{	
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
	RCC->APB2ENR |= RCC_APB2ENR_ADCEN;
	
	// ADC clock = PCLK/2 = 4MHz
	ADC1->CFGR2 = ADC_CFGR2_CKMODE_0;
	
  // Disable ADC
	ADC1->CR &= ~ADC_CR_ADEN;

  // Start calibration
	ADC1->CR |= ADC_CR_ADCAL;
  
	while (ADC1->CR & ADC_CR_ADCAL)
    /* wait */;
	
	// Sampling time = 13.5 ADClk = 1.125us
	ADC1->SMPR = ADC_SMPR_SMP_1;

	// ADC: Trigger on rising edge, trigger on TIM3, Continuous, 
	// Overwrite old data, align, Res, scan up
	// DMA one shot, DMA Enable
	ADC1->CFGR1 = ADC_CFGR1_EXTEN_0|ADC_CFGR1_EXTSEL_0|ADC_CFGR1_EXTSEL_1|ADC_CFGR1_EXTSEL_0|
								ADC_CFGR1_OVRMOD|ADC_RES|ADC_ALIGN|ADC_CFGR1_DMAEN;
	
  TIM3->ARR = ADC_SAMPLE_PERIOD-1;	
  // TIM3 /1, update request, enable TIM3
  TIM3->CR1 = TIM_CR1_URS|TIM_CR1_CEN;
	// TIM3 TRG0 = Update Events
	TIM3->CR2 = TIM_CR2_MMS_1;
	
	// Disable DMA
	DMA1_Channel1->CCR &= ~DMA_CCR_EN;
	// ADC DMA Ch1
	DMA1_Channel1->CPAR = (uint32_t) &ADC1->DR;

  NVIC_SetPriority(DMA1_Ch1_IRQn,ADC_DMA_IRQ_PRIORITY);	
  NVIC_SetPriority(ADC1_COMP_IRQn,ADC_IRQ_PRIORITY);
	NVIC_EnableIRQ(DMA1_Ch1_IRQn);
  NVIC_EnableIRQ(ADC1_COMP_IRQn);	
}

void ADC_Start(void)
{
	// Disable DMA
	DMA1_Channel1->CCR &= ~DMA_CCR_EN;

	if(Audio_Data.Conv_Batt)
	{
		ADC1->CHSELR = ADC_BATTERY;
		DMA1_Channel1->CNDTR = sizeof(Audio_Data.Batt)/sizeof(uint16_t);	
		DMA1_Channel1->CMAR = (uint32_t)&Audio_Data.Batt;
		
		// set priority, size, increment memory,read from peripheral,IRQ,enable
		DMA1_Channel1->CCR = ADC_DMA_PRIORITY|ADC_DMA_MSIZE|ADC_DMA_PSIZE|
												 DMA_CCR_MINC|DMA_CCR_TCIE|DMA_CCR_EN;
	}
	else
	{
		ADC1->CHSELR = ADC_SRC_GROUP;		
		DMA1_Channel1->CNDTR = ADC_AUDIO_SAMPLES*ADC_MAX_CH;	
		DMA1_Channel1->CMAR = (uint32_t) &Audio_Data.AudioBuffer;
		
		// set priority, size, increment memory,read from peripheral,IRQ,enable
		DMA1_Channel1->CCR = ADC_DMA_PRIORITY|ADC_DMA_MSIZE|ADC_DMA_PSIZE|
												 DMA_CCR_MINC|DMA_CCR_TCIE|DMA_CCR_HTIE|DMA_CCR_EN;
	}
	
	// Enable ADC, start conversion	
	ADC1->CR |= ADC_CR_ADEN|ADC_CR_ADSTART;
}

void DMA1_Channel1_IRQHandler(void)
{
	if(DMA1->ISR & DMA_ISR_HTIF1)
	{
		// half done
		DMA1->IFCR = DMA_IFCR_CHTIF1;
		Audio_Data.Conv_HalfDone = 1;
		Audio_Data.Conv_Done = 1;
	}
	else if(DMA1->ISR & DMA_ISR_TCIF1)
	{
		// Clear global interrupt flag
		// Full transfer done
		DMA1->IFCR = DMA_IFCR_CTCIF1;
		Audio_Data.Conv_HalfDone = 0;
		
		if(!Audio_Data.Conv_Batt)
			Audio_Data.Conv_Done = 1;	
		
		Audio_Data.Conv_Batt = !Audio_Data.Conv_Batt;
		ADC_Start();
	}
}
