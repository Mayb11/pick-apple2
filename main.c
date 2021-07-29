/*
 * main.c
 *
 *  Created on: 2021Äê4ÔÂ4ÈÕ
 *      Author: haozhe
 */
// Include drivers.
#include "GSD/GSD.h"
#include "DE1SoC_LT24/DE1SoC_LT24.h"
#include "HPS_Watchdog/HPS_Watchdog.h"
#include "HPS_usleep/HPS_usleep.h"
#include <stdlib.h>
void exitOnFail(signed int status, signed int successStatus){
    if (status != successStatus) {
        exit((int)status); //Add breakpoint here to catch failure
    }
}
// Select which displays to use.
#define DISPLAY1 0
#define DISPLAY2 4
#define DISPLAY3 2
// Peripheral base addresses.
volatile unsigned int *key_ptr = (unsigned int *)0xFF200050;
volatile unsigned int *private_timer_load	   = (unsigned int *) 0xFFFEC600;
volatile unsigned int *private_timer_value	   = (unsigned int *) 0xFFFEC604;
volatile unsigned int *private_timer_control   = (unsigned int *) 0xFFFEC608;
// Store the state of the keys last time we checked.
unsigned int key_last_value = 0;

unsigned int press_key() {

    // Store the current state of the keys.
    unsigned int key_current_value = *key_ptr;
    // If the key was down last cycle, and is up now, mark as pressed.
    unsigned int keys = (~key_current_value) & (key_last_value);
    // Save the key state for next time, so we can compare the next state to this.
    key_last_value = key_current_value;
    // Return result.
    return keys;
}
//main functions here
int main(void)
{

	unsigned int keys = 0;
	unsigned int load_value = 23;//in order to generate a random number of x location of apple

	unsigned int one_step = 10;//pixels each step of people move
	unsigned int people_xlocation = 119;//range in 0~229
	unsigned int people_ylocation = 309;//range in 9~309
	unsigned int apple_xlocation = 119;//range in 0~209
	unsigned int apple_ylocation = 9;//range in 9~309
	unsigned int apple_xlocation1	=119;
	unsigned int apple_ylocation1	=9;
	unsigned int apple_xlocation2	=119;
	unsigned int apple_ylocation2	=9;
	unsigned int SCORE 	 = 0;//how many apples does people picked
	unsigned int FAIL	= 0;//how many applese does people missed
	unsigned int mode	= 1;//how many apples fall down from the top at same time

	*private_timer_load	= load_value;
	*private_timer_control = (0<<8)|(0<<2)|(1<<1)|(1<<0);//enable the private timer

    //Initialise the LCD Display.
    exitOnFail(
            LT24_initialise(0xFF200060,0xFF200080), //Initialise LCD
            LT24_SUCCESS);                          //Exit if not successful
    HPS_ResetWatchdog();

    usleep(500000);
    HPS_ResetWatchdog();

	while (1)
	{
		// scan which mode we are in
		if(mode == 1)
		{
			unsigned int current_timevalue=*private_timer_value;//pick current timer value
			unsigned int next_xlocation=current_timevalue*10;//calculate next apple location
			keys = press_key();
			SCORE_SHOW(DISPLAY1,SCORE);//show SCORE in HEX0 1
			SCORE_SHOW(DISPLAY2,FAIL);//SHOW FAIL in HEX4 5
			SCORE_SHOW(DISPLAY3,mode);//show mode in HEX2 3
			LT24_clearDisplay(LT24_BLACK);// clear the scream
			LT24_drawBlock(LT24_GREEN,people_xlocation,people_ylocation,10,10);//show the people block,width of 10 pixels,length of 10 pixels
			//if apple was picked by people
			if (apple_xlocation== people_xlocation & apple_ylocation+10== people_ylocation)
			{
				SCORE=SCORE+1;// score plus one
				apple_xlocation=(next_xlocation-1);//use private timer calculate next apple's x location
				apple_ylocation=9;// origin y location of apple
				LT24_clearDisplay(LT24_BLUE);//scream show blue
				beep(1,10000);// 10000 is one time unit,enable beep and output beep sound for one time unit
			}
			// if apple was on the ground
			else if (apple_ylocation==309)
			{
				FAIL=FAIL+1;//fail plus one
				apple_xlocation=(next_xlocation-1);//use private timer calculate next apple's x location
				apple_ylocation=9;// origin y location of apple
				LT24_clearDisplay(LT24_CYAN);//scream show cyan
				beep(1,20000);//enable beep and output beep sound for two time unit to present apple was on the ground
			}
			//if key0 was pressed
			else if (keys & 0x1)
			{
				people_xlocation=people_xlocation+one_step;//people move right by one step(10 pixels)
			}
			//if key1 was pressed
			else if (keys & 0x2)
			{
				people_xlocation=people_xlocation-one_step;//people move left by one step(10 pixels)
			}
			//if key2 was pressed
			else if (keys & 0x4 )
			{
				//first check if mode greater than three or not,if greater than 3,put it back to 1
				if (mode >3)
				{
					mode = 1;
				}
				//if not greater than 3, mode add one at one time
				else
				{
					mode ++;
				}
			}
			// if key3 was pressed
			else if (keys & 0x8)
			{
				//reset all variables to origin status
				mode = 1;
				SCORE = 0;
				FAIL = 0;
				people_xlocation=119;
				people_ylocation=309;
				apple_xlocation	=119;
				apple_ylocation	=9;
			}
			//here is code which happened at the most of time
			else
			{
				LT24_clearDisplay(LT24_BLACK);//clear scream
				apple_ylocation=apple_ylocation+10;//apple fall by 10 pixels oen time
				LT24_drawBlock(LT24_RED,apple_xlocation,apple_ylocation,10,10);//show the current apple location
				HPS_ResetWatchdog();
				//usleep(1000);
			}
		    HPS_ResetWatchdog();

		}
		//below is 2 apples fall at one time and 3 apples fall at one time,
		//only different is add extra location of apples and check 2 times or 3 times
		//if apple was picked or on the ground
		else if (mode == 2)
		{
			unsigned int current_timevalue=*private_timer_value;
			unsigned int next_xlocation=current_timevalue*10;
			keys = press_key();
			SCORE_SHOW(DISPLAY1,SCORE);
			SCORE_SHOW(DISPLAY2,FAIL);
			SCORE_SHOW(DISPLAY3,mode);
			LT24_clearDisplay(LT24_BLACK);
			LT24_drawBlock(LT24_GREEN,people_xlocation,people_ylocation,10,10);


			if (apple_xlocation== people_xlocation & apple_ylocation+10== people_ylocation)
			{
				SCORE=SCORE+1;
				apple_xlocation=(next_xlocation-1);
				apple_ylocation=9;
				LT24_clearDisplay(LT24_BLUE);
				beep(1,10000);
			}
			else if (apple_xlocation1==people_xlocation & apple_ylocation1+10==people_ylocation)
			{
				SCORE=SCORE+1;
				apple_xlocation1=(next_xlocation-1);
				apple_ylocation1=9;
				LT24_clearDisplay(LT24_BLUE);
				beep(1,10000);
			}
			else if (apple_ylocation==309)
			{
				FAIL=FAIL+1;
				apple_xlocation=(next_xlocation-1);
				apple_ylocation=9;
				LT24_clearDisplay(LT24_CYAN);
				beep(1,20000);
			}
			else if (apple_ylocation1==309)
			{
				FAIL=FAIL+1;
				apple_xlocation1=(next_xlocation-1);
				apple_ylocation1=9;
				LT24_clearDisplay(LT24_CYAN);
				beep(1,20000);

			}
			else if (keys & 0x1)
			{
				people_xlocation=people_xlocation+one_step;
			}
			else if (keys & 0x2)
			{
				people_xlocation=people_xlocation-one_step;
			}
			else if (keys & 0x4 )
			{
				if (mode > 3)
				{
					mode = 1;
				}
				else
				{
					mode ++;
				}
			}
			else if (keys & 0x8)
			{
				mode = 1;
				SCORE = 0;
				FAIL = 0;
				people_xlocation=119;
				people_ylocation=309;
				apple_xlocation	=119;
				apple_ylocation	=9;
				apple_xlocation1	=119;
				apple_ylocation1	=9;
				//two_apples = 0;
			}
			else
			{
				LT24_clearDisplay(LT24_BLACK);
				apple_ylocation=apple_ylocation+10;
				apple_ylocation1=apple_ylocation1+10;
				LT24_drawBlock(LT24_RED,apple_xlocation,apple_ylocation,10,10);
				LT24_drawBlock(LT24_RED,apple_xlocation1,apple_ylocation1,10,10);

				HPS_ResetWatchdog();

			}
		}
		else if (mode == 3)
		{
			unsigned int current_timevalue=*private_timer_value;
			unsigned int next_xlocation=current_timevalue*10;
			keys = press_key();
			SCORE_SHOW(DISPLAY1,SCORE);
			SCORE_SHOW(DISPLAY2,FAIL);
			SCORE_SHOW(DISPLAY3,mode);
			LT24_clearDisplay(LT24_BLACK);
			LT24_drawBlock(LT24_GREEN,people_xlocation,people_ylocation,10,10);


			if (apple_xlocation== people_xlocation & apple_ylocation+10== people_ylocation)
			{
				SCORE=SCORE+1;
				apple_xlocation=(next_xlocation-1);
				apple_ylocation=9;
				LT24_clearDisplay(LT24_BLUE);
				beep(1,10000);
			}
			else if (apple_xlocation1==people_xlocation & apple_ylocation1+10==people_ylocation)
			{
				SCORE=SCORE+1;
				apple_xlocation1=(next_xlocation-1);
				apple_ylocation1=9;
				LT24_clearDisplay(LT24_BLUE);
				beep(1,10000);
			}
			else if (apple_xlocation2==people_xlocation & apple_ylocation2+10==people_ylocation)
			{
				SCORE=SCORE+1;
				apple_xlocation2=(next_xlocation-1);
				apple_ylocation2=9;
				LT24_clearDisplay(LT24_BLUE);
				beep(1,10000);
			}
			else if (apple_ylocation==309)
			{
				FAIL=FAIL+1;
				apple_xlocation=(next_xlocation-1);
				apple_ylocation=9;
				LT24_clearDisplay(LT24_CYAN);
				beep(1,20000);
			}
			else if (apple_ylocation1==309)
			{
				FAIL=FAIL+1;
				apple_xlocation1=(next_xlocation-1);
				apple_ylocation1=9;
				LT24_clearDisplay(LT24_CYAN);
				beep(1,20000);
			}
			else if (apple_ylocation2==309)
			{
				FAIL=FAIL+1;
				apple_xlocation2=(next_xlocation-1);
				apple_ylocation2=9;
				LT24_clearDisplay(LT24_CYAN);
				beep(1,20000);
			}
			else if (keys & 0x1)
			{
				people_xlocation=people_xlocation+one_step;
			}
			else if (keys & 0x2)
			{
				people_xlocation=people_xlocation-one_step;
			}
			else if (keys & 0x4 )
			{
				if (mode > 3)
				{
					mode = 1;
				}
				else
				{
					mode ++;
				}
			}
			else if (keys & 0x8)
			{
				mode = 1;
				SCORE = 0;
				FAIL = 0;
				people_xlocation=119;
				people_ylocation=309;
				apple_xlocation	=119;
				apple_ylocation	=9;
				apple_xlocation1=119;
				apple_ylocation1=9;
				apple_xlocation2=119;
				apple_ylocation2=9;
				//two_apples = 0;
			}
			else
			{
				LT24_clearDisplay(LT24_BLACK);
				apple_ylocation=apple_ylocation+10;
				apple_ylocation1=apple_ylocation1+10;
				apple_ylocation2=apple_ylocation2+10;
				LT24_drawBlock(LT24_RED,apple_xlocation,apple_ylocation,10,10);
				LT24_drawBlock(LT24_RED,apple_xlocation1,apple_ylocation1,10,10);
				LT24_drawBlock(LT24_RED,apple_xlocation2,apple_ylocation2,10,10);

				HPS_ResetWatchdog();

			}

		}


	}


}

