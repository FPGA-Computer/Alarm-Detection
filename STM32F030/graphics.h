/*
 * graphics.h
 *
 * Created: September-19-16, 12:10:04 PM
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

#ifndef _GRAPHICS_H_
#include <stdint.h>
#include "font.h"
#include "main.h"
#include "audio.h"

enum GVBars_Format
{
	Bar_PeakOnly = 0x00, Bar_Full = 0x01,
	Bar_Narrow = 0x00, Bar_Thick = 0x02, Bar_Dash = 0x03,
	Bar_CursorStay = 0x04,
	
	Bar_Narrow_YScale = 1, Bar_Thick_YScale = 2,
	
	BM_Bar_Narrow = 0x01, BM_Bar_Narrow_Full = 0xff,
	BM_Bar_Thick = 0x03, BM_Bar_Thick_Full = 0xaa
};

enum GPut_Format
{
	LeftJustify = 0x01, RightJustify = 0x00, LeadingZero = 0x02
};

extern uint8_t GCur_X, GCur_Y;

void Gfx_Moveto(uint8_t X, uint8_t Y);
void Gfx_PutCh(uint8_t ch);
void Gfx_Print_uint(uint16_t n, uint16_t Div, uint8_t Format);
void Gfx_VBar(uint8_t Value,uint8_t Height,uint8_t Scale,uint8_t Format);
void Gfx_HBar(uint8_t Value,uint8_t Bitmap,uint8_t Width);
void Gfx_Plot(uint8_t Y0, uint8_t Y1);
#endif

