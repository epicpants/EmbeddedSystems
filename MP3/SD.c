/*
 * Authors: Jon Eftink and Tyler Ryan
 * File: SD.c
 * Brief: function definitions of SD Card initialization and memory block reading
 */
#include "SD.h"
#include "SPI.h"
#include "PORT.H"
#include <stdio.h>

/***********************************************************************
DESC:  Initializes SD Card
INPUT: None
RETURNS: one byte error status (NO_ERRORS, VOLTAGE_NOT_SUPPORTED,
		CARD_NOT_SUPPORTED, TIMEOUT_ERROR, ILLEGAL_COMMAND, SPI_ERROR)
CAUTION: 
************************************************************************/
uint8 SD_Card_init()
{
	uint8 index;
	uint8 errorFlag = 0;
	uint8 errorStatus = 0;
	uint8 array_out[6];
	uint16 timeout;

	for(index = 0; index < 5; index++) //Initialize the array_out values.
	{
		array_out[index] = 0x00;
	}
	
	
	nCS0 = 1;
	for(index = 0; index < 74; index++) // Send 74 clock cycles for SD internal operations.
	{
		SPI_Transfer(0xFF);
	}

	printf("Sending CMD0...\n");   //Sending CMD0
	nCS0 = 0;
	errorFlag = send_command(CMD0, CMD0_ARGUMENT); 

	if(errorFlag != NO_ERRORS)
	{
		errorStatus = errorFlag;
	}

	if(errorStatus == NO_ERRORS)	//Receiving CMD0 response
	{
		errorFlag = receive_response(SIZE_OF_R1, array_out);
	}

	nCS0 = 1; 

	if(errorFlag != NO_ERRORS) // if there was an error in receive response:
	{
		errorStatus = errorFlag;
	}
	else if(array_out[0] == IDLE_STATE && errorStatus == NO_ERRORS) // Got a good response from SD card, send CMD8:
	{
		printf("Received \"Idle state response.\"(good)\n");
		printf("Sending CMD8...\n");
		array_out[0] = 0;
		nCS0 = 0;
		errorFlag = send_command(CMD8, CMD8_ARGUMENT);

		if(errorFlag != NO_ERRORS)
		{
			errorStatus = errorFlag;
		}

		if(errorStatus == NO_ERRORS)
		{
			errorFlag = receive_response(SIZE_OF_R7, array_out);
			nCS0 = 1;
			if(errorFlag != NO_ERRORS) // Error in receive_response:
			{
				errorStatus = errorFlag;
			}
			else if(array_out[0] == IDLE_STATE) //Received good response:
			{
				printf("Received \"Idle state response.\"(good)\n");
				if(array_out[4] != CMD8_CHECK_PATTERN)
				{
					printf("Received BAD_CRC\n");
					for(index = 0; index < 6; index++)
					{
						printf("%2.2bx ", array_out[index]);
					}
					printf("\n");
					errorStatus = BAD_CRC;
				}

			}
			else if(array_out[0] == ILLEGAL_COMMAND) // This may be a version 1 card, or may not be an SD card at all.
			{
				printf("Illegal Command.\nVersion 1 cards are not supported currently.\n");
				errorStatus = array_out[0];
			}
			else
			{
				errorStatus = array_out[0];
			}

			for(index = 0; index < SIZE_OF_R7; index++) // Reset array_out for use later.
			{
				array_out[index] = 0;
			}
				
		}
		else if(errorStatus == NO_ERRORS)//Error that is not illegal command:
		{
			errorStatus = array_out[0];
		}
		nCS0 = 1;
	}

	//Sent CMD0 and CMD8, errorStatus is non-zero if there have been any errors so far.
	if(errorStatus == NO_ERRORS) //Send CMD58:
	{
		printf("Sending CMD58...\n");
		nCS0 = 0;
		errorFlag = send_command(CMD58, 0x00000000);
		
		if(errorFlag != NO_ERRORS)
		{
			errorStatus = errorFlag;
		}
		else
		{
			errorFlag = receive_response(SIZE_OF_R3, array_out);
			nCS0 = 1;
			
			if(errorFlag != NO_ERRORS)
			{
				errorStatus = errorFlag;
			}
			else if(array_out[0] != IDLE_STATE)
			{
				errorStatus = errorFlag;
			}
			else if(!(array_out[2] & 0x78)) //We should support voltages of 3.1V-3.5V.
			{								   //If the SD card supports these at least one of bits 19-22 inclusive will be set.
				errorStatus = VOLTAGE_NOT_SUPPORTED;
			}				
		}
		nCS0 = 1; //There is a chance setting above will not happen.
		for(index = 0; index < SIZE_OF_R3; index++)
			array_out[index] = 0;
	}

	if(errorStatus == NO_ERRORS) // Send ACMD41:
	{
		printf("Sending ACMD41...\n");
		timeout = 0;
		nCS0 = 0;
		do
		{

			errorFlag = send_command(CMD55, CMD55_ARGUMENT); //Sending CMD55
			if(errorFlag != NO_ERRORS)
			{
				errorStatus = errorFlag;
			}
			else
			{
				array_out[0] = 0;
				errorFlag = receive_response(SIZE_OF_R1, array_out);

				if(errorFlag != NO_ERRORS)
				{
					errorStatus = errorFlag;
				}
				else
				{
					errorFlag = send_command(CMD41, CMD41_ARGUMENT_V2);	//Assuming this is a version 2.x card.

					if(errorFlag != NO_ERRORS)
					{
						errorStatus = errorFlag;
					}
					else
					{
						array_out[0] = 0;
						errorFlag = receive_response(SIZE_OF_R1, array_out);
						if(errorFlag != NO_ERRORS)
						{
							errorStatus = errorFlag;
						}
					}
				}
			}
			timeout++;
		}while((array_out[0] == IDLE_STATE) && (timeout != 0) && (errorStatus == NO_ERRORS));
		
		nCS0 = 1;
		if(array_out[0] != 0)
		{
			errorStatus = array_out[0];
		}
		else if(timeout == 0)
		{
			errorStatus = TIMEOUT_ERROR;
		}
		
		if(errorStatus == NO_ERRORS) //This must be a version 2.x card, so send CMD58 again:
		{
			printf("Sending CMD58 again...\n");
			nCS0 = 0;
			errorFlag = send_command(CMD58, 0x00000000);
			
			if(errorFlag != NO_ERRORS)
			{
				errorStatus = errorFlag;
			}
			else
			{
				errorFlag = receive_response(SIZE_OF_R3, array_out);
				nCS0 = 1;
				
				if(errorFlag != NO_ERRORS)
				{
					errorStatus = errorFlag;
				}
				else if(array_out[0] != 0) //Should not be idle or have any errors.
				{
					errorStatus = errorFlag;
				}
				
				else if((array_out[1] & 0x80) && (array_out[1] & 0x40)) //If OCR and CCS bits are set, this is an SDHC card.
				{
					//SD_Card_type = SDHC;
				}
				else if((array_out[1] & 0x80)) //If only OCR bit is set, this is an SD card
				{
					//SD_Card_type = SD;
					errorStatus = CARD_NOT_SUPPORTED;
				}
																	   
				
			}
			nCS0 = 1; //There is a chance setting above will not happen.
			for(index = 0; index < SIZE_OF_R3; index++)
				array_out[index] = 0;
		}
	}
	printf("Exiting SD_Card_init, errorStatus is %2.2bx.\n", errorStatus);
	return errorStatus;
}

