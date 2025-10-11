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

int position = 0;
int movement = 1;

void draw_audiosync(video_screen_mode_t screenmode)
{
	//removing text
	ClearTextLayer();
	
	video_vdp2_set_cycle_patterns_cpu();
	//adding a single color to palette
	uint8_t IRE_top = Get_IRE_Level(100.0);
	uint8_t IRE_bot = Get_IRE_Level(7.5);
	rgb888_t Color = {.cc=0,.r=IRE_bot,.g=IRE_bot,.b=IRE_bot};
	Color.r = IRE_bot;
	Color.g = IRE_bot;
	Color.b = IRE_bot;	
	video_vdp2_set_palette_part(2,&Color,1,1); //palette 2 color 1
	//create single-color tile for the color
	int copies = 1;
	int *_pointer32;
	int vram_offset;
	_pointer32 = (int *)VIDEO_VDP2_NBG0_CHPNDR_START;
	for (int j=0; j<16; j++)
		_pointer32[16+j] = 0x01010101;
	//fill everything with our tile
	_pointer32 = (int *)VIDEO_VDP2_NBG0_PNDR_START;
	vram_offset = VIDEO_VDP2_NBG0_CHPNDR_START - VDP2_VRAM_ADDR(0,0);
   	for (unsigned int i = 0; i < VIDEO_VDP2_NBG0_PNDR_SIZE / sizeof(int); i++)
		_pointer32[i] = 0x00200002 + vram_offset/32; //palette 2, transparency on, black
	video_vdp2_set_cycle_patterns_nbg(screenmode);

	//generating VDP1 tile, 8x1 white
	vdp1_vram_partitions_t vdp1_vram_partitions;
    vdp1_vram_partitions_get(&vdp1_vram_partitions);
	uint8_t * p8 = (uint8_t*)vdp1_vram_partitions.texture_base+0x12000;
	uint8_t color = 1 | FONT_WHITE*4;
	for (int x=0;x<8;x++)
		p8[x] = color;

	vdp1_cmdt_t * cmdlist = (vdp1_cmdt_t*)VDP1_VRAM(0);
	vdp1_cmdt_draw_mode_t sprite_draw_mode = {.raw = 0x0000, .pre_clipping_disable = true};
    vdp1_cmdt_color_bank_t font_color_bank = {.raw = 0};
	//for text layer, decreasing the size to 8x1 to minimize overdraw
	cmdlist[2].cmd_size=((8/8)<<8)|(1);
	//leaving command 3 as-is
	//adding commands for moving horizontal bars
	for (int i=0;i<2;i++)
	{
		memset(&(cmdlist[i+4]),0,sizeof(vdp1_cmdt_t));
	    vdp1_cmdt_scaled_sprite_set(&(cmdlist[i+4]));
		vdp1_cmdt_draw_mode_set(&(cmdlist[i+4]), sprite_draw_mode);
		vdp1_cmdt_color_mode4_set(&(cmdlist[i+4]),font_color_bank);//8bpp
		cmdlist[i+4].cmd_xa = 0+i*160;
    	cmdlist[i+4].cmd_ya = 70;
		cmdlist[i+4].cmd_xc = 160+i*160;
    	cmdlist[i+4].cmd_yc = 85;
		vdp1_cmdt_char_base_set(&(cmdlist[i+4]),vdp1_vram_partitions.texture_base+0x12000);
		cmdlist[i+4].cmd_size=((8/8)<<8)|(1);
	}
	//fixed horizonal bar
	memset(&(cmdlist[6]),0,sizeof(vdp1_cmdt_t));
	vdp1_cmdt_scaled_sprite_set(&(cmdlist[6]));
	vdp1_cmdt_draw_mode_set(&(cmdlist[6]), sprite_draw_mode);
	vdp1_cmdt_color_mode4_set(&(cmdlist[6]),font_color_bank);//8bpp
	cmdlist[6].cmd_xa = 0;
    cmdlist[6].cmd_ya = 176;
	cmdlist[6].cmd_xc = 320;
    cmdlist[6].cmd_yc = 182;
	vdp1_cmdt_char_base_set(&(cmdlist[6]),vdp1_vram_partitions.texture_base+0x12000);
	cmdlist[6].cmd_size=((8/8)<<8)|(1);

	//moving quad
	memset(&(cmdlist[7]),0,sizeof(vdp1_cmdt_t));
	vdp1_cmdt_scaled_sprite_set(&(cmdlist[7]));
	vdp1_cmdt_draw_mode_set(&(cmdlist[7]), sprite_draw_mode);
	vdp1_cmdt_color_mode4_set(&(cmdlist[7]),font_color_bank);//8bpp
	cmdlist[7].cmd_xa = 157;
    cmdlist[7].cmd_ya = 140;
	cmdlist[7].cmd_xc = 163;
    cmdlist[7].cmd_yc = 146;
	vdp1_cmdt_char_base_set(&(cmdlist[7]),vdp1_vram_partitions.texture_base+0x12000);
	cmdlist[7].cmd_size=((8/8)<<8)|(1);

	//blinking (entire screen)
	memset(&(cmdlist[8]),0,sizeof(vdp1_cmdt_t));
	vdp1_cmdt_scaled_sprite_set(&(cmdlist[8]));
	vdp1_cmdt_draw_mode_set(&(cmdlist[8]), sprite_draw_mode);
	vdp1_cmdt_color_mode4_set(&(cmdlist[8]),font_color_bank);//8bpp
	cmdlist[8].cmd_xa = 320;
    cmdlist[8].cmd_ya = 240;
	cmdlist[8].cmd_xc = 320;
    cmdlist[8].cmd_yc = 240;
	vdp1_cmdt_char_base_set(&(cmdlist[8]),vdp1_vram_partitions.texture_base+0x12000);
	cmdlist[8].cmd_size=((8/8)<<8)|(1);

	//end of commands list
    vdp1_cmdt_end_set(&(cmdlist[9]));
}

