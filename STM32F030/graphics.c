/*
 * graphics.c
 *
 * Created: September-19-16, 12:11:07 PM
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

#include "graphics.h"

uint8_t GCur_X, GCur_Y, *GBuf;

void Gfx_Moveto(uint8_t X, uint8_t Y)
{
	GCur_X = X;
	GCur_Y = Y;
	GBuf = &Plot_Data.LCD_Buffer[Y][X];
}

void Gfx_PutCh(uint8_t ch)
{
	uint8_t i;
	const uint8_t *Src;
	
  if((ch >= Font_3x5_CH_LOW) && (ch <= Font_3x5_CH_HIGH))
	{ 
		for(i = Font_3x5_WIDTH,Src = &Font_3x5[(ch-Font_3x5_CH_LOW)*Font_3x5_WIDTH];i;i--)
	    *GBuf++ |= *Src++;
		
		GCur_X += Font_3x5_WIDTH+1;
		GBuf++;
	}
}

void Gfx_Print_uint(uint16_t Value, uint16_t Div, uint8_t Format)
{
	do
  { 
    if((Format & LeadingZero)||(Value >= Div)||(Div == 1))
    { 
			Format = LeadingZero;
      Gfx_PutCh('0'+Value/Div);
      Value %= Div;
     }
    else if(Format & LeftJustify)
    {
			GCur_X += Font_3x5_WIDTH;
			GBuf += Font_3x5_WIDTH;
		}

    Div /= 10;
  } while(Div);
}

void Gfx_HBar(uint8_t Value, uint8_t Bitmap, uint8_t Width)
{
	if(Value > Width)
		Value = Width;
	
	GCur_X += Value;
	
	for(;Value;Value--)
		*GBuf++ |= Bitmap;
}

void Gfx_VBar(uint8_t Value, uint8_t Height, uint8_t Width, uint8_t Format)
{
	uint8_t Bitmap, YScale, Rows, i, y, *ptr;
	
	if(Format & Bar_Thick)
	{
		YScale = Bar_Thick_YScale;
		Bitmap = (Format & Bar_Full)?BM_Bar_Thick_Full:BM_Bar_Thick;
	}
	else
	{
		YScale = Bar_Narrow_YScale;
		Bitmap = (Format & Bar_Full)?BM_Bar_Narrow_Full:BM_Bar_Narrow;
	}
	
	Rows = LCD_PIX_PER_ROW/YScale;
	
	if(Value > Height * Rows)
		Value = Height * Rows;
	
	y = Value/Rows;
	Value %= Rows;
	
	Bitmap <<= (Rows-Value)*YScale;

	for(ptr = GBuf-y*LCD_MAX_X,i=0;i<Width;i++)
	  ptr[i] |= Bitmap;
	
	if(Format & Bar_Full)
	{
		Bitmap = (Format & Bar_Thick)?BM_Bar_Thick_Full:BM_Bar_Narrow_Full;

		for(ptr = ptr+LCD_MAX_X;y;y--,ptr+=LCD_MAX_X)
			for(i=0;i<Width;i++)
			  ptr[i] |= Bitmap;
	}
	
	if(Format & Bar_CursorStay)
		return;
	
	GBuf += Width;
	GCur_X += Width;
}

void Gfx_Set(uint8_t Y)
{
	uint8_t Row, *ptr;
	
	Row = Y/LCD_PIX_PER_ROW;
	Y %= LCD_PIX_PER_ROW;
		
	ptr = GBuf-Row*LCD_MAX_X;
	*ptr |= BM_Bar_Narrow << (LCD_PIX_PER_ROW-1-Y);
}

void Gfx_Plot(uint8_t Y0, uint8_t Y1)
{
  int8_t dy, y, y_mid;
	uint8_t *x1;
	
	dy = (int8_t)Y1 - (int8_t)Y0;
	y_mid = (Y0 + Y1)/2;
	y = Y0;
	x1 = GBuf+1;
	
	if(dy > 0)
		dy = 1;
	else if (dy < 0)
	  dy = -1;
	
  do
	{		
		Gfx_Set(y);
		y += dy;
		
		if(y==y_mid)
			GBuf=x1;

	} while(y!=Y1);
	
	GBuf=x1;
}
