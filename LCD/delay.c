/*
 * Authors: Jon Eftink and Tyler Ryan
 * File: delay.c
 * Brief: Delay routine to wait x milliseconds 
 */
#include <delay.h>

/***********************************************************************
DESC:  Blocks microcontroller for specified number of milliseconds
INPUT: timeout value
RETURNS: void
CAUTION: 
************************************************************************/
void Delay_ms(uint32 timeout_val_ms)
{
	uint32 millisec;
	TMOD &= 0xF0; // clear T0 bits
	TMOD |= 0x01;
	ET0 = 0; // turn off interrupts
	for(millisec = 0; millisec < timeout_val_ms; millisec++)
	{
		TH0 = PRELOADH1;
		TL0 = PRELOADL1;
		TF0 = 0; // clears overflow flag
		TR0 = 1; // starts the timer
		while(TF0 == 0); // wait for overflow
		TR0 = 0;
	}
}