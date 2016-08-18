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

Audio_t Audio_Data;
FFT_t FFT;
int32_t Peak_Decay;

// 0VU is 1.737V peak, -51 to +3VU in 3dB steps scaled to 2047 
const uint32_t dB_Table[]=
{
	0xC2,0x112,0x183,0x223,0x305,0x444,0x607,0x884,
	0xC07,0x10FE,0x1800,0x21E7,0x2FE4,0x43A5,0x5F8E,0x86F9,
	0xBEA8,0x10D50,0x17C6A,ADC_MAX*ADC_BLOCK_SIZE
};

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

const uint32_t fft_dBScale[] =
{
	0x7FFFFF,0x5A9DF7,0x4026E7,0x2D6A86,0x2026F2,0x16C310,0x101D3F,0xB6873,
	0x81385,0x5B7B1,0x40C37,0x2DD95,0x20756,0x16FA9,0x10449,0xB844,
	0x8273,0x5C5A,0x4161,0x2E49,0x20C4,0x1732,0x106C,0xBA0,
	0x83B,0x5D3,0x420,0x2EB,0x211,0x176,0x109,0xBB,
	0x84,0x5E,0x42,0x2F,0x21,0x17,0x10
};

const uint8_t fft_noise_floor[] =
{
	0x40,0x30
};

void Audio_Init(void)
{
	Peak_Decay = dB_Table[dB_TBL_ENTERIES-2]/PEAK_DECAY_SAMPLES;
	memset(&Audio_Data,0,sizeof(Audio_Data));
}

// This sums the raw ADC samples in the buffer
void Audio_Processing(void)
{
	int16_t i,j, *Cur, *Start, Volume;
	int32_t Sum[ADC_MAX_CH]={0}, Volumes[ADC_MAX_AUD_CH]={0};
	
	GPIOA->BSRR = PIN_SET(CTRL0);
	
	if(Audio_Data.Conv_HalfDone)
		Start = Cur = &Audio_Data.AudioBuffer[0];
	else
		Start = Cur = &Audio_Data.AudioBuffer[ADC_MAX_CH*ADC_BLOCK_SIZE];
	
	// Summing loop
	for(i=ADC_BLOCK_SIZE;i;i--)
	{
		Sum[0]+= *Cur++;
		Sum[1]+= *Cur++;
	}
	
	// Offset = Averages
	for(i=0;i< ADC_MAX_CH;i++)
		Audio_Data.Averages[i] = Sum[i]/ADC_BLOCK_SIZE;
	
	Cur = Start;

	// Volume = sum(ABS(Values-offset))
	for(i=ADC_BLOCK_SIZE;i;i--)
	{
		Volume = *Cur++ - Audio_Data.Averages[0];
		Volumes[0]+=(Volume>=0)?Volume:-Volume;
		Volume = *Cur++ - Audio_Data.Averages[1];
		Volumes[1]+=(Volume>=0)?Volume:-Volume;
	}
	
	// Threshold detection
	Audio_Data.Loudness = 0;
	
	for(i=0;i<ADC_MAX_SRC;i++)
	{	
		Volume = 0;
		
		for(j=0;j<ADC_CH_PER_SRC;j++)
		{
			if(Volumes[i*ADC_CH_PER_SRC+j]>=LOUDNESS_THRESHOLD)
			{ 
				Volume = Audio_Loud;
				Audio_Data.Loud_Cnt[i] = LOUDNESS_TIMEOUT;
			}
			else if(Volumes[i*ADC_CH_PER_SRC+j]>=AUDIO_THRESHOLD)
			{
				Volume = Audio_Detect;
				Audio_Data.Detect_Cnt[i] = AUDIO_TIMEOUT;
			}
			else if(Audio_Data.Detect_Cnt[i]>AUDIO_COUNTDOWN_TH)
				Volume = Audio_Sporadic;
			else
				Volume =  Audio_None;
		}
		
		if((Volume != Audio_Loud) && Audio_Data.Loud_Cnt[i])
			Audio_Data.Loud_Cnt[i]--;
		if((Volume != Audio_Detect) && Audio_Data.Detect_Cnt[i])
			Audio_Data.Detect_Cnt[i]--;
		
		Audio_Data.Loudness |= (Volume &0x03)<<(i*2);
	}
	
	// Average volume
	for(j=0;j<ADC_MAX_AUD_CH;j++)
	{
		Audio_Data.Average_Volume[j] = (int32_t)(Audio_Data.Average_Volume[j]*(ADC_AVERAGES-1) +
																   Volumes[j])/ADC_AVERAGES;
		
		if(Volumes[j]>=(int32_t)Audio_Data.Peak_Volume[j])
			Audio_Data.Peak_Volume[j] = Volumes[j];
		else if((int32_t)Audio_Data.Peak_Volume[j]>=Peak_Decay)
			Audio_Data.Peak_Volume[j]-= Peak_Decay;
		else
			Audio_Data.Peak_Volume[j] = 0;
	}
	GPIOA->BSRR = PIN_CLR(CTRL0);
}

