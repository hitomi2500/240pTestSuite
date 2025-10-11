#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <yaul.h>
#include "font.h"
#include "video_vdp2.h"
#include "control.h"

void get_digital_keypress_anywhere(smpc_peripheral_digital_t *digital)
{
	volatile smpc_peripheral_digital_t tmp_digital;
	smpc_peripheral_t *peripheral;
	smpc_peripheral_t *tmp_peripheral;
	digital->pressed.raw = 0;
	tmp_digital.pressed.raw = 0;
    int y=100;
    char buf[128];
    char buf2[32];
	for (int port = 1; port <3; port++)
	{
		const smpc_peripheral_port_t * const _port = smpc_peripheral_raw_port(port);

		if (ID_DIGITAL == _port->peripheral->type) {
			smpc_peripheral_digital_get(_port->peripheral,&tmp_digital);
			if (tmp_digital.pressed.raw) {
				digital[0] = tmp_digital;
				//return;	//if the key is pressed, searching no more
			}
		} else if (ID_ANALOG == _port->peripheral->type) {
			uint8_t * p8 = (uint8_t*)(&tmp_digital.pressed.raw);
	        p8[0] = _port->peripheral->data[0];
	        p8[1] = _port->peripheral->data[1]&0xF8;
			if (tmp_digital.pressed.raw) {
				digital[0] = tmp_digital;
				//some optimization junk, wtf are you doing gcc?
				sprintf(buf,"P%d: %s, con%x size%x cap%x",port,buf2,_port->peripheral->connected,_port->peripheral->size,_port->peripheral->capacity);
				//return;	//if the key is pressed, searching no more
			}
		}
		//now the list	
		for (peripheral = TAILQ_FIRST(&_port->peripherals);
			(peripheral != NULL) && (tmp_peripheral = TAILQ_NEXT(peripheral, peripherals), 1);
           	peripheral = tmp_peripheral) {
				if (ID_DIGITAL == peripheral->type) {
					smpc_peripheral_digital_get(peripheral,&tmp_digital);
					if (tmp_digital.pressed.raw) {
						digital[0] = tmp_digital;
						//return;	//if the key is pressed, searching no more
					}
				} else if (ID_ANALOG == peripheral->type) {
					uint8_t * p8 = (uint8_t*)(&tmp_digital.pressed.raw);
					p8[0] = peripheral->data[0];
					p8[1] = peripheral->data[1]&0xF8;
					if (tmp_digital.pressed.raw) {
						digital[0] = tmp_digital;
						//some optimization junk, wtf are you doing gcc?
						sprintf(buf,"P%d: %s, con%x size%x cap%x",port,buf2,_port->peripheral->connected,_port->peripheral->size,_port->peripheral->capacity);
						//return;	//if the key is pressed, searching no more
					}
				}
			}
	}
}