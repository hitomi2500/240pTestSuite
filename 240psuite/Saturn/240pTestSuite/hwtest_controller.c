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
			sprintf(name,"%d:Keyboard(J)",peripheral->port);
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
	}
}

void get_peripheral_state(smpc_peripheral_t *peripheral, char * string)
{
	smpc_peripheral_digital_t digital;
	sprintf(string,"");
	switch (peripheral->type)
	{
		case ID_DIGITAL:
			smpc_peripheral_digital_get(peripheral,&digital);
			if (digital.pressed.button.a) strcat(string,"#GA#W "); else strcat(string,"A ");
			if (digital.pressed.button.b) strcat(string,"#GB#W "); else strcat(string,"B ");
			if (digital.pressed.button.c) strcat(string,"#GC#W "); else strcat(string,"C ");
			if (digital.pressed.button.x) strcat(string,"#GX#W "); else strcat(string,"X ");
			if (digital.pressed.button.y) strcat(string,"#GY#W "); else strcat(string,"Y ");
			if (digital.pressed.button.z) strcat(string,"#GZ#W "); else strcat(string,"Z ");
			if (digital.pressed.button.l) strcat(string,"#GL#W "); else strcat(string,"L ");
			if (digital.pressed.button.r) strcat(string,"#GR#W "); else strcat(string,"R ");
			if (digital.pressed.button.up) strcat(string,"#GUp#W "); else strcat(string,"Up ");
			if (digital.pressed.button.down) strcat(string,"#GDn#W "); else strcat(string,"Dn ");
			if (digital.pressed.button.left) strcat(string,"#GLt#W "); else strcat(string,"Lt ");
			if (digital.pressed.button.right) strcat(string,"#GRt#W "); else strcat(string,"Rt ");
			if (digital.pressed.button.start) strcat(string,"#GSt#W "); else strcat(string,"St ");
			break;
		case ID_RACING:
			break;
		case ID_ANALOG:
			break;
		case ID_MOUSE:
			break;
		case ID_KEYBOARD:
			break;
		case ID_MD3B:
			break;
		case ID_MD6B:
			break;
		case ID_MDMOUSE:
			break;
		case ID_GUN:
			break;
		default:
			sprintf(string,"unknown type 0x%x, %d bytes",peripheral->type,peripheral->size);
			break;
	}
}

void update_controller(video_screen_mode_t screenmode)
{
	int x,y;
	char buf[256];
	smpc_peripheral_t *peripheral;
    smpc_peripheral_t *tmp_peripheral;

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
		ClearText(x,y,strlen(buf)*6,8);
		DrawString(buf, x, y, FONT_MAGENTA); y+=10;

		//drawing the directly connected peripheral
		if (_port->peripheral->size)
		{
			x = 10;
			_port->peripheral->port = 0;//setting port to 0 for direct connection
			get_peripheral_name(_port->peripheral,buf);
			ClearText(x,y,strlen(buf)*6,8);
			DrawString(buf, x, y, FONT_YELLOW);
			x = 100;
			get_peripheral_state(_port->peripheral,buf);
			ClearText(x,y,strlen(buf)*6,8);
			DrawString(buf, x, y, FONT_WHITE); y+=10;
		}
		
		//drawing the list
        for (peripheral = TAILQ_FIRST(&_port->peripherals);
			(peripheral != NULL) && (tmp_peripheral = TAILQ_NEXT(peripheral, peripherals), 1);
            	peripheral = tmp_peripheral) {
					x = 10;
					get_peripheral_name(peripheral,buf);
					ClearText(x,y,strlen(buf)*6,8);
					DrawString(buf, x, y, FONT_YELLOW);
					x = 100;
					get_peripheral_state(peripheral,buf);
					ClearText(x,y,strlen(buf)*6,8);
					DrawString(buf, x, y, FONT_WHITE); y+=10;
				}

		/*x+=_fw*10;
		if (controller.pressed.button.a)
			DrawString("A", x, y, FONT_GREEN);
		else
			DrawString("A", x, y, FONT_WHITE);
		x+=_fw*2;
		if (controller.pressed.button.b)
			DrawString("B", x, y, FONT_GREEN);
		else
			DrawString("B", x, y, FONT_WHITE);
		x+=_fw*2;
		if (controller.pressed.button.c)
			DrawString("C", x, y, FONT_GREEN);
		else
			DrawString("C", x, y, FONT_WHITE);
		x+=_fw*2;
		if (controller.pressed.button.x)
			DrawString("X", x, y, FONT_GREEN);
		else
			DrawString("X", x, y, FONT_WHITE);
		x+=_fw*2;
		if (controller.pressed.button.y)
			DrawString("Y", x, y, FONT_GREEN);
		else
			DrawString("Y", x, y, FONT_WHITE);
		x+=_fw*2;
		if (controller.pressed.button.z)
			DrawString("Z", x, y, FONT_GREEN);
		else
			DrawString("Z", x, y, FONT_WHITE);
		x+=_fw*2;
		if (controller.pressed.button.l)
			DrawString("L", x, y, FONT_GREEN);
		else
			DrawString("L", x, y, FONT_WHITE);
		x+=_fw*2;
		if (controller.pressed.button.r)
			DrawString("R", x, y, FONT_GREEN);
		else
			DrawString("R", x, y, FONT_WHITE);
		x+=_fw*2;
		if (controller.pressed.button.up)
			DrawString("Up", x, y, FONT_GREEN);
		else
			DrawString("Up", x, y, FONT_WHITE);
		x+=_fw*3;
		if (controller.pressed.button.down)
			DrawString("Down", x, y, FONT_GREEN);
		else
			DrawString("Down", x, y, FONT_WHITE);
		x+=_fw*5;
		if (controller.pressed.button.left)
			DrawString("Left", x, y, FONT_GREEN);
		else
			DrawString("Left", x, y, FONT_WHITE);
		x+=_fw*5;
		if (controller.pressed.button.right)
			DrawString("Right", x, y, FONT_GREEN);
		else
			DrawString("Right", x, y, FONT_WHITE);
		x+=_fw*6;
		if (controller.pressed.button.start)
			DrawString("Start", x, y, FONT_GREEN);
		else
			DrawString("Start", x, y, FONT_WHITE);*/
	}
	DrawString("Press Left + Start on controller 1 to Exit", 20, 200, FONT_WHITE);
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
	update_screen_mode(curr_screenmode,false);
	
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
			update_screen_mode(screenmode,false);
			return;
		}
		else
			key_pressed = false;
		vdp2_tvmd_vblank_out_wait();
		update_controller(curr_screenmode);
		vdp2_tvmd_vblank_in_wait();

		
	}
}
