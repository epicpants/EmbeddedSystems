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

#define STA013 (0x43)
#define TIMEOUT_VAL (50)

extern uint8 code CONFIG;
extern uint8 code CONFIG2;

void main(void)
{
	uint8 xdata block_data[512];
	uint8 send[2];
	uint32 current_directory, ent_clus, index;
	uint8 error_val, timeout;
	uint16 num_entries, entry_number, i;
	uint8 * p_out = &CONFIG;

	REDLED = 0;	// Sanity check
	AUXR = 0x0C; // Allows use of 1 KB of RAM
	RESET = 1;
	for(i = 0; i < 512; i++) // Initializing the block_data.
    {
		block_data[i] = 0;
	}
	UART_INIT();
	printf("Initializing SPI Master to 400 KHz...\n");

	SPI_Master_Init(400000UL);
	printf("Initializing SD Card...\n");

	SD_Card_init();
	printf("Initializing SPI Master to 5 MHz\n");

	error_val = SPI_Master_Init(5000000UL);
	printf("SPI Init error_val: %2.2BX\n", error_val);

	printf("Mounting drive...\n");
	mount_drive(block_data);


	I2C_Set_Frequency(I2C_FREQ);		/**

	/*****************
	 Load patches
	 ****************/

	printf("Applying patch 1...\n");
	index = 0;
	error_val = NO_ERRORS;
	do
	{
		send[0] = *(p_out + index);
		index++;
		send[1] = *(p_out + index);
		index++;
		if(send[0] != 0xFF && send[1] != 0xFF);
		{
			error_val = I2C_Write(STA013, 2, send);
		}
	} while(send[0] != 0xFF && send[1] != 0xFF && error_val == NO_ERRORS);
	if(error_val != NO_ERRORS)
	{
		printf("Patch 1 write error = %2.2bX\n", error_val);
	}

	printf("Applying patch 2...\n");
	index = 0;
	p_out = &CONFIG2;
	error_val = NO_ERRORS;
	do
	{
		send[0] = *(p_out + index);
		index++;
		send[1] = *(p_out + index);
		index++;
		if(send[0] != 0xFF && send[1] != 0xFF);
		{
			error_val = I2C_Write(STA013, 2, send);
		}
	} while(send[0] != 0xFF && send[1] != 0xFF && error_val == NO_ERRORS);
	if(error_val != NO_ERRORS)
	{
		printf("Patch 2 write error = %2.2bX\n", error_val);
	}

	/*****************
	 Test I2C
	 ****************/

	AMBERLED = 0;
	timeout = TIMEOUT_VAL;
	error_val = NO_ERRORS;
	send[0] = 0x01;
	do
	{
		error_val = I2C_Write(STA013, 1, send);
		timeout--;
	} while((error_val != NO_ERRORS) && timeout != 0);

	if(timeout != 0)
	{
		timeout = TIMEOUT_VAL;
		do
		{
			error_val = I2C_Read(STA013, 1, send);
			printf("Read error val = %2.2bX\n", error_val);
			timeout--;
		} while((error_val != NO_ERRORS) && timeout != 0);
		if(timeout != 0)
		{
			printf("I2C Test: Value at address 0x01 = %2.2bX\n", send[0]);
		}
		else
		{
			printf("I2C Test read timeout error\n");
		}
	}
	else
	{
		printf("I2C Test write timeout error\n");
	}

	/*****************
	 Directory functions
	 ****************/

	current_directory = FirstRootDirSec_g;
	
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
	}
}