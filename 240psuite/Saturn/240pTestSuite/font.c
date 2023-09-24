/* 
 * 240p Test Suite for Nintendo 64
 * Copyright (C)2018 Artemio Urbina
 *
 * This file is part of the 240p Test Suite
 *
 * The 240p Test Suite is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * The 240p Test Suite is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with 240p Test Suite; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA	02111-1307	USA
 */

#include <yaul.h>

#include <stdlib.h>

#include "font.h"
#include "svin.h"

int _fh = 9;
int _fw = 6;

void LoadFont()
{
	int i;
	uint8_t *pFont;
	rgb1555_t * pPal;	

	pPal = (rgb1555_t*)VDP2_CRAM(FONT_WHITE*8);
	*(pPal++) = RGB1555(0, 31, 0, 31); //Back
	*(pPal++) = RGB1555(1, 31, 31, 31); //Font
	*(pPal++) = RGB1555(1, 0, 0, 0); //Shadow

	pPal = (rgb1555_t*)VDP2_CRAM(FONT_RED*8);
	*(pPal++) = RGB1555(1, 31, 0, 31); //Back
	*(pPal++) = RGB1555(1, 31, 0, 0); //Font
	*(pPal++) = RGB1555(1, 0, 0, 0); //Shadow

	pPal = (rgb1555_t*)VDP2_CRAM(FONT_GREEN*8);
	*(pPal++) = RGB1555(1, 31, 0, 31); //Back
	*(pPal++) = RGB1555(1, 0, 31, 0); //Font
	*(pPal++) = RGB1555(1, 0, 0, 0); //Shadow

	pPal = (rgb1555_t*)VDP2_CRAM(FONT_CYAN*8);
	*(pPal++) = RGB1555(1, 31, 0, 31); //Back
	*(pPal++) = RGB1555(1, 0, 31, 31); //Font
	*(pPal++) = RGB1555(1, 0, 0, 0); //Shadow

	pPal = (rgb1555_t*)VDP2_CRAM(FONT_YELLOW*8);
	*(pPal++) = RGB1555(1, 31, 0, 31); //Back
	*(pPal++) = RGB1555(1, 31, 31, 0); //Font
	*(pPal++) = RGB1555(1, 0, 0, 0); //Shadow

	/*color_rgb1555_t dd;
	dd.raw = 0xFFAA;

	pPal[0] = dd;*/

	/*pFont = (uint8_t *)CHAR(0);
	for(i=0; i<(SuiteFont_len); i++)
		*(pFont++) = SuiteFont[i];*/
}

void ClearText(int left, int top, int width, int height)
{
	uint8_t *p8_vram;
	vdp1_vram_partitions_t vdp1_vram_partitions;
    vdp1_vram_partitions_get(&vdp1_vram_partitions);
	
	//drawing in low res for now
	for (int _y = top; ( (_y < FONT_QUAD_HEIGHT) && (_y < top+height) ) ; _y++)
	{
		p8_vram = (uint8_t *)(vdp1_vram_partitions.texture_base + _y*FONT_QUAD_WIDTH);
		for (int _x = left; ( (_x < FONT_QUAD_WIDTH) && (_x < left+width) ) ; _x++)
		{
			p8_vram[_x] = 0;
		}
	}
}

void ClearTextLayer()
{
	ClearText(0,0,FONT_QUAD_WIDTH,FONT_QUAD_HEIGHT);
}

/* Draw a char as VDP1 sprite at desired location*/
void DrawChar(unsigned int x, unsigned int y, char c, unsigned int palette, bool transparent) 
{
	vdp1_vram_partitions_t vdp1_vram_partitions;
    vdp1_vram_partitions_get(&vdp1_vram_partitions);

	//drawing in low res for now
	uint8_t *p8_vram;
	uint8_t *p8_char;
	uint8_t tmp;
	for (int _y = 0; _y < 8; _y++)
	{
		p8_vram = (uint8_t *)(vdp1_vram_partitions.texture_base + x + (_y+y)*FONT_QUAD_WIDTH);
		p8_char =  &(SuiteFont[(c-32)*32+_y*4]);
		for (int _x = 0; _x < 4; _x++)
		{
			tmp = p8_char[_x]&0x3;
			if (tmp) p8_vram[_x*2+1] = tmp | palette*4;
			tmp = (p8_char[_x]>>4)&0x3;
			if (tmp) p8_vram[_x*2] = tmp | palette*4;
		}
	}
}

