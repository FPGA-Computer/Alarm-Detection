/*
 * hardware.c
 *
 * Created: June-16-16, 8:38:27 AM
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
 
void Hardware_Init(void)
{
	// Enable GPIO, DMA
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN|RCC_AHBENR_GPIOBEN|RCC_AHBENR_GPIOFEN|RCC_AHBENR_DMAEN;
	// Enable SPI, SYSCFG, DBGMCU, TIM16
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGCOMPEN|RCC_APB2ENR_DBGMCUEN|RCC_APB2ENR_TIM16EN;
	
	// Assign GPIOA functions
	GPIOA->MODER = GPIOA_MODER;
	GPIOA->AFR[1] = GPIOA_AFR1;

	// PB1 = Output
	GPIOB->BSRR = PIN_SET(BACKLIGHT);
	GPIOB->MODER = PIN_OUTPUT(PB1);
	
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
	
	// SPI Master, Software NSS, Slave select, MSB first, 
	// SCK=8MHz/2=4MHz, CPOL=0, CHPA=0
	SPI1->CR1 = SPI_CR1_MSTR|SPI_CR1_SSM|SPI_CR1_SSI|SPI_BR;
	
	// Data size = 8-bit, Motorola SPI mode, No NSS
	SPI1->CR2 = SPI_CR2_DS_2|SPI_CR2_DS_1|SPI_CR2_DS_0|SPI_CR2_TXDMAEN;	

	// Enable SPI
	SPI1->CR1|= SPI_CR1_SPE;
	
	// DMA Ch3 - SPI
	DMA1_Channel3->CPAR = (uint32_t) &SPI1->DR;  
	
	NVIC_SetPriority(DMA1_Ch2_3_DMA2_Ch1_2_IRQn,LCD_DMA_IRQ_PRIORITY);
  NVIC_EnableIRQ(DMA1_Ch2_3_DMA2_Ch1_2_IRQn);
	
	LCD_Init();
	ADC_Init();
}

volatile uint8_t SPI_Wait, Mem_Wait;

void DMA1_Channel2_3_IRQHandler(void)
{
	if(DMA1->ISR & DMA_ISR_TCIF3)
	{	
		DMA1->IFCR = DMA_IFCR_CTCIF3;
		SPI_Wait = 0;
	}
	if(DMA1->ISR & DMA_ISR_TCIF2)
	{	
		DMA1->IFCR = DMA_IFCR_CTCIF2;
		Mem_Wait = 0;
	}
}

void SPI_DMA_Wait(void)
{
	while(SPI_Wait)
		__WFI();
  
  // wait until SPI finishes
  while(SPI1->SR & SPI_SR_BSY)
    /* wait */ ;
}

void SPI_Block_Fill(uint8_t Fill, uint16_t size)
{
  DMA1_Channel3->CCR = 0;
  DMA1_Channel3->CMAR = (uint32_t) &Fill;
  DMA1_Channel3->CNDTR = size;
  SPI_Wait = 1;
	
  // 8-bit memory/peripheral, write, enable
  DMA1_Channel3->CCR = SPI_DMA_PRIORITY|DMA_CCR_DIR|DMA_CCR_TCIE|DMA_CCR_EN;
  SPI_DMA_Wait();  
}

void SPI_Block_Write(const uint8_t *ptr, uint16_t size)
{
  DMA1_Channel3->CCR = 0;
  DMA1_Channel3->CMAR = (uint32_t) ptr;
  DMA1_Channel3->CNDTR = size;
  SPI_Wait = 1;
	
  // 8-bit memory/peripheral, memory increment, write, enable, complete IRQ
  DMA1_Channel3->CCR = SPI_DMA_PRIORITY|DMA_CCR_MINC|DMA_CCR_DIR|DMA_CCR_TCIE|DMA_CCR_EN;
  SPI_DMA_Wait();  
}

void SPI_ByteWrite(uint8_t byte)
{
  SPI_Block_Write(&byte,1);
}

// async assending memmove using DMA - not 100% compatible
void DMA_memmove(void *Dst, void *Src, uint16_t Size)
{
  DMA1_Channel2->CCR = 0;
  DMA1_Channel2->CMAR = (uint32_t)Src;
	DMA1_Channel2->CPAR = (uint32_t)Dst;
  DMA1_Channel2->CNDTR = Size;
	
	// Memory to memory mode, 8-bit, low priority, increment
  DMA1_Channel2->CCR = DMA_CCR_EN|DMA_CCR_MEM2MEM|DMA_CCR_PINC|DMA_CCR_MINC|DMA_CCR_DIR|DMA_CCR_TCIE;
}

void DMA_memset(void *Dst, uint8_t fill, uint16_t Size)
{
  DMA1_Channel2->CCR = 0;
  DMA1_Channel2->CMAR = (uint32_t)&fill;
	DMA1_Channel2->CPAR = (uint32_t)Dst;
  DMA1_Channel2->CNDTR = Size;
	Mem_Wait = 1;
	
	// Memory to memory mode, 8-bit, low priority, increment
  DMA1_Channel2->CCR = DMA_CCR_EN|DMA_CCR_MEM2MEM|DMA_CCR_PINC|DMA_CCR_DIR|DMA_CCR_TCIE;
	
	while(Mem_Wait)
		__WFI();
}
