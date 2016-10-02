/*
 * audio.c
 *
 * Created: June-20-16, 5:45:54 PM
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
#include "intfft.h"
#include "graphics.h"

Audio_t Audio_Data;
PlotData_t Plot_Data;
int32_t Peak_Decay;

const int16_t Blackman_256pt[] =
{
	0x000B,0x000C,0x000D,0x000F,0x0012,0x0015,0x001A,0x001F,
	0x0025,0x002D,0x0036,0x003F,0x004B,0x0057,0x0066,0x0076,
	0x0088,0x009C,0x00B2,0x00CA,0x00E5,0x0103,0x0124,0x0147,
	0x016E,0x0199,0x01C7,0x01FA,0x0230,0x026B,0x02AB,0x02EF,
	0x0339,0x0389,0x03DE,0x0439,0x049B,0x0503,0x0572,0x05E8,
	0x0665,0x06EA,0x0777,0x080C,0x08A9,0x094F,0x09FE,0x0AB7,
	0x0B78,0x0C43,0x0D18,0x0DF7,0x0EE0,0x0FD3,0x10D1,0x11D9,
	0x12EC,0x140A,0x1533,0x1667,0x17A5,0x18EF,0x1A43,0x1BA3,
	0x1D0D,0x1E82,0x2002,0x218C,0x2321,0x24C0,0x2669,0x281B,
	0x29D7,0x2B9C,0x2D6A,0x2F40,0x311F,0x3305,0x34F2,0x36E6,
	0x38E0,0x3AE0,0x3CE4,0x3EEE,0x40FB,0x430C,0x451F,0x4735,
	0x494B,0x4B63,0x4D7A,0x4F90,0x51A5,0x53B7,0x55C7,0x57D2,
	0x59D9,0x5BDA,0x5DD5,0x5FC8,0x61B4,0x6398,0x6571,0x6741,
	0x6905,0x6ABE,0x6C6A,0x6E09,0x6F9A,0x711C,0x728F,0x73F2,
	0x7544,0x7685,0x77B4,0x78D1,0x79DA,0x7AD1,0x7BB3,0x7C81,
	0x7D3B,0x7DE0,0x7E6F,0x7EE8,0x7F4C,0x7F9A,0x7FD2,0x7FF3,
	0x7FFF,0x7FF3,0x7FD2,0x7F9A,0x7F4C,0x7EE8,0x7E6F,0x7DE0,
	0x7D3B,0x7C81,0x7BB3,0x7AD1,0x79DA,0x78D1,0x77B4,0x7685,
	0x7544,0x73F2,0x728F,0x711C,0x6F9A,0x6E09,0x6C6A,0x6ABE,
	0x6905,0x6741,0x6571,0x6398,0x61B4,0x5FC8,0x5DD5,0x5BDA,
	0x59D9,0x57D2,0x55C7,0x53B7,0x51A5,0x4F90,0x4D7A,0x4B63,
	0x494B,0x4735,0x451F,0x430C,0x40FB,0x3EEE,0x3CE4,0x3AE0,
	0x38E0,0x36E6,0x34F2,0x3305,0x311F,0x2F40,0x2D6A,0x2B9C,
	0x29D7,0x281B,0x2669,0x24C0,0x2321,0x218C,0x2002,0x1E82,
	0x1D0D,0x1BA3,0x1A43,0x18EF,0x17A5,0x1667,0x1533,0x140A,
	0x12EC,0x11D9,0x10D1,0x0FD3,0x0EE0,0x0DF7,0x0D18,0x0C43,
	0x0B78,0x0AB7,0x09FE,0x094F,0x08A9,0x080C,0x0777,0x06EA,
	0x0665,0x05E8,0x0572,0x0503,0x049B,0x0439,0x03DE,0x0389,
	0x0339,0x02EF,0x02AB,0x026B,0x0230,0x01FA,0x01C7,0x0199,
	0x016E,0x0147,0x0124,0x0103,0x00E5,0x00CA,0x00B2,0x009C,
	0x0088,0x0076,0x0066,0x0057,0x004B,0x003F,0x0036,0x002D,
	0x0025,0x001F,0x001A,0x0015,0x0012,0x000F,0x000D,0x000C
};

const uint32_t dB_Table[]=
{
	0,
	/*0xC2,0x112,0x1830,x223,*/
	0x305,0x444,0x607,0x884,
	0xC07,0x10FE,0x1800,0x21E7,0x2FE4,0x43A5,0x5F8E,0x86F9,
	0xBEA8,0x10D50,0x17C6A,0x21959,
	ADC_MAX*ADC_BLOCK_SIZE
};

