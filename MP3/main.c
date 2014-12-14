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
#include "I2C.h"
#include "button.h"

#define STA013 (0x43)
#define TIMEOUT_VAL (50)

extern uint8 code CONFIG;
extern uint8 code CONFIG2;

void main(void)
{
	uint8 xdata block_data_1[512];
	uint8 xdata block_data_2[512];
	uint32 current_directory, ent_clus;
	uint8 error_val;
	uint16 num_entries, entry_number, i;

	AUXR = 0x0C; // Allows use of 1 KB of RAM
	UART_INIT();
	printf("Initializing SPI Master to 400 KHz...\n");
	SPI_Master_Init(400000UL);
	printf("Initializing SD Card...\n");
	SD_Card_init();
	printf("Initializing SPI Master to 5 MHz\n");
	error_val = SPI_Master_Init(5000000UL);
	printf("SPI Init error_val: %2.2BX\n", error_val);
	printf("Mounting drive...\n");
	mount_drive(block_data_1);
	current_directory = FirstRootDirSec_g;
	
	GREENLED = 0;
	while(1)
	{
		num_entries = Print_Directory(current_directory, block_data_1);
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