/*
 * main.c
 */

#include <stdint.h>
#include <stdio.h>
#include "usbstk5505.h"

#pragma	DATA_SECTION( outBuffer , "expdata0");
#pragma	DATA_SECTION( inBuffer , "expdata1");
#define DATALENGTH   4800
short inBuffer[DATALENGTH];
short outBuffer[DATALENGTH];

void *bufferH[103];
void *bufferL[51];
int coef = 0;
int a =0, k= 0, j=0, i=102;

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


void queueH(void *value){
	bufferH[coef]= value;
	coef++;
	if (coef == 102) {
		coef = 0;
	}
}

void queueL(void *value){
	bufferL[coef]= &value;
	coef++;
	if (coef == 50){
		coef = 0;
	}
}

//code above is circular buffer example from
 	 int main(void) {
//here we * the chirp with the HighPass
 		 //set a temp variable to check values
	for (a=0;a<4799;a++)
	{
		Temp[i]= BHigh[i]*inBuffer[j];
		//Temp[i]=Temp[i]<<15;
		C[k]=C[k]+Temp[i];
		i--;
		if (C[k] > 32766) C[k]=C[k]/32766;
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
}