const uint32_t fft_dBScale[] =
{
  0,
	0x00000E,0x00001A,0x00002F,0x000053,
	0x000095,0x000109,0x0001D7,0x000346,
	0x0005D3,0x000A5C,0x00126D,0x0020C4,
	0x003A45,0x00679F,0x00B844,0x0147AE,
	0x0246B4,0x040C37,0x0732AE,0x0CCCCC,
	0x16C310,0x287A26,0x47FACC,0x7FFFFF,
	0xffffffff
};

// binary search table lookup
int8_t Lookup(const uint32_t Value, const uint32_t *Table, uint16_t Last)
{
  uint16_t Low=0, Mid;
	
  while(Low<=Last)
	{
		Mid = (Low+Last)/2;
		
		if(Table[Mid]<=Value)
		{
			if(Value<Table[Mid+1])
			  return(Mid);

			Low = Mid+1;
		}
		else
			Last = Mid-1;
	}
	return(0);
}

void Audio_Init(void)
{
	Peak_Decay = dB_Table[dB_TBL_ENTERIES-2]/PEAK_DECAY_RATE;
	DMA_memset(&Audio_Data,0,sizeof(Audio_Data));
	DMA_memset(&Plot_Data,0,sizeof(Plot_Data));
}

// This sums the raw ADC samples in the buffer
void Audio_Processing(void)
{
	int16_t i, *Cur, *Start, Volume;
	int32_t Sum=0, VolumeSum =0;
	
	GPIOA->BSRR = PIN_SET(CTRL0);
	
	// Give DMA a bit of head start
	DMA_memmove(Plot_Data.volume,Plot_Data.volume+1,sizeof(Plot_Data.volume)-1);
	
	if(Audio_Data.Conv_HalfDone)
		Start = Cur = &Audio_Data.AudioBuffer[0];
	else
		Start = Cur = &Audio_Data.AudioBuffer[ADC_MAX_CH*ADC_BLOCK_SIZE];
	
	// Summing loop
	for(i=ADC_BLOCK_SIZE;i;i--)
		Sum+= *Cur++;
	
	// Offset = Averages
	for(i=0;i< ADC_MAX_CH;i++)
		Audio_Data.Offset = Sum/ADC_BLOCK_SIZE;
	
	Cur = Start;
	Volume = 0;
	
	// Volume = sum(ABS(Values-offset))
	for(i=ADC_BLOCK_SIZE;i;i--)
	{
		Volume = *Cur++ - Audio_Data.Offset;
		VolumeSum +=(Volume>=0)?Volume:-Volume;
	}
	
	// Average volume
	Audio_Data.Average_Volume = (int32_t)(Audio_Data.Average_Volume*(ADC_AVERAGES-1) +
																 VolumeSum)/ADC_AVERAGES;
	
	if(VolumeSum >=Audio_Data.Peak_Volume)
		Audio_Data.Peak_Volume = VolumeSum;
	else if(Audio_Data.Peak_Volume >= Peak_Decay)
		Audio_Data.Peak_Volume -= Peak_Decay;
	else
		Audio_Data.Peak_Volume = 0;
	
	Plot_Data.volume[sizeof(Plot_Data.volume)-1] = Lookup(VolumeSum,dB_Table,sizeof(dB_Table)/sizeof(uint32_t)-2);
	
		// Battery voltage
	Audio_Data.Avg_Batt += (Audio_Data.Batt - Audio_Data.Avg_Batt)/ADC_BATT_AVERAGES;
	GPIOA->BSRR = PIN_CLR(CTRL0);
}

