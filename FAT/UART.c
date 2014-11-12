/*
 * Authors: Jon Eftink and Tyler Ryan
 * File: UART.c
 * Brief: function definition of UART initialization
 */
#include <UART.h>
#include <AT89C51RC2.h>

/***********************************************************************
DESC:  Initializes SFRs in the 8051 to set up UART communication
INPUT: SFR names via AT89C51RC2.h
RETURNS: void
CAUTION: 
************************************************************************/
void UART_INIT(void)
{
	PCON |= (SMOD1 << 7);
	PCON &= ((SMOD1 << 7) | 0x3F);
	SCON = 0x50;
	BDRCON = 0;
	BRL = BRL_VALUE;
	BDRCON = (0x1C | (SPD << 1));
	ES = 0;
	TI = 1;
}