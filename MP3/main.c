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
#include "button.h"
#include "I2C.h"
#include "STA013.h"



#define TIMEOUT_VAL (50)



void main(void)
{
	uint8 xdata block_data_1[512];
	uint8 xdata block_data_2[512];
	uint32 idata current_directory;
	uint32 idata ent_clus;
	uint8 idata error_val;
	uint16 idata num_entries;
 	uint16 idata entry_number;

	AUXR = 0x0C; // Allows use of 1 KB of RAM
	UART_INIT();
	printf("Initializing SPI Master to 400 KHz...\n");
	YELLOWLED = 0;
	SPI_Master_Init(400000UL);
	printf("Initializing SD Card...\n");
	SD_Card_init();
	printf("Initializing SPI Master to 8 MHz\n");
	error_val = SPI_Master_Init(8000000UL);
	printf("SPI Init error_val: %2.2BX\n", error_val);
	YELLOWLED = 0;
	printf("Mounting drive...\n");
	mount_drive(block_data_1);
	AMBERLED = 0;
	current_directory = FirstRootDirSec_g;

	error_val = init_decoder();
	if(error_val != NO_ERRORS)
	{
		printf("Initialization of the MP3 decoder failed.\n");
		REDLED = 0;
	}


	
	GREENLED = 0;
	while(1)
	{
		num_entries = Print_Directory(current_directory, block_data_1);
		YELLOWLED = 0;
		printf("Enter entry number: ");
		entry_number = (uint16) long_serial_input();
		if(entry_number <= num_entries)
		{
			ent_clus = Read_Dir_Entry(current_directory, entry_number, block_data_1);
			if(ent_clus & directory_bit)
			{
				ent_clus &= 0x0FFFFFFF;
				current_directory = First_Sector(ent_clus);
			}
			else
			{
				Open_File(ent_clus, block_data_1);
			}
		}
	}
}