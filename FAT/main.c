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
#include "Directory_Functions.h"


void main(void)
{
	uint32 block_num, current_directory, ent_clus;
	uint8 error_val;
	uint8 xdata block_data[512];
	uint16 num_entries, entry_number;

	REDLED = 0;	// Sanity check
	UART_INIT();
	printf("Initializing SPI Master to 400 KHz...\n");
	YELLOWLED = 0;
	SPI_Master_Init(400000UL);
	AMBERLED = 0;
	printf("Initializing SD Card...\n");
	SD_Card_init();
	printf("Initializing SPI Master to 5 MHz\n");
	GREENLED = 0;
	SPI_Master_Init(5000000UL);
	YELLOWLED = 0;
	printf("Mounting drive...\n");
	mount_drive(block_data);
	AMBERLED = 0;
	current_directory = FirstRootDirSec_g;
	AUXR = 0x0C; // Allows use of 1 KB of RAM
	GREENLED = 0;
	while(1)
	{
		num_entries = Print_Directory(current_directory, block_data);
		YELLOWLED = 0;
		printf("Enter entry number: ");
		entry_number = (uint16) long_serial_input();
		if(entry_number <= num_entries)
		{
			ent_clus = Read_Dir_Entry(current_directory, entry_number, block_data);
			if(ent_clus & directory_bit)
			{
				ent_clus &= 0x0FFFFFFF;
				current_directory = First_Sector(ent_clus);
			}
			else
			{
				Open_File(ent_clus, block_data);
			}
		}
		/*
		//block_num = long_serial_input(); // Blocks until user enters a number (BS and DEL supported)
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
		*/
	}
}