void Spectrum(void)
{
	uint16_t i, start, DC_Offset;
	int16_t *Cur;
	uint8_t *Mag;
	int16comp_t *Sp;
	uint32_t Magnitude;
	
	GPIOA->BSRR = PIN_SET(CTRL0);
	
	if(Audio_Data.Conv_HalfDone)
		Cur = &Audio_Data.AudioBuffer[0];
	else
		Cur = &Audio_Data.AudioBuffer[ADC_MAX_CH*ADC_BLOCK_SIZE];		

	start = 0;
	DC_Offset = Audio_Data.Offset;

	Sp = &Plot_Data.fft_data[0];
	Cur += start;
	
	memset(&Plot_Data.fft_data,0,sizeof(Plot_Data.fft_data));
	//DMA_memset((uint8_t *)&Plot_Data.fft_data,0,sizeof(Plot_Data.fft_data));
	
	// subtract offset and hanning window	
	for(i=0;i<ADC_BLOCK_SIZE;i++)
	{  
		Sp->r = ((int32_t)(Cur[0]-DC_Offset)*(int32_t)Blackman_256pt[i])>>11;
		Sp++;
		Cur += ADC_MAX_CH;
	}
	
	int16fft_exec(Plot_Data.fft_data);
	
	Mag = Plot_Data.fft_mag;
	
	for(i=0;i< N_FFT/2;i++)
	{
		Sp = &Plot_Data.fft_data[Tbl_brev[i]];
		Magnitude = (uint32_t)(Sp->r*Sp->r) + (uint32_t)(Sp->i*Sp->i);
		
		*Mag++ = Lookup(Magnitude,fft_dBScale,sizeof(fft_dBScale)/sizeof(uint32_t)-2);
	}

	GPIOA->BSRR = PIN_CLR(CTRL0);
}

void Plot_Batt(uint16_t mV)
{
	Gfx_Moveto(BATT_COL,BATT_ROW);
	Gfx_HBar((mV - BATT_LOW_END)/BATT_BAR_RES,BATT_BM,BATT_BAT_WIDTH);
	
	mV /= 10;
	Gfx_Moveto(BATT_COL,BATT_ROW);	
	Gfx_Print_uint(mV/FP_2D,1,RightJustify);
	Gfx_PutCh('.');
	Gfx_Moveto(BATT_COL+Font_3x5_WIDTH+3,BATT_ROW);	
	Gfx_Print_uint(mV%FP_2D,10,LeadingZero);
	Gfx_PutCh(Font_3x5_V);
}

