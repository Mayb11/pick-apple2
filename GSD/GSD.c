/*
 * GSD.c
 *
 *  Created on: 2021Äê4ÔÂ4ÈÕ
 *      Author: haozhe
 */

#include "../GSD/GSD.h"
#include "DE1SoC_LT24/DE1SoC_LT24.h"
#include "HPS_Watchdog/HPS_Watchdog.h"
#include "HPS_usleep/HPS_usleep.h" //some useful delay routines
#include "DE1SoC_WM8731/DE1SoC_WM8731.h"
#include <math.h>
#include <stdlib.h>

#define F_SAMPLE 48000.0        //Sampling rate of WM8731 Codec (Do not change)
#define PI2      6.28318530718  //2 x Pi      (Apple or Peach?)

volatile unsigned int*  LEDR_ptr  = (unsigned int *) 0xFF200000; // Red LEDs base address
volatile unsigned char* fifospace_ptr;
volatile unsigned int*  audio_left_ptr;
volatile unsigned int*  audio_right_ptr;



signed int LT24_drawBlock(unsigned short colour,unsigned int x,unsigned int y,unsigned int w,unsigned int h)
{
    signed int status = LT24_setWindow(x,y,w,h);
	unsigned int i, j;//Define single pixel window
    if (status != LT24_SUCCESS) return status;   //Check for any errors
    for (i = 0;i<w;i++)
    {
    	for (j = 0;j<h;j++)
    	{
            LT24_write(true, colour);
    	}
    }
    return LT24_SUCCESS;                         //And Done
}
volatile unsigned char *sevenseg_base_lo_ptr=(unsigned char *)0xFF200020;
volatile unsigned char *sevenseg_base_hi_ptr=(unsigned char *)0xFF200030;

//volatile unsigned char *sevenseg_base_lo_ptr = 0;
//volatile unsigned char *sevenseg_base_hi_ptr = 0;

// There are four HEX displays attached to the low (first) address.
#define SEVENSEG_N_DISPLAYS_LO 4

// There are two HEX displays attached to the high (second) address.
#define SEVENSEG_N_DISPLAYS_HI 2

void DE1SoC_SevenSeg_Write(unsigned int display, unsigned char value) {
    // Select between the two addresses so that the higher level functions
    // have a seamless interface.
    if (display < SEVENSEG_N_DISPLAYS_LO) {
        // If we are targeting a low address, use byte addressing to access
        // directly.
        sevenseg_base_lo_ptr[display] = value;
    } else {
        // If we are targeting a high address, shift down so byte addressing
        // works.
        display = display - SEVENSEG_N_DISPLAYS_LO;
        sevenseg_base_hi_ptr[display] = value;
    }
}

void DE1SoC_SevenSeg_SetSingle(unsigned int display, unsigned int value) {
    // ToDo: Write the code for driving a single seven segment display here.
    // Your function should turn a real value 0-F into the correctly encoded
    // bits to enable the correct segments on the seven segment display to
    // illuminate. Use the DE1SoC_SevenSeg_Write function you created earlier
    // to set the bits of the display.
		//display=display+2;
		if (value==0){
			DE1SoC_SevenSeg_Write(display,0x3F);// light segment 0,1,2,3,4,5
		}
		if (value==1){
			DE1SoC_SevenSeg_Write(display,0x06);//  light segment 1,2
		}
		if (value==2){
			DE1SoC_SevenSeg_Write(display,0x5B);// light segment 0,1,3,4,6
		}
		if (value==3){
			DE1SoC_SevenSeg_Write(display,0x4F);// light segment 0,1,2,3,6
		}
		if (value==4){
			DE1SoC_SevenSeg_Write(display,0x66);// light segment 1,2,5,6
		}
		if (value==5){
			DE1SoC_SevenSeg_Write(display,0x6D);// light segment 0,2,3,5,6
		}
		if (value==6){
			DE1SoC_SevenSeg_Write(display,0x7D);// light segment 0,2,3,4,5,6
		}
		if (value==7){
			DE1SoC_SevenSeg_Write(display,0x07);//light segment 0,1,2
		}
		if (value==8){
			DE1SoC_SevenSeg_Write(display,0x7F);// light all segment
		}
		if (value==9){
			DE1SoC_SevenSeg_Write(display,0x67);// light segment 0,1,2,5,6
		}
	}


void SCORE_SHOW(unsigned int display, unsigned int value) {

	unsigned int valuefH=value%10;// take the first bit of the input number
	unsigned int valuesH=value/10;// take the second bit of the input number
	DE1SoC_SevenSeg_SetSingle(display, valuefH);// light the first bit number in the display by setsingle function
	DE1SoC_SevenSeg_SetSingle(display+1, valuesH);// light the second bit number in the display by setsingle functin
}

void beep(unsigned int enable, unsigned int time)
{
	double phase = 0.0;  // Phase accumulator
	double inc   = 0.0;  // Phase increment
	double ampl  = 0.0;  // Tone amplitude (i.e. volume)
	signed int audio_sample = 0;
	unsigned int counter;
	//unsigned int enable;
	//unsigned int counter;
	exitOnFail(
	            WM8731_initialise(0xFF203040),  //Initialise Audio Codec
	            WM8731_SUCCESS);                //Exit if not successful
	    //Clear both FIFOs
	    WM8731_clearFIFO(true,true);
	    //Grab the FIFO Space and Audio Channel Pointers
	    fifospace_ptr = WM8731_getFIFOSpacePtr();
	    audio_left_ptr = WM8731_getLeftFIFOPtr();
	    audio_right_ptr = WM8731_getRightFIFOPtr();

	    //Initialise Phase Accumulator
	    inc   = 440.0 * PI2 / F_SAMPLE;  // Calculate the phase increment based on desired frequency - e.g. 440Hz
	    ampl  = 8388608.0;               // Pick desired amplitude (e.g. 2^23). WARNING: If too high = deafening!
	    phase = 0.0;
	    // Primary function while loop
	    while (enable) {

	    	if (counter >= time) enable=0;
	        //Always check the FIFO Space before writing or reading left/right channel pointers
	        if ((fifospace_ptr[2] > 0) && (fifospace_ptr[3] > 0)) {
	            //If there is space in the write FIFO for both channels:
	            //Increment the phase
	            phase = phase + inc;
	            //Ensure phase is wrapped to range 0 to 2Pi (range of sin function)
	            while (phase >= PI2) {
	                phase = phase - PI2;
	            }
	            // Calculate next sample of the output tone.
	            //for (T=0;T<100;T++)

	            	audio_sample = (signed int)( ampl * sin( phase ) );
	            	// Output tone to left and right channels.
	            	*audio_left_ptr = audio_sample;
	            	*audio_right_ptr = audio_sample;

	        }
	        counter++;
	        //Debugging - display FIFO space on red LEDs.
	        *LEDR_ptr = fifospace_ptr[2];  // Output 'WSRC' register to the red LEDs.
	        //Finally reset the watchdog.
	        HPS_ResetWatchdog();
	    }
}


