#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <yaul.h>
#include "font.h"
#include "video_vdp2.h"
#include "video.h"
#include "control.h"
#include "ire.h"
#include "image_big_digits.h"

void get_peripheral_name(smpc_peripheral_t *peripheral, char * name)
{
	switch (peripheral->type)
	{
		case ID_DIGITAL:
			sprintf(name,"%d:Digital pad",peripheral->port);
			break;
		case ID_RACING:
			sprintf(name,"%d:Racing wheel",peripheral->port);
			break;
		case ID_ANALOG:
			sprintf(name,"%d:3D pad",peripheral->port);
			break;
		case ID_MOUSE:
			sprintf(name,"%d:Mouse",peripheral->port);
			break;
		case ID_KEYBOARD:
			sprintf(name,"%d:Keyboard",peripheral->port);
			break;
		case ID_MD3B:
			sprintf(name,"%d:MD 3-pad",peripheral->port);
			break;
		case ID_MD6B:
			sprintf(name,"%d:MD 6-pad",peripheral->port);
			break;
		case ID_MDMOUSE:
			sprintf(name,"%d:MD mouse",peripheral->port);
			break;
		case ID_GUN:
			sprintf(name,"%d:Gun",peripheral->port);
			break;
		default:
			sprintf(name,"%d:Unknown(0x%x)",peripheral->port,peripheral->type);
			break;
	}
}