/***********************************************************************
DESC:  Reads a block of SD Card memory
INPUT: number of bytes to be read, array of bytes to put read block in
RETURNS: one byte error status (NO_ERRORS, TIMEOUT_ERROR, BAD_VALUE)
		populates array with number_of_bytes bytes of SD Card memory
CAUTION: Current use of receive response without pointer may not work correctly.
************************************************************************/
uint8 read_block(uint16 number_of_bytes, uint8 xdata * array)
{
	uint16 idata index;
	uint16 idata timeout;
	uint16 idata SPI_Return;
	uint8 errorStatus;
	uint8 errorFlag;
	uint8 dat;

	errorStatus = 0;
	errorFlag = 0;
	
	//printf("Waiting for R1 response...\n");
	errorFlag = receive_response(SIZE_OF_R1, &dat); //Receive R1 response:

	if(errorFlag != NO_ERRORS)
	{
		errorStatus = errorFlag;
	}
	else if(dat != NO_ERRORS)
	{
		errorStatus = dat;
	}

	if(errorStatus == NO_ERRORS)
	{
		//printf("Got a good R1 response, now waiting for data start token...\n");
		timeout = 0;
		do
		{
			SPI_Return = SPI_Transfer(0xFF);
			errorFlag = (SPI_Return >> 8);
			dat = (uint8)(SPI_Return & 0x00FF);
			timeout++;
		} while((timeout != 0) && (dat == 0xFF) && (errorFlag == NO_ERRORS));

		if(timeout == 0) //Error handling:
		{
			errorStatus = TIMEOUT_ERROR;
		}
		else if(errorFlag != NO_ERRORS)
		{
			errorStatus = errorFlag;
		}
		else if((dat & 0xF0) == 0x00)
		{
			errorStatus = dat;
		}
		else if(dat != DATA_START_TOKEN) 
		{
			errorStatus = BAD_VALUE;
		}
		else // If we land here we got the data start token and can proceed with reading the data:
		{
			//printf("Got the data start token, now reading in data.\n");
			for(index = 0; (index < number_of_bytes) && (errorStatus == NO_ERRORS); index++) //Reading in data:
			{
				SPI_Return = SPI_Transfer(0xFF);
				errorFlag = (SPI_Return >> 8);
				dat = (SPI_Return & 0x00FF);
				if(errorFlag != NO_ERRORS)
				{
					errorStatus = errorFlag;
					//printf("Error received: %2.2bx",errorStatus); 
				}
				else
				{
					array[index] = dat;
				}
			}

			//printf("Received data, now sending three more bytes...\n");
			for(index = 0; index < 3; index++) //Discarding checksum (first two bytes)
			{
				SPI_Return = SPI_Transfer(0xFF);
			}
			  	
		}
	}
	//printf("Exiting read_block, error status is %2.2bx.\n",errorStatus); 
	return errorStatus;
}
