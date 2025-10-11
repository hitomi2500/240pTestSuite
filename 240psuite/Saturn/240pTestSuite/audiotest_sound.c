#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <yaul.h>
#include "font.h"
#include "video_vdp2.h"
#include "video.h"
#include "control.h"
#include "audio.h"
#include "audiotest_sound.h"
#include "ponesound.h"

int channel_selection = 0;

void draw_sound(video_screen_mode_t screenmode)
{
	draw_bg_with_expansion(screenmode,false);
	update_sound();
}

void update_sound()
{
	char buf[256];
	//removing text
	ClearTextLayer();
	
	sprintf(buf,"Sound Test");
	DrawString(buf, 160-strlen(buf)*3, 60, FONT_GREEN);

	sprintf(buf,"Center Channel");
	if (0 == channel_selection)
		DrawString(buf, 160-strlen(buf)*3, 115, FONT_RED);
	else
		DrawString(buf, 160-strlen(buf)*3, 115, FONT_WHITE);

	sprintf(buf,"Left Channel");
	if (1 == channel_selection)
		DrawString(buf, 100-strlen(buf)*3, 130, FONT_RED);
	else
		DrawString(buf, 100-strlen(buf)*3, 130, FONT_WHITE);

	sprintf(buf,"Right Channel");
	if (2 == channel_selection)
		DrawString(buf, 220-strlen(buf)*3, 130, FONT_RED);
	else
		DrawString(buf, 220-strlen(buf)*3, 130, FONT_WHITE);
}

void audiotest_sound(video_screen_mode_t screenmode)
{
	//using 240p screenmode, no switching supported
	video_screen_mode_t curr_screenmode =
	{
		.scanmode = VIDEO_SCANMODE_240P,
		.x_res = VIDEO_X_RESOLUTION_320,
		.y_res = VDP2_TVMD_VERT_240,
		.x_res_doubled = false,
		.colorsystem = VDP2_TVMD_TV_STANDARD_NTSC,
	};
	update_screen_mode(curr_screenmode,BITMAP_MODE_NONE);
	
	draw_sound(curr_screenmode);
	wait_for_key_unpress();

	while (1)
	{
		smpc_peripheral_process();
		get_digital_keypress_anywhere(&controller);
		if ( (controller.pressed.button.a) || (controller.pressed.button.c))
		{
			//play the sound on the requested channel
			ponesound_pcm_play(PCM_NUMBER_TONES, PCM_PROTECTED, 6);
			switch (channel_selection)
			{
			case 0: 
				ponesound_pcm_parameter_change(PCM_NUMBER_TONES, 15, 0);
				break;
			case 1: 
				ponesound_pcm_parameter_change(PCM_NUMBER_TONES, 15, 31);
				break;
			case 2: 
				ponesound_pcm_parameter_change(PCM_NUMBER_TONES, 15, 15);
				break;
			}
			wait_for_key_unpress();
		}
		else if ( (controller.pressed.button.b) )
		{
			//quit the pattern
			wait_for_key_unpress();
			update_screen_mode(screenmode,BITMAP_MODE_NONE);
			return;
		}
		else if ( (controller.pressed.button.left) )
		{
			if (0 == channel_selection)
				channel_selection = 1;
			else if (2 == channel_selection)
				channel_selection = 0;
			update_sound();
			wait_for_key_unpress();
		}
		else if ( (controller.pressed.button.right) )
		{
			if (0 == channel_selection)
				channel_selection = 2;
			else if (1 == channel_selection)
				channel_selection = 0;
			update_sound();
			wait_for_key_unpress();
		}
		vdp2_tvmd_vblank_out_wait();
		vdp2_tvmd_vblank_in_wait();
	}
}