void get_peripheral_line(smpc_peripheral_t *peripheral, char * string, char * string2)
{
	char buf[32];
	smpc_peripheral_digital_t digital;
	smpc_peripheral_analog_t analog;
	smpc_peripheral_racing_t racing;
	smpc_peripheral_mouse_t mouse;
	smpc_peripheral_keyboard_t keyboard;
	get_peripheral_name(peripheral,buf);
	sprintf(string,"#C%s#W : ",buf);
	string2[0] = 0;
	switch (peripheral->type)
	{
		case ID_DIGITAL:
			smpc_peripheral_digital_get(peripheral,&digital);
			if (digital.pressed.button.a) strcat(string,"#MA#W "); else strcat(string,"A ");
			if (digital.pressed.button.b) strcat(string,"#MB#W "); else strcat(string,"B ");
			if (digital.pressed.button.c) strcat(string,"#MC#W "); else strcat(string,"C ");
			if (digital.pressed.button.x) strcat(string,"#MX#W "); else strcat(string,"X ");
			if (digital.pressed.button.y) strcat(string,"#MY#W "); else strcat(string,"Y ");
			if (digital.pressed.button.z) strcat(string,"#MZ#W "); else strcat(string,"Z ");
			if (digital.pressed.button.l) strcat(string,"#ML#W "); else strcat(string,"L ");
			if (digital.pressed.button.r) strcat(string,"#MR#W "); else strcat(string,"R ");
			if (digital.pressed.button.up) strcat(string,"#MUp#W "); else strcat(string,"Up ");
			if (digital.pressed.button.down) strcat(string,"#MDn#W "); else strcat(string,"Dn ");
			if (digital.pressed.button.left) strcat(string,"#MLt#W "); else strcat(string,"Lt ");
			if (digital.pressed.button.right) strcat(string,"#MRt#W "); else strcat(string,"Rt ");
			if (digital.pressed.button.start) strcat(string,"#MSt#W "); else strcat(string,"St ");
			break;
		case ID_RACING:
			for (int i=0;i<3;i++)
	        	racing.raw[i] = peripheral->data[i];
			if (racing.button.a_trg) strcat(string,"#MA#W "); else strcat(string,"A ");
			if (racing.button.b_trg) strcat(string,"#MB#W "); else strcat(string,"B ");
			if (racing.button.c_trg) strcat(string,"#MC#W "); else strcat(string,"C ");
			if (racing.button.x_trg) strcat(string,"#MX#W "); else strcat(string,"X ");
			if (racing.button.y_trg) strcat(string,"#MY#W "); else strcat(string,"Y ");
			if (racing.button.z_trg) strcat(string,"#MZ#W "); else strcat(string,"Z ");
			if (racing.button.l_trg) strcat(string,"#ML#W "); else strcat(string,"L ");
			if (racing.button.r_trg) strcat(string,"#MR#W "); else strcat(string,"R ");
			if (racing.button.up) strcat(string,"#MUp#W "); else strcat(string,"Up ");
			if (racing.button.down) strcat(string,"#MDn#W "); else strcat(string,"Dn ");
			if (racing.button.left) strcat(string,"#MLt#W "); else strcat(string,"Lt ");
			if (racing.button.right) strcat(string,"#MRt#W "); else strcat(string,"Rt ");
			if (racing.button.start) strcat(string,"#MSt#W "); else strcat(string,"St ");
			sprintf(buf,"W=%3d",racing.button.wheel); strcat(string2,buf);
			break;
		case ID_ANALOG:
			for (int i=0;i<6;i++)
	        	analog.pressed.raw[i] = peripheral->data[i];
			if (analog.pressed.button.a) strcat(string,"#MA#W "); else strcat(string,"A ");
			if (analog.pressed.button.b) strcat(string,"#MB#W "); else strcat(string,"B ");
			if (analog.pressed.button.c) strcat(string,"#MC#W "); else strcat(string,"C ");
			if (analog.pressed.button.x) strcat(string,"#MX#W "); else strcat(string,"X ");
			if (analog.pressed.button.y) strcat(string,"#MY#W "); else strcat(string,"Y ");
			if (analog.pressed.button.z) strcat(string,"#MZ#W "); else strcat(string,"Z ");
			if (analog.pressed.button.l) strcat(string,"#ML#W "); else strcat(string,"L ");
			if (analog.pressed.button.r) strcat(string,"#MR#W "); else strcat(string,"R ");
			if (analog.pressed.button.up) strcat(string,"#MUp#W "); else strcat(string,"Up ");
			if (analog.pressed.button.down) strcat(string,"#MDn#W "); else strcat(string,"Dn ");
			if (analog.pressed.button.left) strcat(string,"#MLt#W "); else strcat(string,"Lt ");
			if (analog.pressed.button.right) strcat(string,"#MRt#W "); else strcat(string,"Rt ");
			if (analog.pressed.button.start) strcat(string,"#MSt#W "); else strcat(string,"St ");
			sprintf(buf,"X = %3d ",analog.pressed.button.axis.x_axis); strcat(string2,buf);
			sprintf(buf,"Y = %3d ",analog.pressed.button.axis.y_axis); strcat(string2,buf);
			sprintf(buf,"L = %3d ",analog.pressed.button.trigger.l); strcat(string2,buf);
			sprintf(buf,"R = %3d ",analog.pressed.button.trigger.r); strcat(string2,buf);
			break;
		case ID_MOUSE:
		case ID_MDMOUSE:
			for (int i=0;i<3;i++)
	        	mouse.raw[i] = peripheral->data[i];
			if (mouse.button.m_btn) strcat(string,"#MM#W "); else strcat(string,"M ");
			if (mouse.button.r_btn) strcat(string,"#MR#W "); else strcat(string,"R ");
			if (mouse.button.l_btn) strcat(string,"#ML#W "); else strcat(string,"L ");
			if (mouse.button.start) strcat(string,"#MSt#W "); else strcat(string,"St ");
			if (mouse.button.x_sgn) strcat(string,"#MX+#W "); else strcat(string,"X- ");
			if (mouse.button.y_sgn) strcat(string,"#MY+#W "); else strcat(string,"Y- ");
			if (mouse.button.x_ovr) strcat(string,"#MXo#W "); else strcat(string,"Xo ");
			if (mouse.button.y_ovr) strcat(string,"#MYo#W "); else strcat(string,"Yo ");
			sprintf(buf,"X = %3d ",mouse.button.x); strcat(string,buf);
			sprintf(buf,"Y = %3d ",mouse.button.y); strcat(string,buf);
			break;
		case ID_KEYBOARD:
			for (int i=0;i<4;i++)
	        	keyboard.raw[i] = peripheral->data[i];
			if (keyboard.button.a_trg) strcat(string,"#MA#W "); else strcat(string,"A ");
			if (keyboard.button.b_trg) strcat(string,"#MB#W "); else strcat(string,"B ");
			if (keyboard.button.c_trg) strcat(string,"#MC#W "); else strcat(string,"C ");
			if (keyboard.button.x_trg) strcat(string,"#MX#W "); else strcat(string,"X ");
			if (keyboard.button.y_trg) strcat(string,"#MY#W "); else strcat(string,"Y ");
			if (keyboard.button.z_trg) strcat(string,"#MZ#W "); else strcat(string,"Z ");
			if (keyboard.button.l_trg) strcat(string,"#ML#W "); else strcat(string,"L ");
			if (keyboard.button.r_trg) strcat(string,"#MR#W "); else strcat(string,"R ");
			if (keyboard.button.up) strcat(string,"#MUp#W "); else strcat(string,"Up ");
			if (keyboard.button.down) strcat(string,"#MDn#W "); else strcat(string,"Dn ");
			if (keyboard.button.left) strcat(string,"#MLt#W "); else strcat(string,"Lt ");
			if (keyboard.button.right) strcat(string,"#MRt#W "); else strcat(string,"Rt ");
			if (keyboard.button.start) strcat(string,"#MSt#W "); else strcat(string,"St ");
			if (keyboard.button.caps_lock) strcat(string2,"Caps "); else strcat(string2,"#MCaps#W ");
			if (keyboard.button.num_lock) strcat(string2,"Num "); else strcat(string2,"#MNum#W ");
			if (keyboard.button.scr_lock) strcat(string2,"Scroll "); else strcat(string2,"#MScroll#W ");
			if (keyboard.button.mak_flag) strcat(string2,"Make "); else strcat(string2,"#MMake#W ");
			if (keyboard.button.brk_flag) strcat(string2,"Break "); else strcat(string2,"#MBreak#W ");
			sprintf(buf,"Code = %2d ",keyboard.button.keycode); strcat(string2,buf);
			break;
		case ID_MD3B:
			smpc_peripheral_digital_get(peripheral,&digital);
			if (digital.pressed.button.a) strcat(string,"#MA#W "); else strcat(string,"A ");
			if (digital.pressed.button.b) strcat(string,"#MB#W "); else strcat(string,"B ");
			if (digital.pressed.button.c) strcat(string,"#MC#W "); else strcat(string,"C ");
			if (digital.pressed.button.up) strcat(string,"#MUp#W "); else strcat(string,"Up ");
			if (digital.pressed.button.down) strcat(string,"#MDn#W "); else strcat(string,"Dn ");
			if (digital.pressed.button.left) strcat(string,"#MLt#W "); else strcat(string,"Lt ");
			if (digital.pressed.button.right) strcat(string,"#MRt#W "); else strcat(string,"Rt ");
			if (digital.pressed.button.start) strcat(string,"#MSt#W "); else strcat(string,"St ");
			break;
		case ID_MD6B:
			smpc_peripheral_digital_get(peripheral,&digital);
			if (digital.pressed.button.a) strcat(string,"#MA#W "); else strcat(string,"A ");
			if (digital.pressed.button.b) strcat(string,"#MB#W "); else strcat(string,"B ");
			if (digital.pressed.button.c) strcat(string,"#MC#W "); else strcat(string,"C ");
			if (digital.pressed.button.x) strcat(string,"#MX#W "); else strcat(string,"X ");
			if (digital.pressed.button.y) strcat(string,"#MY#W "); else strcat(string,"Y ");
			if (digital.pressed.button.z) strcat(string,"#MZ#W "); else strcat(string,"Z ");
			if (digital.pressed.button.r) strcat(string,"#MMode#W "); else strcat(string,"Mode ");
			if (digital.pressed.button.up) strcat(string,"#MUp#W "); else strcat(string,"Up ");
			if (digital.pressed.button.down) strcat(string,"#MDn#W "); else strcat(string,"Dn ");
			if (digital.pressed.button.left) strcat(string,"#MLt#W "); else strcat(string,"Lt ");
			if (digital.pressed.button.right) strcat(string,"#MRt#W "); else strcat(string,"Rt ");
			if (digital.pressed.button.start) strcat(string,"#MSt#W "); else strcat(string,"St ");
			break;
		case ID_GUN:
		default:
			sprintf(string,"Raw = ");
			for (int i=0;i<peripheral->size;i++)
				sprintf(buf,"%2d ",peripheral->data[i]); strcat(string,buf);
			break;
	}
}

