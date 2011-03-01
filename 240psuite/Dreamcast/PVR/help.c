/* 
 * 240p Test Suite
 * Copyright (C)2011 Artemio Urbina
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

#include <kos.h>
#include <stdio.h>

#include "controller.h"
#include "image.h"
#include "font.h"
#include "vmodes.h"
#include "vmu.h"

#include "help.h"

#define LINESPERPAGE	16

extern ImagePtr		scanlines;

static inline void DrawScanlines()
{
	if(vmode == FAKE_640_SL && scanlines)
		DrawImage(scanlines);
}

char *LoadHelpFile(char *filename, char ***pages, int *npages)
{
	int		i = 0, lines = 0, linecount = 0, currpage = 0;
	long 	size = 0;
	FILE 	*fp = NULL;
	char 	*buffer = NULL;

	fp = fopen(filename, "r");
	if(!fp)
	{
		fprintf(stderr, "Could not load %s help file\n", filename);
		return NULL;
	}
	fseek(fp, 0L, SEEK_END);
	size = ftell(fp)+1;
	buffer = (char*)malloc(sizeof(char)*size);
	if(!buffer)
	{
		fclose(fp);
		fprintf(stderr, "Could not load %s help file to RAM\n", filename);
		return NULL;
	}
	fseek(fp, 0L, SEEK_SET);
	memset(buffer, 0x0, sizeof(char)*size);
	fread(buffer, sizeof(char), size-1, fp);
	fclose(fp);

	for(i = 0; i < size; i++)
		if(buffer[i] == '\n')
			lines ++;

	*npages = (lines / LINESPERPAGE);
	if(lines % LINESPERPAGE && lines > LINESPERPAGE)
		(*npages) ++;
	if(*npages < 1)
		*npages = 1;

	*pages = (char**)malloc(sizeof(char*)*(*npages));
	if(!*pages)
	{
		fprintf(stderr, "Could not malloc array of pages %s\n", filename);
		free(buffer);
		buffer = NULL;
		return NULL;
	}

	*pages[0] = buffer;
	currpage = 1;
	if(*npages > 1)
	{
		for(i = 0; i < size; i++)
		{
			if(buffer[i] == '\n')
			{
				linecount ++;
				if(linecount == LINESPERPAGE)
				{
					linecount = 0;
					buffer[i] = 0x0;
					if(currpage < *npages)
						(*pages)[currpage++] = buffer+i+1;
				}
			}
		}
	}

	return buffer;
}

uint16 HelpWindow(char *filename, ImagePtr screen)
{
	int 				done = 0, npages = 0, page = 0;
	uint16			oldbuttons, pressed = 0;		
	ImagePtr		back;
	char				*buffer = NULL, **pages = NULL;
	controller	*st;

	oldbuttons = InitController(0);
	buffer = LoadHelpFile(filename, &pages, &npages);
	if(!buffer)
		return 1;

	back = LoadImage("/rd/help.png", 1);
	back->alpha = 0.75f;
		
	updateVMU(" 	Help	", "", 1);
	while(!done) 
	{
		pvr_wait_ready();

		st = ReadController(0);
		if(st)
		{
			pressed = st->buttons & ~oldbuttons;
			oldbuttons = st->buttons;
					
			if (pressed & CONT_START)
				done =	1;								
			if (pressed & CONT_DPAD_LEFT)
				page --;
			if (pressed & CONT_DPAD_RIGHT)
				page ++;
			if (st->rtrig > 5)
				page ++;
			if (st->ltrig > 5)
				page --;
		}

		if(page > npages - 1)
			page = npages - 1;

		if(page < 0)
			page = 0;

		pvr_scene_begin();

		pvr_list_begin(PVR_LIST_TR_POLY);
		if(screen)
			DrawImage(screen);
		DrawImage(back);
		DrawStringS(34, 42, 1.0f, 1.0f, 1.0f, pages[page]); 

		DrawStringS(100, 200, 0.9f, 0.9f, 0.9f, "Press START to return"); 
		DrawScanlines();
		pvr_list_finish();				

		pvr_scene_finish();
	}
	if(buffer)
		free(buffer);
	if(pages)
		free(pages);
	FreeImage(&back);
	return pressed;
}


