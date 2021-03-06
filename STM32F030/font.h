/*
 * font.h
 *
 * Created: June-18-16, 2:46:41 PM
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

#ifndef _FONT_5X8_H_
#define _FONT_5X8_H_

#include <stdint.h>

extern const uint8_t Font_5x8[];
extern const uint8_t Font_3x5[];

#define Font_5x8_CH_LOW					32U
#define Font_5x8_CH_HIGH				127U

enum Font_3x5_Sym
{ 
	Font_3x5_V = '.'+1,
	//----
	Font_3x5_CH_LOW =	'.',
	Font_3x5_CH_HIGH = '9',
	//----
	Font_3x5_WIDTH = 3,
	Font_3x5_HEIGHT = 5
};
	
#define FP_2D           100U
#define FP_MULT         1000U
#define FP_DIGIT        (FP_MULT/10)
#endif
