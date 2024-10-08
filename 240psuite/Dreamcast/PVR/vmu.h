/* 
 * 240p Test Suite
 * Copyright (C)2011-2022 Artemio Urbina
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with 240p Test Suite; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef VMU_H
#define VMU_H

int  isLCDPresent();
int  isVMUPresent();
void updateVMU(char *line1, char*line2, int force);
void updateVMUFlash(char *line1, char*line2, int force);
void updateVMUGraphic(char **xpm);
void updateVMUGraphic_low_priority(char **xpm);
void updateVMUGraphicDev(maple_device_t *vmu, char **xpm);
void updateVMU_Donna();
void updateVMU_SD();
void updateVMU_SD_Dev(maple_device_t *vmu);
void updateVMU_wait();
void clearVMUGraphicDev(maple_device_t *vmu);
void SD_blink_cycle();
void disableVMU_LCD();
void enableVMU_LCD();

int vmu_found_bad_lcd_vmu();
void vmu_report_controller_swap(int no_controller);

void sleep_VMU_cycle();
void resetSleep();
void enableSleep();
void disableSleep();

#endif
