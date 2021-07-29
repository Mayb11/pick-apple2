/*
 * GSD.h
 *
 *  Created on: 2021Äê4ÔÂ4ÈÕ
 *      Author: haozhe
 */

#ifndef GSD_GSD_H_
#define GSD_GSD_H_

//Include required header files
#include <stdbool.h> //Boolean variable type "bool" and "true"/"false" constants.

signed int LT24_drawBlock(unsigned short colour,unsigned int x,unsigned int y,unsigned int w,unsigned int h);

void DE1SoC_SevenSeg_Write(unsigned int display, unsigned char value);

void DE1SoC_SevenSeg_SetSingle(unsigned int display, unsigned int value);

void SCORE_SHOW(unsigned int display, unsigned int value);


void beep(unsigned int enable, unsigned int time);


#endif
//Error Codes
