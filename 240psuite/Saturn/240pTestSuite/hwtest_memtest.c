#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <yaul.h>
#include "font.h"
#include "video_vdp2.h"
#include "video.h"
#include "control.h"
#include "ire.h"
#include "ponesound.h"

extern uint8_t asset_sound_driver[];
extern uint8_t asset_sound_driver_end[];
static int prev_mem = 0;
static int statuses[8] = {0};
static int statuses_prev[8] = {0};

#define MEMS_COUNT 7

char * memtest_get_name_by_id (int mem_id){
	switch (mem_id) {
		case 0: 
			return "1024K HWRAM ";
			break; 
		case 1: 
			return "1024K LWRAM ";
			break; 
		case 2: 
			return " 512K Sound RAM ";
			break; 
		case 3: 
			return " 512K VDP1 VRAM ";
			break; 
		case 4: 
			return " 512K VDP1 Framebuffers ";
			break; 
		case 5: 
			return " 512K VDP2 VRAM ";
			break; 
		case 7: 
			return "  32K Backup RAM ";
			break; 
		case 6: 
			return "   4K VDP2 Color RAM ";
			break; 
		case 8: 
			return " 512K CD Block RAM ";
			break; 
			}
	return " UNKNOWN RAM ";
}

int memtest_get_size_by_id (int mem_id){
	switch (mem_id) {
		case 0: 
		case 1: 
			return 1024*1024;
			break; 
		case 2: 
		case 3: 
		case 4: 
		case 5: 
		case 8: 
			return 512*1024;
			break;
		case 7: 
			return 32*1024;
			break; 
		case 6: 
			return 4*1024;
			break; 
	}
	return 0;
}

int memtest_get_address_by_id (int mem_id){
	//using uncached accesses for all memories
	switch (mem_id) {
		case 0: 
			return HWRAM_UNCACHED(0);
			break;
		case 1: 
			return LWRAM_UNCACHED(0);
			break; 
		case 2: 
			return SCSP_RAM(0);
			break; 
		case 3: 
			return VDP1_VRAM(0);
			break;
		case 4: 
			return VDP1_FB(0);
			break;
		case 5: 
			return VDP2_VRAM(0);
			break;
		case 6: 
			return VDP2_CRAM(0);
			break;
		case 7: 
			return 0x20180000; //backup ram
			break; 
		case 8: 
			return 0; //TODO
			break; 
	}
	return 0;
}

char * memtest_get_status_string (int status){
	switch (status) {
		case 0: 
			return " Untested";
			break; 
		case -1: 
			return " Testing";
			break; 
		case -2:
			return " OK";
			break; 
	}
	return " Errors";
}

void draw_memtest(video_screen_mode_t screenmode, int current_mem)
{
	char buf[256];
	prev_mem = 0;
	for (int i=0;i<8;i++) statuses_prev[i] = 0;
	//removing text
	ClearTextLayer();
	
	//add colors to palette
	uint8_t IRE_top = Get_IRE_Level(100.0);
	uint8_t IRE_bot = Get_IRE_Level(7.5);
	rgb888_t Color = {0,0,0,0};
	Color.r = IRE_top;
	Color.g = IRE_top;
	Color.b = IRE_top;	
	video_vdp2_set_palette_part(2,&Color,1,1); //palette 2 color 1 = IRE white
	Color.r = IRE_bot + (IRE_top-IRE_bot)/5;
	Color.g = IRE_bot + (IRE_top-IRE_bot)/5;
	Color.b = IRE_top;
	video_vdp2_set_palette_part(2,&Color,2,2); //palette 2 color 2 = IRE lightblue

	video_vdp2_set_cycle_patterns_cpu();
	//create single-color tile for the color
	int *_pointer32 = (int *)VIDEO_VDP2_NBG0_CHPNDR_START;
	for (int j=0; j<16; j++)
		_pointer32[16+j] = 0x02020202;
	//fill everything with our tile
    _pointer32 = (int *)VIDEO_VDP2_NBG0_PNDR_START;
    for (unsigned int i = 0; i < VIDEO_VDP2_NBG0_PNDR_SIZE / sizeof(int); i++)
    {
        _pointer32[i] = 0x00200002; //palette 2, transparency on, black
    }
	video_vdp2_set_cycle_patterns_nbg(screenmode);

	DrawString("Memory test", 160 - _fw*strlen("Memory test")/2, 20, FONT_WHITE);

	for (int pos = 0; pos<MEMS_COUNT; pos++) {
		strcpy(buf,memtest_get_name_by_id(pos));
		while (strlen(buf) < 38)
			strcat(buf,".");
		strcat(buf,memtest_get_status_string(0));
		DrawString(buf, 20, 50+_fh*pos, current_mem == pos ? FONT_RED : FONT_WHITE);
	}

	sprintf(buf,"Press A to Run selected memory test");
	DrawString(buf, 30, 160, FONT_WHITE);
	//sprintf(buf,"Press X to Run all safe tests");
	//DrawString(buf, 30, 160+_fh, FONT_WHITE);
	//sprintf(buf,"Press Z to Run all tests");
	//DrawString(buf, 30, 160+_fh*1, FONT_WHITE);
	sprintf(buf,"Press B to Exit");
	DrawString(buf, 30, 160+_fh*1, FONT_WHITE);
	sprintf(buf,"Note : framebuffers test doesn't show any");
	DrawString(buf, 30, 160+_fh*3, FONT_WHITE);
	sprintf(buf,"progress and takes ~10 minutes");
	DrawString(buf, 30, 160+_fh*4, FONT_WHITE);
}

