/*
 * hardware.h
 *
 * Created: July-10-16, 4:09:33 PM
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

#ifndef _MAIN_H_
#define _MAIN_H_

#include <stdint.h>
#include "stm32f0xx.h"
#include "core_cm0.h"
#include <string.h>

void Task_Update_Volume(void);
void Task_ADC_Sample(void);
void UpdateTime(void);
void UpdateDisplay(void);
void ChangeDisplayMode(uint8_t Mode);
void Update_AudioSource(void);
void Draw_AudioSource_Label(void);
extern uint8_t Ticks;

// DC offset averages
#define OFFSET_PERIOD					64
// Battery voltage averages ideally 2^n
#define ADC_BATT_AVERAGES			8

// Volume average time in ms
#define VOLUME_AVERAGE_PERIOD	300UL
// Peak volume decay time in ms
#define PEAK_VOLUME_DECAY			1500UL

#define ADC_AUDIO_SAMPLES			512UL
#define ADC_BLOCK_SIZE				(ADC_AUDIO_SAMPLES/2)

// FFT size = 256
#define	N_FFT									ADC_BLOCK_SIZE

// 8MHz/781 = 10243
#define ADC_SAMPLE_RATE				10240UL
#define ADC_SAMPLE_PERIOD			(CPU_CLOCK/ADC_SAMPLE_RATE)

// 40Hz
#define ADC_DMA_RATE					(ADC_SAMPLE_RATE/ADC_BLOCK_SIZE)
#define PEAK_DECAY_RATE				((PEAK_VOLUME_DECAY*ADC_DMA_RATE)/1000UL)
#define SCREEN_REFRESH_RATE		10UL
#define TICK_RELOAD						(ADC_DMA_RATE/SCREEN_REFRESH_RATE)
		
#define AUDIO_THRESHOLD 			0x260

#define PLOTDATA_ROW					1
#define PLOTDATA_ROWS					5

#define SPECTRUM_ROW					4
#define SPECTRUM_COL					0
#define SPECTRUM_ROWS					3
#define SPECTRUM_WIDTH				((SPECTRUM_BIN-SPECTRUM_START)/2)

#define SPECTRUM_GRID_ROW			(SPECTRUM_ROW-SPECTRUM_ROWS+1)
#define SPECTRUM_GRID_LONG		0x06
#define SPECTRUM_GRID_SHORT		0x02

#define SPECTRUM_START				1
#define SPECTRUM_BIN					(ADC_BLOCK_SIZE/2) 
#define SPECTRUM_BIN_INC			(SPECTRUM_BIN/SPECTRUM_WIDTH)
#define SPECTRUM_BIN_FREQ			(ADC_SAMPLE_RATE/N_FFT)

#define VU_ROW								4
#define VU_COL								(LCD_MAX_X-(VU_WIDTH+1))
#define VU_WIDTH							5
#define VU_ROWS								4

#define VOLUME_COL						0
#define VOLUME_COLS						1
#define VOLUME_ROW						1
#define VOLUME_ROWS						2
#define VOLUME_WIDTH					(64/VOLUME_COLS)

#define BATT_ROW							0
#define BATT_COL							(SPECTRUM_WIDTH+3)
#define BATT_BAT_WIDTH				(LCD_MAX_X - BATT_COL)
#define BATT_BM								0x40
#define BATT_LOW_END					2300
#define BATT_BAR_RES					30

#define BACKLIGHT_TIMEOUT			40
#endif
