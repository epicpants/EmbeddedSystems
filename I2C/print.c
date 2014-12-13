/*
 * Authors: Jon Eftink and Tyler Ryan
 * File: print.c
 * Brief: function definitions for printing lines and blocks of memory
 * via UART
 */
#include <stdio.h>
#include "print.h"

/***********************************************************************
DESC:  Prints a line of memory contents via UART
INPUT: pointer to starting address in memory (array_in_p)
RETURNS: prints pointer address and 16 contiguous memory location contents
		 via UART
CAUTION: 
************************************************************************/
void print_line(uint8 * array_in_p)
{
    uint8 index, dat;
    printf("%p ", array_in_p);
    // Print 16 bytes as hex
    for(index = 0; index < 16; index++)
    {
        dat = *(array_in_p + index);
        printf("%2.2bX ", dat);
    }
    // Print 16 values as ASCII
    for(index = 0; index < 16; index++)
    {
        dat = *(array_in_p + index);
        if((dat < 0x20) || (dat > 0x7E)) {dat = 0x2E;}
        putchar(dat);
    }
}

/***********************************************************************
DESC:  Prints a block of memory location contents
INPUT: total number of memory locations to print contents of (num_bytes)
	   pointer to starting address of memory locations (address_p)
RETURNS: calls print_line until the requested number of memory location
		 contents have been printed via UART
CAUTION: 
************************************************************************/
void print_memory_block(uint16 num_bytes, uint8 * address_p)
{
    uint16 index;
    printf("ADDR   0  1  2  3  4  5  6  7  8  9  10 11 12 13 14 15");
	putchar(CR);
	putchar(LF);
    index = (uint16) address_p;
    index = index & 0x000F;
    address_p = address_p - index;
    index = 0;
    while(index < num_bytes)
    {
        print_line(address_p + index);
        index += 16;
        putchar(CR);
        putchar(LF);
    }
}