void redraw_memtest(video_screen_mode_t screenmode, int current_mem, int statuses[8], int offsets[8])
{
	char buf[256];
	char buf2[16];

	strcpy(buf,memtest_get_name_by_id(prev_mem));
	while (strlen(buf) <38)
		strcat(buf,".");

	ClearText(20+strlen(buf)*_fw, 50+_fh*prev_mem, 10*_fw, _fh);
	if (statuses[prev_mem] > 0) {
		sprintf(buf2," %d errors",statuses[prev_mem]);
		strcat(buf,buf2);
	} else if (statuses[prev_mem] == -1) {
		sprintf(buf2," %d",offsets[prev_mem]*100/memtest_get_size_by_id(prev_mem));
		strcat(buf,buf2);
		strcat(buf,"\%");
	}
	else
		strcat(buf,memtest_get_status_string(statuses[prev_mem]));
	DrawString(buf, 20, 50+_fh*prev_mem, FONT_WHITE);
	strcpy(buf,memtest_get_name_by_id(current_mem));
	while (strlen(buf) <38)
		strcat(buf,".");
	ClearText(20+strlen(buf)*_fw, 50+_fh*current_mem, 10*_fw, _fh);
	if (statuses[current_mem] > 0) {
		sprintf(buf2," %d errors",statuses[current_mem]);
		strcat(buf,buf2);
	} else if (statuses[current_mem] == -1) {
		sprintf(buf2," %d",offsets[current_mem]*100/memtest_get_size_by_id(current_mem));
		strcat(buf,buf2);
		strcat(buf,"\%");
	}
	else
		strcat(buf,memtest_get_status_string(statuses[current_mem]));
	DrawString(buf, 20, 50+_fh*current_mem, FONT_RED);

	redraw_statuses_memtest(screenmode, current_mem, statuses, offsets);

	prev_mem = current_mem;
}

void redraw_statuses_memtest(video_screen_mode_t screenmode, int current_mem, int statuses[8], int offsets[8])
{
	char buf[256];
	char buf2[16];

	//redraw for every mem that is not prev and current, but in progress
	for (int i=0;i<MEMS_COUNT;i++)  {
		if ( (statuses_prev[i] != statuses[i]) || (statuses[i] == -1) ){
			for (int k=0;k<38;k++) buf[k] = ' ';
			buf[38] = '\0';
			ClearText(20+strlen(buf)*_fw, 50+_fh*i, 10*_fw, _fh);
			if (statuses[i] > 0) {
				sprintf(buf2," %d errors",statuses[i]);
				strcat(buf,buf2);
			} else if (statuses[i] == -1) {
				sprintf(buf2," %d",offsets[i]*100/memtest_get_size_by_id(i));
				strcat(buf,buf2);
				strcat(buf,"\%");
			}
			else
				strcat(buf,memtest_get_status_string(statuses[i]));
			DrawString(buf, 20, 50+_fh*i, (i == current_mem) ? FONT_RED : FONT_WHITE);
		}
		statuses_prev[i] = statuses[i];
	}
}