void Spectrum(void)
{
	uint16_t i, start, DC_Offset;
	int16_t *Cur;
	uint32_t *Mag;
	int16comp_t *Sp;
	
	GPIOA->BSRR = PIN_SET(CTRL0);
	
	if(Audio_Data.Conv_HalfDone)
		Cur = &Audio_Data.AudioBuffer[0];
	else
		Cur = &Audio_Data.AudioBuffer[ADC_MAX_CH*ADC_BLOCK_SIZE];		

	start = 0;
	DC_Offset = Audio_Data.Averages[start];

	Sp = &FFT.fft_data[0];
	Cur += start;
	
	memset(&FFT,0,sizeof(FFT));
	
	// subtract offset and hanning window	
	for(i=0;i<ADC_BLOCK_SIZE;i++)
	{  
		Sp->r = ((int32_t)(Cur[0]-DC_Offset)*(int32_t)Blackman_256pt[i])>>12;
		Sp++;
		Cur += ADC_MAX_CH;
	}
	
	int16fft_exec(FFT.fft_data);
	
	Mag = FFT.fft_mag;
	
	for(i=0;i< N_FFT / 2;i++)
	{
		Sp = &FFT.fft_data[Tbl_brev[i]];
		*Mag++ = (uint32_t)(Sp->r*Sp->r) + (uint32_t)(Sp->i*Sp->i);
	}
	
	// surpress the noise floor
	for(i=0;i<sizeof(fft_noise_floor);i++)
	  if(FFT.fft_mag[i] >= fft_noise_floor[i])
		  FFT.fft_mag[i] -= fft_noise_floor[i];
		else
			FFT.fft_mag[i] = 0;
		
	GPIOA->BSRR = PIN_CLR(CTRL0);
}

void Plot_Spectrum(void)
{
	uint16_t i,j,k,bm;
	int16_t l;
	uint32_t *Mag, Peak, Average;
	
	GPIOA->BSRR = PIN_SET(CTRL0);	
	memset(FFT.LCD_Buffer,0,sizeof(FFT.LCD_Buffer));
	
	// plot VU
	for(j=0;j<ADC_CH_PER_SRC;j++)
	{
		Peak = Audio_Data.Peak_Volume[Audio_Data.Selected+(1-j)];
		Average = Audio_Data.Average_Volume[Audio_Data.Selected+(1-j)];	
		
		if(Peak < Average)
			Peak = Average;
	
		// Peak -> dB lookup		
		for(l=dB_TBL_ENTERIES-1;l>=0;l--)
			if((Peak >= dB_Table[l])&&(Peak <= dB_Table[l+1]))
				break;
		
		i=SPECTRUM_ROWS-l/(LCD_PIX_PER_ROW/2)-1;		
		bm = 1<<((LCD_PIX_PER_ROW/2-l%(LCD_PIX_PER_ROW/2))*2+1);
		bm |=	bm>>1;
			
		for(k=0;k<SPECTRUM_VU_WIDTH;k++)
			FFT.LCD_Buffer[i][SPECTRUM_VU_COL+j*(SPECTRUM_VU_WIDTH+1)+k]= bm;			

		// average -> dB lookup
		for(l=dB_TBL_ENTERIES-1;l>=0;l--)
		  if(Average >= dB_Table[l])
				break;

		bm = 0;
		i=SPECTRUM_ROWS-l/(LCD_PIX_PER_ROW/2)-1;			

		for(k=0;k<(l%(LCD_PIX_PER_ROW/2));k++)
			bm |= 1<<((LCD_PIX_PER_ROW/2-k)*2+1);

		// Draw remainder	
		for(k=0;k<SPECTRUM_VU_WIDTH;k++)
			FFT.LCD_Buffer[i][SPECTRUM_VU_COL+j*(SPECTRUM_VU_WIDTH+1)+k] |= bm;			
		
		// draw full column
		for(i++;i<=SPECTRUM_ROWS;i++)
			for(k=0;k<SPECTRUM_VU_WIDTH;k++)
				FFT.LCD_Buffer[i][SPECTRUM_VU_COL+j*(SPECTRUM_VU_WIDTH+1)+k] |= 0xaa;			
	}

	#ifdef SPECTRUM_AUTOSCALE
	// Spectrum width
	Mag = &FFT.fft_mag[ADC_BLOCK_SIZE/2];
	
	for(i=ADC_BLOCK_SIZE/2;i;i--)
	  if(*(Mag--) >=fft_dBScale[sizeof(fft_dBScale)/sizeof(uint32_t)-1])
			break;
	
	// Scaling spectrum display
	l=(i<SPECTRUM_VU_COL/2)?2:1;
	#else	
		l=1;
	#endif
		
	// Plot Spectrum		
	Mag = &FFT.fft_mag[SPECTRUM_START];	
	for(j=0;j<SPECTRUM_END;j+=l)
	{
		for(i=0;i<SPECTRUM_ROWS;i++)
		{
			bm = 0xff;
			
			for(k=0;k<LCD_PIX_PER_ROW;k++)
			{
			  if(*Mag>=fft_dBScale[i*LCD_PIX_PER_ROW+k])
					break;

				bm &= ~(1<<k);
			}
			FFT.LCD_Buffer[i][j] |= bm;
			
			#ifdef SPECTRUM_WIDEBAR
			FFT.LCD_Buffer[i][j+1] |= bm;
			#endif
		}
		Mag++;
	}
	
	LCD_CORD_XY(SPECTRUM_COL,SPECTRUM_ROW);
	LCD_DataMode();
	SPI_Block_Write((const uint8_t *)FFT.LCD_Buffer,sizeof(FFT.LCD_Buffer));
	Audio_Data.Spectrum_Blank = 0;
	GPIOA->BSRR = PIN_CLR(CTRL0);
}

void Blank_Spectrum(void)
{
	if(!Audio_Data.Spectrum_Blank)
	{
		LCD_CORD_XY(SPECTRUM_COL,SPECTRUM_ROW);
		LCD_DataMode();	
		SPI_Block_Fill(0,SPECTRUM_ROWS*LCD_MAX_X);
		Audio_Data.Spectrum_Blank = 1;
		GPIOA->BSRR = PIN_CLR(CTRL0);
	}
}
