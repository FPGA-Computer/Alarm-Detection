/*
 * main.c
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
#include "font.h"

uint32_t Prev_Time=0xffff;
uint8_t Ticks;

int main(void)
{ 
	Hardware_Init();
	Audio_Init();

	ADC_Start();
	
	__WFI( 	);	
	while(1)
	{	
		if(Audio_Data.Conv_Done)
		{ 		
			Audio_Data.Conv_Done = 0;
			
			Audio_Processing();
			UpdateDisplay();
			__WFI( 	);
		}
	}
}

void UpdateDisplay(void)
{
	if(Ticks)
		Ticks--;
	else
	{
		Ticks = TICK_RELOAD;
		
		if(!(Audio_Data.Loudness & (0x03<<Audio_Data.Selected*2)))
			Blank_Spectrum();
		else 				
		{
			Spectrum();
			Plot_Spectrum();
		}
	}
}
