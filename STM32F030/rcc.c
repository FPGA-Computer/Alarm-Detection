/*
 * rcc.c
 *
 * Created: March-14-16, 4:41:54 PM
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
 
#include "stm32f0xx.h"
#include "core_cm0.h"
#include "audio.h"

// Runs before main() set up system clock

void SystemInit(void)
{
	// FLASH default: prefetch = off, wait state to 0
	
	// SYSCLK = HSE = 8MHz, PLL = OFF
	RCC->CR |= RCC_CR_HSEON;

	while(!(RCC->CR & RCC_CR_HSERDY))
		/* wait for HSE ready*/;

	// Switch to HSE as clock source
	RCC->CFGR |= RCC_CFGR_SW_HSE;
	
	// wait for clock switching
	while((RCC->CFGR & RCC_CFGR_SWS)!= RCC_CFGR_SWS_HSE)
	 ;	
	// Turn off HSI
	RCC->CR &= ~RCC_CR_HSION;
}