void update_audiosync()
{
		int position_fix = (position < 60) ? position : 120-position;
		vdp1_cmdt_t * cmdlist = (vdp1_cmdt_t*)VDP1_VRAM(0);
		//moving horizontal bars
		cmdlist[4].cmd_xc = 160 - position_fix*2;
		cmdlist[5].cmd_xa = 160 + position_fix*2;
		//moving quad
    	cmdlist[7].cmd_ya = 170 - position_fix;
    	cmdlist[7].cmd_yc = 176 - position_fix;
		if (position == 120) {
    		cmdlist[8].cmd_xa = 0;
    		cmdlist[8].cmd_ya = 0;
		} else {
    		cmdlist[8].cmd_xa = 320;
    		cmdlist[8].cmd_ya = 240;
		}
}

void audiotest_sync(video_screen_mode_t screenmode)
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
	
	draw_audiosync(curr_screenmode);
	wait_for_key_unpress();

	while (1)
	{
		smpc_peripheral_process();
		get_digital_keypress_anywhere(&controller);
		if ( (controller.pressed.button.a) || (controller.pressed.button.c) )
		{
			wait_for_key_unpress();
			movement = (movement) ? 0: 1;
		}
		if ( (controller.pressed.button.b) )
		{
			//quit the pattern
			wait_for_key_unpress();
			update_screen_mode(screenmode,BITMAP_MODE_NONE);
			return;
		}
		vdp2_tvmd_vblank_out_wait();
		vdp2_tvmd_vblank_in_wait();
		if (movement) 
			position++;
		if (position > 120)
			position = 1;
		update_audiosync();
		if (position == 120) {
			ponesound_pcm_play(PCM_NUMBER_BEEP, PCM_PROTECTED, 6);
			ponesound_pcm_parameter_change(PCM_NUMBER_BEEP, 15, 0);
		}
	}
}