/* Print a string at x, y using sprites by VDP1*/
void DrawString(char *str, unsigned int x, unsigned int y, unsigned int palette)
{
	int orig_x = x;
	int orig_pal = palette;
	unsigned int i;
	int highlight = 0;

	for(i = 0; i < strlen(str); i++)
	{
		if(str[i] == '\n')
		{
			x = orig_x;
			y += _fh;
			i++;
		}

		if(str[i] == '#')
		{
			highlight = !highlight;
			i++;
			
			if(highlight)
			{
				switch(str[i])
				{
					case 'R':
						palette = FONT_RED;
						break;
					case 'G':
						palette = FONT_GREEN;
						break;
					case 'C':
						palette = FONT_CYAN;
						break;
					case 'Y':
						palette = FONT_YELLOW;
						break;
					case 'W':
						palette = FONT_WHITE;
						break;
				}
			}
			else
				palette = orig_pal;
			i++;
		}

		DrawChar(x, y, str[i], palette, true);
		x += _fw;
	}
}

unsigned char SuiteFont[] = {
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x00, 0x00,
	0x00, 0x01, 0x02, 0x00,
	0x00, 0x01, 0x02, 0x00,
	0x00, 0x01, 0x02, 0x00,
	0x00, 0x00, 0x02, 0x00,
	0x00, 0x01, 0x00, 0x00,
	0x00, 0x00, 0x02, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x10, 0x10, 0x00,
	0x00, 0x10, 0x10, 0x20,
	0x00, 0x10, 0x10, 0x20,
	0x00, 0x00, 0x20, 0x20,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x10, 0x10, 0x00,
	0x01, 0x11, 0x11, 0x20,
	0x00, 0x12, 0x12, 0x22,
	0x01, 0x11, 0x11, 0x20,
	0x00, 0x12, 0x12, 0x22,
	0x00, 0x00, 0x20, 0x20,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x11, 0x10, 0x00,
	0x01, 0x01, 0x22, 0x20,
	0x00, 0x11, 0x12, 0x00,
	0x00, 0x01, 0x21, 0x20,
	0x00, 0x11, 0x12, 0x02,
	0x00, 0x00, 0x22, 0x20,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x10, 0x00, 0x00,
	0x00, 0x10, 0x21, 0x00,
	0x00, 0x00, 0x10, 0x02,
	0x00, 0x01, 0x00, 0x20,
	0x00, 0x10, 0x01, 0x00,
	0x00, 0x00, 0x21, 0x02,
	0x00, 0x00, 0x00, 0x02,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x00, 0x00,
	0x00, 0x10, 0x12, 0x00,
	0x00, 0x01, 0x20, 0x20,
	0x00, 0x10, 0x12, 0x00,
	0x00, 0x01, 0x21, 0x20,
	0x00, 0x00, 0x02, 0x02,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x10, 0x00,
	0x00, 0x00, 0x10, 0x20,
	0x00, 0x00, 0x10, 0x20,
	0x00, 0x00, 0x00, 0x20,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x10, 0x00,
	0x00, 0x01, 0x00, 0x20,
	0x00, 0x01, 0x02, 0x00,
	0x00, 0x01, 0x02, 0x00,
	0x00, 0x01, 0x02, 0x00,
	0x00, 0x00, 0x12, 0x00,
	0x00, 0x00, 0x00, 0x20,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x00, 0x00,
	0x00, 0x00, 0x12, 0x00,
	0x00, 0x00, 0x10, 0x20,
	0x00, 0x00, 0x10, 0x20,
	0x00, 0x00, 0x10, 0x20,
	0x00, 0x01, 0x00, 0x20,
	0x00, 0x00, 0x02, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x10, 0x10, 0x00,
	0x00, 0x01, 0x20, 0x20,
	0x00, 0x11, 0x12, 0x00,
	0x00, 0x01, 0x22, 0x20,
	0x00, 0x10, 0x12, 0x00,
	0x00, 0x00, 0x20, 0x20,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x00, 0x00,
	0x00, 0x01, 0x02, 0x00,
	0x01, 0x11, 0x11, 0x00,
	0x00, 0x01, 0x22, 0x22,
	0x00, 0x01, 0x02, 0x00,
	0x00, 0x00, 0x02, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x10, 0x00,
	0x00, 0x01, 0x02, 0x20,
	0x00, 0x10, 0x02, 0x00,
	0x00, 0x00, 0x20, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x11, 0x11, 0x00,
	0x00, 0x00, 0x22, 0x22,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x10, 0x00,
	0x00, 0x01, 0x12, 0x20,
	0x00, 0x00, 0x02, 0x20,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x01, 0x00,
	0x00, 0x00, 0x10, 0x02,
	0x00, 0x01, 0x00, 0x20,
	0x00, 0x10, 0x02, 0x00,
	0x00, 0x00, 0x20, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x10, 0x00,
	0x00, 0x01, 0x01, 0x20,
	0x00, 0x01, 0x01, 0x02,
	0x00, 0x01, 0x01, 0x02,
	0x00, 0x01, 0x01, 0x02,
	0x00, 0x00, 0x12, 0x02,
	0x00, 0x00, 0x00, 0x20,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x10, 0x00,
	0x00, 0x01, 0x10, 0x20,
	0x00, 0x00, 0x12, 0x20,
	0x00, 0x00, 0x10, 0x20,
	0x00, 0x00, 0x10, 0x20,
	0x00, 0x01, 0x11, 0x20,
	0x00, 0x00, 0x02, 0x22,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x10, 0x00,
	0x00, 0x10, 0x01, 0x20,
	0x00, 0x00, 0x21, 0x02,
	0x00, 0x00, 0x10, 0x02,
	0x00, 0x01, 0x00, 0x20,
	0x00, 0x11, 0x11, 0x00,
	0x00, 0x00, 0x22, 0x22,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x11, 0x11, 0x00,
	0x00, 0x00, 0x21, 0x22,
	0x00, 0x01, 0x10, 0x02,
	0x00, 0x00, 0x01, 0x20,
	0x00, 0x10, 0x01, 0x02,
	0x00, 0x01, 0x10, 0x02,
	0x00, 0x00, 0x02, 0x20,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x10, 0x00,
	0x00, 0x01, 0x10, 0x20,
	0x00, 0x10, 0x12, 0x20,
	0x00, 0x11, 0x11, 0x20,
	0x00, 0x00, 0x12, 0x22,
	0x00, 0x00, 0x10, 0x20,
	0x00, 0x00, 0x00, 0x20,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x11, 0x11, 0x00,
	0x00, 0x10, 0x22, 0x22,
	0x00, 0x11, 0x10, 0x00,
	0x00, 0x00, 0x21, 0x20,
	0x00, 0x10, 0x01, 0x02,
	0x00, 0x01, 0x10, 0x02,
	0x00, 0x00, 0x02, 0x20,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x10, 0x00,
	0x00, 0x10, 0x02, 0x20,
	0x00, 0x11, 0x10, 0x00,
	0x00, 0x10, 0x21, 0x20,
	0x00, 0x10, 0x21, 0x02,
	0x00, 0x01, 0x10, 0x02,
	0x00, 0x00, 0x02, 0x20,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x11, 0x11, 0x00,
	0x00, 0x00, 0x21, 0x22,
	0x00, 0x00, 0x10, 0x02,
	0x00, 0x00, 0x10, 0x20,
	0x00, 0x01, 0x00, 0x20,
	0x00, 0x01, 0x02, 0x00,
	0x00, 0x00, 0x02, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x10, 0x00,
	0x00, 0x10, 0x01, 0x20,
	0x00, 0x01, 0x10, 0x02,
	0x00, 0x10, 0x01, 0x20,
	0x00, 0x10, 0x21, 0x02,
	0x00, 0x01, 0x10, 0x02,
	0x00, 0x00, 0x02, 0x20,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x10, 0x00,
	0x00, 0x10, 0x01, 0x20,
	0x00, 0x10, 0x21, 0x02,
	0x00, 0x01, 0x11, 0x02,
	0x00, 0x00, 0x01, 0x22,
	0x00, 0x01, 0x10, 0x02,
	0x00, 0x00, 0x02, 0x20,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x10, 0x00,
	0x00, 0x01, 0x12, 0x20,
	0x00, 0x00, 0x02, 0x20,
	0x00, 0x01, 0x10, 0x00,
	0x00, 0x01, 0x12, 0x20,
	0x00, 0x00, 0x02, 0x20,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x10, 0x00,
	0x00, 0x01, 0x12, 0x20,
	0x00, 0x00, 0x02, 0x20,
	0x00, 0x01, 0x10, 0x00,
	0x00, 0x01, 0x02, 0x20,
	0x00, 0x10, 0x02, 0x00,
	0x00, 0x00, 0x20, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x01, 0x00,
	0x00, 0x00, 0x10, 0x02,
	0x00, 0x01, 0x00, 0x20,
	0x00, 0x00, 0x12, 0x00,
	0x00, 0x00, 0x01, 0x20,
	0x00, 0x00, 0x00, 0x02,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x11, 0x11, 0x00,
	0x00, 0x00, 0x22, 0x22,
	0x00, 0x11, 0x11, 0x00,
	0x00, 0x00, 0x22, 0x22,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x00, 0x00,
	0x00, 0x00, 0x12, 0x00,
	0x00, 0x00, 0x01, 0x20,
	0x00, 0x00, 0x10, 0x02,
	0x00, 0x01, 0x00, 0x20,
	0x00, 0x00, 0x02, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x10, 0x00,
	0x00, 0x01, 0x01, 0x20,
	0x00, 0x00, 0x01, 0x02,
	0x00, 0x00, 0x10, 0x02,
	0x00, 0x00, 0x00, 0x20,
	0x00, 0x00, 0x10, 0x00,
	0x00, 0x00, 0x00, 0x20,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x10, 0x00,
	0x00, 0x10, 0x01, 0x20,
	0x00, 0x10, 0x11, 0x02,
	0x00, 0x10, 0x11, 0x22,
	0x00, 0x10, 0x20, 0x22,
	0x00, 0x01, 0x10, 0x00,
	0x00, 0x00, 0x02, 0x20,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x10, 0x00,
	0x00, 0x10, 0x01, 0x20,
	0x00, 0x10, 0x21, 0x02,
	0x00, 0x11, 0x11, 0x02,
	0x00, 0x10, 0x21, 0x22,
	0x00, 0x10, 0x21, 0x02,
	0x00, 0x00, 0x20, 0x02,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x11, 0x10, 0x00,
	0x00, 0x10, 0x21, 0x20,
	0x00, 0x11, 0x10, 0x02,
	0x00, 0x10, 0x21, 0x20,
	0x00, 0x10, 0x21, 0x02,
	0x00, 0x11, 0x10, 0x02,
	0x00, 0x00, 0x22, 0x20,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x10, 0x00,
	0x00, 0x10, 0x01, 0x20,
	0x00, 0x10, 0x20, 0x02,
	0x00, 0x10, 0x20, 0x00,
	0x00, 0x10, 0x21, 0x00,
	0x00, 0x01, 0x10, 0x02,
	0x00, 0x00, 0x02, 0x20,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x11, 0x10, 0x00,
	0x00, 0x10, 0x21, 0x20,
	0x00, 0x10, 0x21, 0x02,
	0x00, 0x10, 0x21, 0x02,
	0x00, 0x10, 0x21, 0x02,
	0x00, 0x11, 0x10, 0x02,
	0x00, 0x00, 0x22, 0x20,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x11, 0x11, 0x00,
	0x00, 0x10, 0x22, 0x22,
	0x00, 0x11, 0x10, 0x00,
	0x00, 0x10, 0x22, 0x20,
	0x00, 0x10, 0x20, 0x00,
	0x00, 0x11, 0x11, 0x00,
	0x00, 0x00, 0x22, 0x22,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x11, 0x11, 0x00,
	0x00, 0x10, 0x22, 0x22,
	0x00, 0x11, 0x10, 0x00,
	0x00, 0x10, 0x22, 0x20,
	0x00, 0x10, 0x20, 0x00,
	0x00, 0x10, 0x20, 0x00,
	0x00, 0x00, 0x20, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x10, 0x00,
	0x00, 0x10, 0x01, 0x20,
	0x00, 0x10, 0x20, 0x02,
	0x00, 0x10, 0x11, 0x00,
	0x00, 0x10, 0x21, 0x22,
	0x00, 0x01, 0x11, 0x02,
	0x00, 0x00, 0x02, 0x22,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x10, 0x01, 0x00,
	0x00, 0x10, 0x21, 0x02,
	0x00, 0x11, 0x11, 0x02,
	0x00, 0x10, 0x21, 0x22,
	0x00, 0x10, 0x21, 0x02,
	0x00, 0x10, 0x21, 0x02,
	0x00, 0x00, 0x20, 0x02,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x11, 0x00,
	0x00, 0x00, 0x12, 0x22,
	0x00, 0x00, 0x10, 0x20,
	0x00, 0x00, 0x10, 0x20,
	0x00, 0x00, 0x10, 0x20,
	0x00, 0x01, 0x11, 0x20,
	0x00, 0x00, 0x02, 0x22,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x01, 0x00,
	0x00, 0x00, 0x01, 0x02,
	0x00, 0x00, 0x01, 0x02,
	0x00, 0x00, 0x01, 0x02,
	0x00, 0x10, 0x01, 0x02,
	0x00, 0x01, 0x10, 0x02,
	0x00, 0x00, 0x02, 0x20,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x10, 0x01, 0x00,
	0x00, 0x10, 0x10, 0x02,
	0x00, 0x11, 0x20, 0x20,
	0x00, 0x11, 0x22, 0x00,
	0x00, 0x10, 0x12, 0x00,
	0x00, 0x10, 0x21, 0x20,
	0x00, 0x00, 0x20, 0x02,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x10, 0x00, 0x00,
	0x00, 0x10, 0x20, 0x00,
	0x00, 0x10, 0x20, 0x00,
	0x00, 0x10, 0x20, 0x00,
	0x00, 0x10, 0x20, 0x00,
	0x00, 0x11, 0x11, 0x00,
	0x00, 0x00, 0x22, 0x22,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x10, 0x01, 0x00,
	0x00, 0x11, 0x11, 0x02,
	0x00, 0x11, 0x11, 0x22,
	0x00, 0x10, 0x21, 0x22,
	0x00, 0x10, 0x21, 0x02,
	0x00, 0x10, 0x21, 0x02,
	0x00, 0x00, 0x20, 0x02,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x10, 0x01, 0x00,
	0x00, 0x11, 0x21, 0x02,
	0x00, 0x11, 0x21, 0x02,
	0x00, 0x10, 0x11, 0x02,
	0x00, 0x10, 0x11, 0x22,
	0x00, 0x10, 0x21, 0x22,
	0x00, 0x00, 0x20, 0x02,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x10, 0x00,
	0x00, 0x10, 0x01, 0x20,
	0x00, 0x10, 0x21, 0x02,
	0x00, 0x10, 0x21, 0x02,
	0x00, 0x10, 0x21, 0x02,
	0x00, 0x01, 0x10, 0x02,
	0x00, 0x00, 0x02, 0x20,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x11, 0x10, 0x00,
	0x00, 0x10, 0x21, 0x20,
	0x00, 0x10, 0x21, 0x02,
	0x00, 0x11, 0x10, 0x02,
	0x00, 0x10, 0x22, 0x20,
	0x00, 0x10, 0x20, 0x00,
	0x00, 0x00, 0x20, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x10, 0x00,
	0x00, 0x10, 0x01, 0x20,
	0x00, 0x10, 0x21, 0x02,
	0x00, 0x10, 0x21, 0x02,
	0x00, 0x11, 0x21, 0x02,
	0x00, 0x01, 0x12, 0x02,
	0x00, 0x00, 0x01, 0x20,
	0x00, 0x00, 0x00, 0x02,
	0x00, 0x11, 0x10, 0x00,
	0x00, 0x10, 0x21, 0x20,
	0x00, 0x10, 0x21, 0x02,
	0x00, 0x11, 0x10, 0x02,
	0x00, 0x10, 0x12, 0x20,
	0x00, 0x10, 0x21, 0x20,
	0x00, 0x00, 0x20, 0x02,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x10, 0x00,
	0x00, 0x10, 0x01, 0x20,
	0x00, 0x01, 0x20, 0x02,
	0x00, 0x00, 0x12, 0x00,
	0x00, 0x10, 0x01, 0x20,
	0x00, 0x01, 0x10, 0x02,
	0x00, 0x00, 0x02, 0x20,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x11, 0x00,
	0x00, 0x00, 0x12, 0x22,
	0x00, 0x00, 0x10, 0x20,
	0x00, 0x00, 0x10, 0x20,
	0x00, 0x00, 0x10, 0x20,
	0x00, 0x00, 0x10, 0x20,
	0x00, 0x00, 0x00, 0x20,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x10, 0x01, 0x00,
	0x00, 0x10, 0x21, 0x02,
	0x00, 0x10, 0x21, 0x02,
	0x00, 0x10, 0x21, 0x02,
	0x00, 0x10, 0x21, 0x02,
	0x00, 0x01, 0x10, 0x02,
	0x00, 0x00, 0x02, 0x20,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x10, 0x01, 0x00,
	0x00, 0x10, 0x21, 0x02,
	0x00, 0x10, 0x21, 0x02,
	0x00, 0x10, 0x21, 0x02,
	0x00, 0x01, 0x10, 0x02,
	0x00, 0x01, 0x12, 0x20,
	0x00, 0x00, 0x02, 0x20,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x10, 0x01, 0x00,
	0x00, 0x10, 0x21, 0x02,
	0x00, 0x10, 0x21, 0x02,
	0x00, 0x11, 0x11, 0x02,
	0x00, 0x11, 0x11, 0x22,
	0x00, 0x10, 0x21, 0x22,
	0x00, 0x00, 0x20, 0x02,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x10, 0x01, 0x00,
	0x00, 0x10, 0x21, 0x02,
	0x00, 0x01, 0x10, 0x02,
	0x00, 0x01, 0x12, 0x20,
	0x00, 0x10, 0x01, 0x20,
	0x00, 0x10, 0x21, 0x02,
	0x00, 0x00, 0x20, 0x02,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x02,
	0x00, 0x01, 0x01, 0x02,
	0x00, 0x00, 0x12, 0x02,
	0x00, 0x00, 0x10, 0x20,
	0x00, 0x00, 0x10, 0x20,
	0x00, 0x00, 0x00, 0x20,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x11, 0x11, 0x00,
	0x00, 0x00, 0x21, 0x22,
	0x00, 0x00, 0x10, 0x02,
	0x00, 0x01, 0x00, 0x20,
	0x00, 0x10, 0x02, 0x00,
	0x00, 0x11, 0x11, 0x00,
	0x00, 0x00, 0x22, 0x22,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x11, 0x00,
	0x00, 0x01, 0x02, 0x22,
	0x00, 0x01, 0x02, 0x00,
	0x00, 0x01, 0x02, 0x00,
	0x00, 0x01, 0x02, 0x00,
	0x00, 0x01, 0x11, 0x00,
	0x00, 0x00, 0x02, 0x22,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x10, 0x00, 0x00,
	0x00, 0x01, 0x20, 0x00,
	0x00, 0x00, 0x12, 0x00,
	0x00, 0x00, 0x01, 0x20,
	0x00, 0x00, 0x00, 0x02,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x11, 0x00,
	0x00, 0x00, 0x01, 0x22,
	0x00, 0x00, 0x01, 0x02,
	0x00, 0x00, 0x01, 0x02,
	0x00, 0x00, 0x01, 0x02,
	0x00, 0x01, 0x11, 0x02,
	0x00, 0x00, 0x02, 0x22,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x10, 0x00,
	0x00, 0x01, 0x01, 0x20,
	0x00, 0x00, 0x02, 0x02,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x11, 0x11, 0x00,
	0x00, 0x00, 0x22, 0x22,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x00, 0x00,
	0x00, 0x00, 0x12, 0x00,
	0x00, 0x00, 0x00, 0x20,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x11, 0x00,
	0x00, 0x10, 0x01, 0x22,
	0x00, 0x10, 0x11, 0x02,
	0x00, 0x01, 0x21, 0x22,
	0x00, 0x00, 0x02, 0x02,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x10, 0x00, 0x00,
	0x00, 0x10, 0x20, 0x00,
	0x00, 0x11, 0x10, 0x00,
	0x00, 0x10, 0x21, 0x20,
	0x00, 0x10, 0x21, 0x02,
	0x00, 0x11, 0x10, 0x02,
	0x00, 0x00, 0x22, 0x20,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x10, 0x00,
	0x00, 0x10, 0x02, 0x20,
	0x00, 0x10, 0x20, 0x00,
	0x00, 0x01, 0x10, 0x00,
	0x00, 0x00, 0x02, 0x20,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x01, 0x00,
	0x00, 0x00, 0x01, 0x02,
	0x00, 0x01, 0x11, 0x02,
	0x00, 0x10, 0x01, 0x22,
	0x00, 0x10, 0x21, 0x02,
	0x00, 0x01, 0x11, 0x02,
	0x00, 0x00, 0x02, 0x22,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x10, 0x00,
	0x00, 0x10, 0x11, 0x20,
	0x00, 0x11, 0x20, 0x22,
	0x00, 0x01, 0x12, 0x00,
	0x00, 0x00, 0x02, 0x20,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x10, 0x00,
	0x00, 0x01, 0x01, 0x20,
	0x00, 0x01, 0x02, 0x02,
	0x00, 0x11, 0x12, 0x00,
	0x00, 0x01, 0x22, 0x20,
	0x00, 0x01, 0x02, 0x00,
	0x00, 0x00, 0x02, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x11, 0x00,
	0x00, 0x10, 0x01, 0x22,
	0x00, 0x01, 0x10, 0x02,
	0x00, 0x10, 0x02, 0x20,
	0x00, 0x01, 0x11, 0x00,
	0x00, 0x00, 0x02, 0x22,
	0x00, 0x10, 0x00, 0x00,
	0x00, 0x10, 0x20, 0x00,
	0x00, 0x11, 0x10, 0x00,
	0x00, 0x10, 0x21, 0x20,
	0x00, 0x10, 0x21, 0x02,
	0x00, 0x10, 0x21, 0x02,
	0x00, 0x00, 0x20, 0x02,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x10, 0x00,
	0x00, 0x00, 0x00, 0x20,
	0x00, 0x01, 0x10, 0x00,
	0x00, 0x00, 0x12, 0x20,
	0x00, 0x00, 0x10, 0x20,
	0x00, 0x01, 0x11, 0x20,
	0x00, 0x00, 0x02, 0x22,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x01, 0x00,
	0x00, 0x00, 0x00, 0x02,
	0x00, 0x00, 0x01, 0x00,
	0x00, 0x00, 0x01, 0x02,
	0x00, 0x00, 0x01, 0x02,
	0x00, 0x01, 0x01, 0x02,
	0x00, 0x00, 0x12, 0x02,
	0x00, 0x00, 0x00, 0x20,
	0x00, 0x10, 0x00, 0x00,
	0x00, 0x10, 0x20, 0x00,
	0x00, 0x10, 0x10, 0x00,
	0x00, 0x11, 0x20, 0x20,
	0x00, 0x10, 0x12, 0x00,
	0x00, 0x10, 0x21, 0x20,
	0x00, 0x00, 0x20, 0x02,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x10, 0x00,
	0x00, 0x00, 0x12, 0x20,
	0x00, 0x00, 0x10, 0x20,
	0x00, 0x00, 0x10, 0x20,
	0x00, 0x00, 0x10, 0x20,
	0x00, 0x01, 0x11, 0x20,
	0x00, 0x00, 0x02, 0x22,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x10, 0x10, 0x00,
	0x00, 0x11, 0x11, 0x20,
	0x00, 0x10, 0x21, 0x22,
	0x00, 0x10, 0x21, 0x02,
	0x00, 0x00, 0x20, 0x02,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x11, 0x10, 0x00,
	0x00, 0x10, 0x21, 0x20,
	0x00, 0x10, 0x21, 0x02,
	0x00, 0x10, 0x21, 0x02,
	0x00, 0x00, 0x20, 0x02,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x10, 0x00,
	0x00, 0x10, 0x01, 0x20,
	0x00, 0x10, 0x21, 0x02,
	0x00, 0x01, 0x10, 0x02,
	0x00, 0x00, 0x02, 0x20,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x11, 0x10, 0x00,
	0x00, 0x10, 0x21, 0x20,
	0x00, 0x10, 0x21, 0x02,
	0x00, 0x11, 0x10, 0x02,
	0x00, 0x10, 0x22, 0x20,
	0x00, 0x00, 0x20, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x11, 0x00,
	0x00, 0x10, 0x01, 0x22,
	0x00, 0x10, 0x21, 0x02,
	0x00, 0x01, 0x11, 0x02,
	0x00, 0x00, 0x01, 0x22,
	0x00, 0x00, 0x00, 0x02,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x11, 0x10, 0x00,
	0x00, 0x10, 0x21, 0x20,
	0x00, 0x10, 0x20, 0x02,
	0x00, 0x10, 0x20, 0x00,
	0x00, 0x00, 0x20, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x11, 0x00,
	0x00, 0x11, 0x02, 0x22,
	0x00, 0x00, 0x11, 0x00,
	0x00, 0x11, 0x10, 0x22,
	0x00, 0x00, 0x22, 0x20,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x00, 0x00,
	0x00, 0x01, 0x02, 0x00,
	0x00, 0x11, 0x12, 0x00,
	0x00, 0x01, 0x22, 0x20,
	0x00, 0x01, 0x02, 0x00,
	0x00, 0x00, 0x11, 0x00,
	0x00, 0x00, 0x00, 0x22,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x10, 0x01, 0x00,
	0x00, 0x10, 0x21, 0x02,
	0x00, 0x10, 0x21, 0x02,
	0x00, 0x01, 0x11, 0x02,
	0x00, 0x00, 0x02, 0x22,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x02,
	0x00, 0x01, 0x01, 0x02,
	0x00, 0x00, 0x12, 0x02,
	0x00, 0x00, 0x00, 0x20,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x10, 0x01, 0x00,
	0x00, 0x10, 0x21, 0x02,
	0x00, 0x11, 0x11, 0x02,
	0x00, 0x11, 0x11, 0x22,
	0x00, 0x00, 0x22, 0x22,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x10, 0x01, 0x00,
	0x00, 0x01, 0x10, 0x02,
	0x00, 0x01, 0x12, 0x20,
	0x00, 0x10, 0x01, 0x20,
	0x00, 0x00, 0x20, 0x02,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x10, 0x01, 0x00,
	0x00, 0x10, 0x21, 0x02,
	0x00, 0x01, 0x21, 0x02,
	0x00, 0x00, 0x12, 0x02,
	0x00, 0x01, 0x00, 0x20,
	0x00, 0x00, 0x02, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x11, 0x11, 0x00,
	0x00, 0x00, 0x12, 0x22,
	0x00, 0x01, 0x00, 0x20,
	0x00, 0x11, 0x11, 0x00,
	0x00, 0x00, 0x22, 0x22,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x01, 0x00,
	0x00, 0x00, 0x10, 0x02,
	0x00, 0x01, 0x10, 0x20,
	0x00, 0x00, 0x12, 0x20,
	0x00, 0x00, 0x10, 0x20,
	0x00, 0x00, 0x01, 0x20,
	0x00, 0x00, 0x00, 0x02,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x10, 0x00,
	0x00, 0x00, 0x10, 0x20,
	0x00, 0x00, 0x10, 0x20,
	0x00, 0x00, 0x10, 0x20,
	0x00, 0x00, 0x10, 0x20,
	0x00, 0x00, 0x10, 0x20,
	0x00, 0x00, 0x00, 0x20,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x00, 0x00,
	0x00, 0x00, 0x12, 0x00,
	0x00, 0x00, 0x11, 0x20,
	0x00, 0x00, 0x10, 0x22,
	0x00, 0x00, 0x10, 0x20,
	0x00, 0x01, 0x00, 0x20,
	0x00, 0x00, 0x02, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x01, 0x00,
	0x00, 0x10, 0x12, 0x02,
	0x00, 0x00, 0x20, 0x20,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
};

int SuiteFont_len = 3072;