void update_controller(video_screen_mode_t screenmode)
{
	int x,y;
	char buf[256];
	char buf2[256];
	smpc_peripheral_t *peripheral;
    smpc_peripheral_t *tmp_peripheral;

	//can't draw all the text each frame, using buffering
	uint8_t * buffer = (uint8_t *)HWRAM(0xD0000);

	ClearTextLayerOnBuffer(buffer);
	y = 10;
	for (int port = 1; port < 3; port++)
	{
		const smpc_peripheral_port_t * const _port = smpc_peripheral_raw_port(port);
		x = 5;
		if (_port->peripheral->size == 0)
		{
			//either multitap or disconnected
			if (_port->peripheral->capacity == 0) 
				sprintf(buf,"Port %d : unconnected",port);
			else
				sprintf(buf,"Port %d : multitap (%d ports)",port,_port->peripheral->capacity); 
		}
		else
		{
			sprintf(buf,"Port %d : direct connection",port);
		}
		DrawStringOnBuffer(buf, x, y, FONT_GREEN,buffer); y+=10;

		//drawing the directly connected peripheral
		if (_port->peripheral->size)
		{
			x = 10;
			_port->peripheral->port = 0;//setting port to 0 for direct connection
			//get_peripheral_name(_port->peripheral,buf);
			//DrawStringOnBuffer(buf, x, y, FONT_YELLOW,buffer);
			x = 10;
			get_peripheral_line(_port->peripheral,buf,buf2);
			DrawStringOnBuffer(buf, x, y, FONT_WHITE,buffer); y+=10;
			
			if (strlen(buf2)>0) {
				x = 30;
				DrawStringOnBuffer(buf2, x, y, FONT_WHITE,buffer); y+=10;
			}
		}
		
		//drawing the list
        for (peripheral = TAILQ_FIRST(&_port->peripherals);
			(peripheral != NULL) && (tmp_peripheral = TAILQ_NEXT(peripheral, peripherals), 1);
            	peripheral = tmp_peripheral) {
					x = 10;
					//get_peripheral_name(peripheral,buf);
					//DrawStringOnBuffer(buf, x, y, FONT_YELLOW,buffer);
					x = 10;
					get_peripheral_line(peripheral,buf,buf2);
					DrawStringOnBuffer(buf, x, y, FONT_WHITE,buffer); y+=10;
					if (strlen(buf2)>0) {
						x = 30;
						DrawStringOnBuffer(buf2, x, y, FONT_WHITE,buffer); y+=10;
					}
				}
	}
	DrawStringOnBuffer("Press Left + Start on controller 1 to Exit", 20, 200, FONT_WHITE,buffer);
	
	//syncing data
	vdp2_tvmd_vblank_out_wait();
	vdp2_tvmd_vblank_in_wait();

	//copying buffer to VDP1 quad
	vdp1_vram_partitions_t vdp1_vram_partitions;
    vdp1_vram_partitions_get(&vdp1_vram_partitions);
	scu_dma_transfer((scu_dma_level_t)0,(uint8_t *)vdp1_vram_partitions.texture_base,buffer,320*224);
	//not waiting for end, because it will certainly be over when we get here again
}


void draw_controller(video_screen_mode_t screenmode)
{
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
}

void hwtest_controller(video_screen_mode_t screenmode)
{
	bool key_pressed = false;
	int results[10];
	int results_number = 0;

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
	
	int offset = 0;
	int offset_dir = 1;
	int mode = 1;
	char buf[64];
	draw_controller(curr_screenmode);
	update_controller(curr_screenmode);
	wait_for_key_unpress();
	vdp1_cmdt_t * cmdlist = (vdp1_cmdt_t*)VDP1_VRAM(0);

	while (1)
	{
		smpc_peripheral_process();
		get_digital_keypress_anywhere(&controller);
		if ( (controller.pressed.button.left) && (controller.pressed.button.start) )
		{
			//quit the pattern
			wait_for_key_unpress();
			update_screen_mode(screenmode,BITMAP_MODE_NONE);
			return;
		}
		else
			key_pressed = false;
		
		//	vdp2_tvmd_vblank_out_wait();
		//vdp2_tvmd_vblank_in_wait();

		update_controller(curr_screenmode);		
	}
}
