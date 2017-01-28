/*****************************************************************************/
/*                                                                           */
/* FILENAME                                                                  */
/* 	 main.c                                                                  */
/*                                                                           */
/* DESCRIPTION                                                               */
/*   TMS320C5505 USB Stick. Application 2. Improved Audio Template.          */
/*   Take microphone input and send to headphones.                           */
/*                                                                           */
/* REVISION                                                                  */
/*   Revision: 1.00	                                                         */
/*   Author  : Richard Sikora                                                */
/*---------------------------------------------------------------------------*/
/*                                                                           */
/* HISTORY                                                                   */
/*   Revision: 1.00                                                          */
/*   5th March 2010. Created by Richard Sikora from TMS320C5510 DSK code.    */
/*                                                                           */
/*****************************************************************************/
/*
 * Copyright (C) 2010 Texas Instruments Incorporated - http://www.ti.com/ 
 * 
 * 
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions 
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the   
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
*/

#include "stdio.h"
#include "usbstk5505.h"
#include "aic3204.h"
#include "PLL.h"
#include "stereo.h"
#include "LEDflasher.h"

Int16 left_input;
Int16 right_input;
Int16 left_output;
Int16 right_output;
Int16 mono_input;
Int16 *bufferH[103];
Int16 *bufferL[51];
int coef = 0;
int a =0, k= 0, j=0, i=102,p=0,x=0;
short inBuffer[4800];
short outBuffer[4800];
Int16 S[103];
Int16 A[103];
Int16 B[103];
Int16 C[103]; //7ffff
Int16 L[103];
Int16 Temp[103];
const int BL = 103;
Int16 BHigh[103] = {
     -564,    336,    250,    184,    133,     92,     56,     24,     -7,
      -36,    -65,    -92,   -117,   -139,   -156,   -168,   -171,   -167,
     -153,   -129,    -95,    -52,     -1,     57,    118,    182,    243,
      300,    348,    384,    404,    405,    385,    342,    273,    179,
       60,    -83,   -247,   -429,   -626,   -833,  -1044,  -1253,  -1456,
    -1646,  -1818,  -1966,  -2086,  -2174,  -2229,  30521,  -2229,  -2174,
    -2086,  -1966,  -1818,  -1646,  -1456,  -1253,  -1044,   -833,   -626,
     -429,   -247,    -83,     60,    179,    273,    342,    385,    405,
      404,    384,    348,    300,    243,    182,    118,     57,     -1,
      -52,    -95,   -129,   -153,   -167,   -171,   -168,   -156,   -139,
     -117,    -92,    -65,    -36,     -7,     24,     56,     92,    133,
      184,    250,    336,   -564
};


Int16 BLow[51] = {
      -60,   -178,   -163,    240,    895,   1137,    502,   -430,   -504,
      400,    909,     23,  -1108,   -584,   1143,   1360,   -806,  -2244,
      -68,   3132,   1793,  -3891,  -5394,   4403,  20317,  28184,  20317,
     4403,  -5394,  -3891,   1793,   3132,    -68,  -2244,   -806,   1360,
     1143,   -584,  -1108,     23,    909,    400,   -504,   -430,    502,
     1137,    895,    240,   -163,   -178,    -60
};


//#define SAMPLES_PER_SECOND 24000L

/* New. Gain as a #define */
/* Use 30 for microphone. Use 0 for line */
//#define GAIN_IN_dB 5

//changed define to global variables
Int16 GAIN_IN_dB = 5;
Uint32 SAMPLES_PER_SECOND =24000L;


/* New. Variable for step */
unsigned int Step = 0; 


/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  main( )                                                                 *
 *                                                                          *
 * ------------------------------------------------------------------------ */


void queueH(void *value){
	bufferH[coef]= value;
	coef++;
	if (coef == 102) {
		coef = 0;
	}
}

void queueL(void *value){
	bufferL[coef]= value;
	coef++;
	if (coef == 50){
		coef = 0;
	}
}


void main( void ) 
{
    /* Initialize BSL */
    USBSTK5505_init( );
	
	/* Initialize PLL */
	pll_frequency_setup(100);

    /* Initialise hardware interface and I2C for code */
    aic3204_hardware_init();
    
    /* Initialise the AIC3204 codec */
	aic3204_init(); 

    printf("\n\nRunning Improved Audio Template Project\n");
    printf( "<-> Audio Loopback from Stereo IN --> to HP/Lineout\n\n" );

	/* Setup sampling frequency and 30dB gain for microphone */
    set_sampling_frequency_and_gain(SAMPLES_PER_SECOND, GAIN_IN_dB);

    
	/* New. Default to XF LED off */
	asm(" bclr XF");
   
 	for ( i = 0  ; i < SAMPLES_PER_SECOND * 600L  ;i++  )
 	{
 		//Default Values
 		GAIN_IN_dB = 5;
 		SAMPLES_PER_SECOND =24000L;

	 	for (p=0;p<103;p++){

	 	     aic3204_codec_read(&left_input, &right_input); // Configured for one interrupt per two channels.

	 	     mono_input = stereo_to_mono(left_input, right_input);
inBuffer[p]=left_input;
	 	}
     //here we * the chirp with the HighPass
      		 //set a temp variable to check values
	 	if (p==103){
	 		for (a=0;a<4799;a++)
	 		{
	 			Temp[i]= BHigh[i]*inBuffer[j];
	 			Temp[i]=Temp[i]<<15;
	 			C[k]=C[k]+Temp[i];
	 			i--;
	 			if (C[k] > 32766) C[k]=C[k]/32766; //was originally used because of overlapping in developing FIR filter which was caused by breakpointing wrong
	 												//used as a cautionary step

	 			if (i==0){

	 				queueH(&C[k]);
	 				k++;
	 				j++;
	 				i=102;
	 				C[k]=0;
	 			}

	 		}
	 	//here we * the chirp with a Low Pass and different method was used to input
	 		for (a=0;a<4799;a++)
	 		{
	 			outBuffer[k]=outBuffer[k]+BLow[i]*C[j];
	 			i++;

	 			if (i==51){

	 				queueL(outBuffer);
	 				k++;
	 				j++;
	 				i=0;
	 			}


     	}


	 /* New. LED flasher */
      Step = LEDFlasher(3); // Takes total number of steps as argument, here 3.
			for (x=0;x<103;x++){
				left_output=outBuffer[x];
				 aic3204_codec_write(left_output, left_output);

			}
				 //mono using left output only
				 p=0;
	 	}
 	}

   /* Disable I2S and put codec into reset */
    aic3204_disable();

    printf( "\n***Program has Terminated***\n" );
    SW_BREAKPOINT;
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  End of main.c                                                           *
 *                                                                          *
 * ------------------------------------------------------------------------ */
