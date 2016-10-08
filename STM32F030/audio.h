/*
 * audio.h
 *
 * Created: June-16-16, 8:34:16 AM
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

#ifndef _AUDIO_H_
#define _AUDIO_H_

#include "hardware.h"
#include "intfft.h"

void Audio_Init(void);
void Audio_Processing(void);
void Draw_VU_Legend(void);
void Draw_VUBar(int32_t Vin, int32_t Peak);
void Draw_VUBars(void);
void Spectrum(void);
void Plot_All(void);
void Blank_Spectrum(void);

enum Plot_VU
{ Plot_Average, Plot_Peak};

#pragma anon_unions

typedef struct
{
	int16_t		AudioBuffer[ADC_AUDIO_SAMPLES];
	uint32_t	Average_Volume;
	uint32_t  Peak_Volume;
	int16_t 	Offset;
	uint16_t	Avg_Batt;
	volatile  uint16_t Batt;
	uint16_t	Backlight_Cnt;
	uint8_t		Spectrum_Blank:1;
	volatile 	uint8_t Conv_Done:1;
	volatile	uint8_t Conv_HalfDone:1;
	volatile	uint8_t Conv_Batt:1;
} Audio_t;

typedef struct
{
	union
	{ 
		int16comp_t fft_data[ADC_BLOCK_SIZE];	
	  uint8_t LCD_Buffer[PLOTDATA_ROWS][LCD_MAX_X];	
	};
	uint8_t fft_mag[SPECTRUM_BIN];
	uint8_t volume[VOLUME_WIDTH];
	uint8_t noise;
	uint8_t peak;
} PlotData_t;

extern Audio_t Audio_Data;
extern PlotData_t Plot_Data;
extern const uint32_t dB_Table[];
extern const uint32_t fft_dBScale[];

#define dB_TBL_ENTERIES (sizeof(dB_Table)/sizeof(int32_t)-1)
#define PEAK_VOL_MULT		8UL

#endif
