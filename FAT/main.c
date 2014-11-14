/*
 * Authors: Jon Eftink and Tyler Ryan
 * File: main.c
 * Brief: Entry point for the program. Initializes SD Card communication and 
 * allows user to view SD Card memory contents via UART in 512 byte blocks
 */
#include "main.h"
#include "UART.h"
#include <stdio.h>
#include "SD.h"
#include "SPI.h"
#include "Serial_In.h"
#include "print.h"


void main(void)
{
	uint32 block_num;
	uint8 error_val;
	uint8 xdata block_data[512];	

	REDLED = 0;	// Sanity check
	UART_INIT();
	printf("Initializing SPI Master to 400 KHz...\n");
	SPI_Master_Init(400000UL);
	printf("Initializing SD Card...\n");
	SD_Card_init();
	printf("Initializing SPI Master to 5 MHz\n");
	SPI_Master_Init(5000000UL);
	AUXR = 0x0C; // Allows use of 1 KB of RAM
	while(1)
	{
		printf("Enter block number: ");
		block_num = long_serial_input(); // Blocks until user enters a number (BS and DEL supported)
		nCS0 = 0;
		error_val = send_command(17, block_num);
		if(error_val == NO_ERRORS)
		{
			error_val = read_block(512, block_data); // Populate block_data
			if(error_val == NO_ERRORS)
			{
				print_memory_block(512, block_data); // Print block
			}
			else
			{
				printf("Error in read block\n");
			}
			
		}
		else if(error_val == TIMEOUT_ERROR)
		{
			printf("Timeout error occurred\n");
		}
		else if(error_val == SPI_ERROR)
		{
			printf("SPI error occurred\n");
		}
		else // This should not happen
		{
			printf("SPI error occurred: %2.2bx\n", error_val);
		}
		nCS0 = 1;
	}
}