void hwtest_memtest(video_screen_mode_t screenmode)
{
	bool key_pressed = false;
	int current_mem = 0;
	bool redraw = false;
	int offsets[8] = {0};
	int errors[8] = {0};

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
	
	draw_memtest(curr_screenmode,current_mem);
	wait_for_key_unpress();
	vdp1_cmdt_t * cmdlist = (vdp1_cmdt_t*)VDP1_VRAM(0);

	while (1)
	{
		smpc_peripheral_process();
		get_digital_keypress_anywhere(&controller);
		if ( (controller.pressed.button.b) )
		{
			//quit the pattern
			wait_for_key_unpress();
			update_screen_mode(screenmode,BITMAP_MODE_NONE);
			return;
		}
		else if	( (controller.pressed.button.up) )
		{
			current_mem--;
			if (current_mem < 0) current_mem = MEMS_COUNT-1;
			wait_for_key_unpress();
			redraw = true;
		}
		else if	( (controller.pressed.button.down) )
		{
			current_mem++;
			if (current_mem >= MEMS_COUNT) current_mem = 0;
			wait_for_key_unpress();
			redraw = true;
			
		}
		else if	( (controller.pressed.button.a) || (controller.pressed.button.c) )
		{
			statuses[current_mem] = -1;
			offsets[current_mem] = 0;
			wait_for_key_unpress();
			redraw = true;
		}
		/*else if	( (controller.pressed.button.z) )
		{
			for (int i=0;i <MEMS_COUNT; i++) {
				statuses[i] = -1;
				offsets[i] = 0;
			}
			wait_for_key_unpress();
			redraw = true;
		}*/
		else
			key_pressed = false;
		//testing logic
		for (int mem=0; mem<MEMS_COUNT; mem++)
		{
			int test_granularity = 1024;
			if (4==mem) test_granularity = 32;
			if (statuses[mem] == -1) {
				//if it's a start, reset error counter to 0 and do the lines test first
				if (0 == offsets[mem]) {
					//for sound ram stop the sound CPU at the start
					if ( mem == 2) {
						smpc_smc_wait(0);
						smpc_smc_call(SMPC_SMC_SNDOFF);
						errors[mem]= memtest_test_lines(memtest_get_address_by_id(mem),memtest_get_size_by_id(mem));
					}
					//for vdp1 framebufers, mark current buffer as "first"
					else if ( mem == 4) {
						volatile vdp1_ioregs_t * const _regs = (volatile vdp1_ioregs_t *)VDP1_IOREG_BASE;
						_regs->ptmr = 0;//VDP1_PTMR_IDLE;
						volatile uint16_t *p16 = (uint16_t*)(VDP1_FB(0x3FFFE));
						p16[0] = 0xAA55;
						while ( p16[0] == 0xAA55 );
						errors[mem] = memtest_test_lines(memtest_get_address_by_id(mem),memtest_get_size_by_id(mem)/2);
						p16[0] = 0x3333;
						while ( p16[0] == 0x3333 );
						errors[mem] += memtest_test_lines(memtest_get_address_by_id(mem),memtest_get_size_by_id(mem)/2);
					}
					else
						errors[mem]= memtest_test_lines(memtest_get_address_by_id(mem),memtest_get_size_by_id(mem));
				}
				//testing with 1kB chunks in a single step
				errors[mem] += memtest_test_area(memtest_get_address_by_id(mem)+offsets[mem],test_granularity);
				offsets[mem] += test_granularity;
				if (offsets[mem] >= memtest_get_size_by_id(mem)) {
					//test is over
					//for sound ram restart the sound CPU at the end (doesn't work)
					if ( mem == 2)
						ponesound_load_driver(asset_sound_driver, asset_sound_driver_end - asset_sound_driver,ADX_MASTER_768);//load driver binary to SMPC
					//for vdp1 fb restore drawing
					if ( mem == 4) {
						volatile vdp1_ioregs_t * const _regs = (volatile vdp1_ioregs_t *)VDP1_IOREG_BASE;
						_regs->ptmr = 2;//VDP1_PTMR_AUTO;
					}
					if (errors[mem])
						statuses[mem] = errors[mem];
					else
						statuses[mem] = -2;
					redraw = true;
				}
			}
		}

		vdp2_tvmd_vblank_out_wait();
		//redraw logic
		if (redraw) {
			redraw_memtest(curr_screenmode,current_mem,statuses,offsets);
			redraw = false;
		} else {
			redraw_statuses_memtest(curr_screenmode,current_mem,statuses,offsets);
		}
		vdp2_tvmd_vblank_in_wait();
		
	}
}
