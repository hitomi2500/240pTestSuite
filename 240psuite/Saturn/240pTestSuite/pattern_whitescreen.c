#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <yaul.h>
#include "font.h"
#include "video_vdp2.h"
#include "video.h"
#include "control.h"
#include "ire.h"

void draw_whitescreen(video_screen_mode_t screenmode, int color)
{
	//removing text
	ClearTextLayer();
	
	video_vdp2_set_cycle_patterns_cpu();
	//adding a single color to palette
	uint8_t IRE_top = Get_IRE_Level(100.0);
	uint8_t IRE_bot = Get_IRE_Level(7.5);
	rgb888_t Color = {0,IRE_bot,IRE_bot,IRE_bot};
	switch (color)
	{
		case 0: //white
			Color.r = IRE_top;
			Color.g = IRE_top;
			Color.b = IRE_top;	
			break;
		case 1: //black
			Color.r = IRE_bot;
			Color.g = IRE_bot;
			Color.b = IRE_bot;	
			break;
		case 2: //red
			Color.r = IRE_top;
			Color.g = IRE_bot;
			Color.b = IRE_bot;	
			break;
		case 3: //green
			Color.r = IRE_bot;
			Color.g = IRE_top;
			Color.b = IRE_bot;	
			break;
		case 4: //blue
			Color.r = IRE_bot;
			Color.g = IRE_bot;
			Color.b = IRE_top;	
			break;
	}
	video_vdp2_set_palette_part(2,&Color,1,1); //palette 2 color 1
	//create single-color tile for the color
	int *_pointer32 = (int *)VIDEO_VDP2_NBG0_CHPNDR_START;
	for (int j=0; j<16; j++)
		_pointer32[16+j] = 0x01010101;
	//fill everything with our tile
    _pointer32 = (int *)VIDEO_VDP2_NBG0_PNDR_START;
    for (unsigned int i = 0; i < VIDEO_VDP2_NBG0_PNDR_SIZE / sizeof(int); i++)
    {
        _pointer32[i] = 0x00200002; //palette 2, transparency on, black
    }
	video_vdp2_set_cycle_patterns_nbg(screenmode);
}

void pattern_whitescreen(video_screen_mode_t screenmode)
{
	video_screen_mode_t curr_screenmode = screenmode;
	int color = 0;
	draw_whitescreen(curr_screenmode,color);
	bool key_pressed = false;

	wait_for_key_unpress();
	
	int mode_display_counter = 0;

	while (1)
	{
		smpc_peripheral_process();
		get_digital_keypress_anywhere(&controller);
		if ( (controller.pressed.button.l) )
		{
			curr_screenmode = prev_screen_mode(curr_screenmode);
			update_screen_mode(curr_screenmode,false);
			draw_whitescreen(curr_screenmode,color);
			print_screen_mode(curr_screenmode);
			wait_for_key_unpress();
			mode_display_counter=120;
		}
		else if ( (controller.pressed.button.r) )
		{
			curr_screenmode = next_screen_mode(curr_screenmode);
			update_screen_mode(curr_screenmode,false);
			draw_whitescreen(curr_screenmode,color);
			print_screen_mode(curr_screenmode);
			wait_for_key_unpress();
			mode_display_counter=120;
		}
		else if ( (controller.pressed.button.a) || (controller.pressed.button.c) )
		{
			//change the checkered mode
			color++;
			if (color > 4) color = 0;
			draw_whitescreen(curr_screenmode,color);
			wait_for_key_unpress();
		}
		else if (controller.pressed.button.b)
		{
			//quit the pattern
			wait_for_key_unpress();
			update_screen_mode(screenmode,false);
			return;
		}
		vdp2_tvmd_vblank_in_wait();
		vdp2_tvmd_vblank_out_wait();
		if (mode_display_counter > 0)
		{
			mode_display_counter--;
			if (0 == mode_display_counter)
			{
				ClearTextLayer();
			}
		}
	}
}
