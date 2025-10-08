#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <yaul.h>
#include "font.h"
#include "video_vdp2.h"
#include "control.h"

void get_digital_keypress_anywhere(smpc_peripheral_digital_t *digital)
{
    //smpc_peripheral_digital_port(1, digital);
    //return;
	//cycling through all peripherals until we meet one with digital key pressed
	//smpc_peripheral_port_t * const _port;
	smpc_peripheral_digital_t tmp_digital;
	smpc_peripheral_t *peripheral;
	smpc_peripheral_t *tmp_peripheral;
	digital->pressed.raw = 0;
    int y=100;
    char buf[128];
    char buf2[32];
	for (int port = 1; port <3; port++)
	{
		//_port = (smpc_peripheral_port_t*)smpc_peripheral_raw_port(port);
		const smpc_peripheral_port_t * const _port = smpc_peripheral_raw_port(port);
        //first checking single value
        //peripheral = _port->peripheral;

		/*DrawStringWithBackground("                                                 ", 5, y, FONT_YELLOW,FONT_BLACK);
		switch (_port->peripheral->type) {
			case ID_DIGITAL:  sprintf(buf2,"DIG"); break;
			case 1:  sprintf(buf2,"TAP"); break;
			case 0:  sprintf(buf2,"N/A"); break;
			default:  sprintf(buf2,"T%d",_port->peripheral->type); break;
		}
		sprintf(buf,"P%d: %s, con%x size%x cap%x",port,buf2,
			_port->peripheral->connected,_port->peripheral->size,_port->peripheral->capacity);
		DrawString(buf, 5, y, FONT_YELLOW);
		y+=10;*/

		//checking if the peripheral is actually a digital device
		//if (TYPE_DIGITAL == peripheral->type)
        //sprintf(buf,"%d type = %d (%d)",port,peripheral->type,peripheral->connected);
        //DrawString(buf, 10, y, FONT_WHITE);
		if (ID_DIGITAL == _port->peripheral->type)
		{
				/*DrawStringWithBackground("                                                 ", 5, y, FONT_YELLOW,FONT_BLACK);
				uint8_t * p8 = ((uint8_t *)&_port->peripheral->previous_data[0]);
				sprintf(buf,"peripheral-> = %x digital-> = %x",p8[0],digital->previous.pressed.raw);
				DrawString(buf, 5, y, FONT_YELLOW);
	            y+=10;*/
			smpc_peripheral_digital_get(_port->peripheral,&tmp_digital);
			//if the key is pressed, searching no more
			if (tmp_digital.pressed.raw)
			{
				/*DrawStringWithBackground("                                                 ", 5, y, FONT_YELLOW,FONT_BLACK);
				sprintf(buf,"Pressed : %x",digital->pressed.raw);
				DrawString(buf, 5, y, FONT_YELLOW);
	            y+=10;*/
				digital[0] = tmp_digital;
				//return;
			}
		}
		//now the list	
		for (peripheral = TAILQ_FIRST(&_port->peripherals);
			(peripheral != NULL) && (tmp_peripheral = TAILQ_NEXT(peripheral, peripherals), 1);
           	peripheral = tmp_peripheral) {
				//checking if the peripheral is actually a digital device
                //sprintf(buf,"%dL type = %d (%d)",port,peripheral->type,peripheral->connected);
                //DrawString(buf, 10, y, FONT_WHITE);
				/*DrawStringWithBackground("                                 ", 5, y, FONT_YELLOW,FONT_BLACK);
				sprintf(buf,"Per %x type %x port %x size %x",peripheral, peripheral->type, peripheral->port, peripheral->size);
				DrawString(buf, 5, y, FONT_YELLOW);
                y+=10;*/
				if (ID_DIGITAL == peripheral->type)
				{
					smpc_peripheral_digital_get(peripheral,&tmp_digital);
					//if the key is pressed, searching no more
					if (tmp_digital.pressed.raw)
					{
						/*DrawStringWithBackground("                                                 ", 5, y, FONT_YELLOW,FONT_BLACK);
						sprintf(buf,"Pressed : %x",digital->pressed.raw);
						DrawString(buf, 5, y, FONT_YELLOW);
	            		y+=10;*/
						digital[0] = tmp_digital;
						//return;
					}
				}
			}
	}
}