void Plot_All(void)
{	
	const uint8_t SpectrumGrid[] = { 7, 13, 20, 26, 32, 38, 45, 51, 57, 63 };
	uint8_t update, Peak, Volume, x, y0, y1;
	uint16_t i;
	
	GPIOA->BSRR = PIN_SET(CTRL0);		
	update = (Audio_Data.Average_Volume >= AUDIO_THRESHOLD);
	
	if(update)
	{	
		// Plot Volume
		Peak = Plot_Data.volume[0];

		for(i=0;i<VOLUME_WIDTH;i++)
		{ 
			if(Peak < Plot_Data.volume[i])
				Peak = Plot_Data.volume[i];	
		}	
		// plot VU
		Volume = Lookup(Audio_Data.Average_Volume,dB_Table,sizeof(dB_Table)/sizeof(uint32_t)-2);	
		
		Peak = Lookup(Audio_Data.Peak_Volume,dB_Table,sizeof(dB_Table)/sizeof(uint32_t)-2);	
		
		if(Peak < Volume)
			Peak = Volume;	
	
		Audio_Data.Backlight_Cnt = BACKLIGHT_TIMEOUT;
		GPIOB->BSRR = PIN_CLR(BACKLIGHT);

		Spectrum();
		DMA_memset(Plot_Data.LCD_Buffer,0,sizeof(Plot_Data.LCD_Buffer));
	
		Gfx_Moveto(VU_COL,VU_ROW);
		Gfx_VBar(Volume,VU_ROWS,VU_WIDTH,Bar_Full|Bar_Dash);		

		Gfx_Moveto(VU_COL,VU_ROW);
		Gfx_VBar(Peak,VU_ROWS,VU_WIDTH,Bar_PeakOnly|Bar_Thick);
		
		for(i=0;i<sizeof(SpectrumGrid)/sizeof(uint8_t);i++)
		{
			x = SPECTRUM_COL+SpectrumGrid[i];
			Plot_Data.LCD_Buffer[SPECTRUM_GRID_ROW][x]|=(i&0x01)?SPECTRUM_GRID_LONG:SPECTRUM_GRID_SHORT;
		}
		
		Gfx_Moveto(SPECTRUM_COL,SPECTRUM_ROW);
		
		// consolidate 2 bins
		for(i=0;i<SPECTRUM_WIDTH;i++)
		{
			Peak = Plot_Data.fft_mag[SPECTRUM_START+i*SPECTRUM_BIN_INC];
			
			if(Peak<Plot_Data.fft_mag[SPECTRUM_START+i*SPECTRUM_BIN_INC+1])
				Peak = Plot_Data.fft_mag[SPECTRUM_START+i*SPECTRUM_BIN_INC+1];
			
			Gfx_VBar(Peak,SPECTRUM_ROWS,1,Bar_Full|Bar_Narrow);
		}	
		Audio_Data.Spectrum_Blank = 0;		
	}
	else
	{
		if(Audio_Data.Backlight_Cnt)
			Audio_Data.Backlight_Cnt--;
		else
			GPIOB->BSRR = PIN_SET(BACKLIGHT);
		
		DMA_memset(Plot_Data.LCD_Buffer,0,
							 Audio_Data.Spectrum_Blank?LCD_MAX_X*VOLUME_ROWS:
							 sizeof(Plot_Data.LCD_Buffer));
	}

	Plot_Batt((uint32_t)((Audio_Data.Avg_Batt*ADC_BATT_SCALE_MULT)/ADC_BATT_SCALE_DIV));
	
	Gfx_Moveto(VOLUME_COL,VOLUME_ROW);
	y0 = Plot_Data.volume[0];
	
	if(y0 > VOLUME_ROWS*LCD_PIX_PER_ROW-1)
	  y0 = VOLUME_ROWS*LCD_PIX_PER_ROW-1;
	
	for(i=1;i<VOLUME_WIDTH;i++)
	{ 
		y1 = Plot_Data.volume[i];
		
		if(y1 > VOLUME_ROWS*LCD_PIX_PER_ROW-1)
			y1 = VOLUME_ROWS*LCD_PIX_PER_ROW-1;		
		
		Gfx_Plot(y0,y1);
		y0 = y1;
	}
	
	LCD_Cord_XY(SPECTRUM_COL,PLOTDATA_ROW);
	LCD_DataMode();
	SPI_Block_Write((const uint8_t *)Plot_Data.LCD_Buffer,
									Audio_Data.Spectrum_Blank?LCD_MAX_X*VOLUME_ROWS:
									sizeof(Plot_Data.LCD_Buffer));	
	
	if(!update && !Audio_Data.Spectrum_Blank)
		Audio_Data.Spectrum_Blank = 1;

	GPIOA->BSRR = PIN_CLR(CTRL0);
}
