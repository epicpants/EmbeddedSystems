/*
 * Authors: Jon Eftink and Tyler Ryan
 * File: main.c
 * Brief: entry point for the UART memory test program which outputs the contents
 * of blocks of memory via UART
 */
#include <main.h>
#include <memory_test.h>
#include <print.h>
#include <port.h>
#include <UART.h>
#include <stdio.h>

void main(void)
{
    uint8 * generic_p;
    uint16 index;
	uint8 num_bytes = 64;

    // "I'm alive" LED trigger to signal code entry
    GREENLED = 0;
    for(index = 0; index < 50000; index++);
    GREENLED = 1;
	UART_INIT();
	// Infinite loop so that all outputs will be seen
	while(1)
	{
		generic_p = code_memory_init();
	    print_memory_block(num_bytes, generic_p);
	    generic_p = xdata_memory_init();
	    print_memory_block(num_bytes, generic_p);	
	}
}
