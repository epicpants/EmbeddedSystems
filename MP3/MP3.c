#include "main.h"
#include "MP3.h"
#include "Directory_Functions.h"
#include "SPI.h"
#include "SD.h"

#define RELOAD_10MS (65536 - 10 * (OSC_FREQ / (OSC_PER_INST * 1000)))
#define TIMEOUT_RELOAD_H ( RELOAD_10MS >> 8)
#define TIMEOUT_RELOAD_L (0x00FF & RELOAD_10MS)

/*				Globals					*/
uint32 idata cluster_g, sector_g;
uint16 idata index1_g, index2_g;
uint8 num_sectors_g;
player_state idata player_state_g;
player_status idata player_status_g;

void Play_MP3_File(uint32 first_cluster_of_song)
{
	// Something goes here
	cluster_g = first_cluster_of_song;
	sector_g = First_Sector(first_cluster_of_song);
	num_sectors_g = 0;

	// Load Buffer 1
	SPI_Transfer_End();
	BIT_EN = 0;
	nCS0 = 0;
	send_command(CMD17, (sector_g + num_sectors_g));
	read_block(512, block_data_1);
	nCS0 = 1;
	num_sectors_g++;
	//index1_g = 0;

	// Load Buffer 2
	SPI_Transfer_End();
	BIT_EN = 0;
	nCS0 = 0;
	send_command(CMD17, (sector_g + num_sectors_g));
	read_block(512, block_data_2);
	nCS0 = 1;
	num_sectors_g++;

	// Prepare to begin playing
	index1_g = 0;
	index2_g = 0;
	player_state_g = DATA_IDLE_1;
	player_status_g = PLAYING;
	Timer_2_Interrupt_Init(11);
	
	while(1)
	{
		if(player_status_g == STOP)
		{
			TR2 = 0; // Stop timer 2
			break;
		}
		PCON |= 0x01; // Go to sleep
	}
}

void Timer_2_Interrupt_Init(uint8 tick_ms)
{
	uint16 Timer_2_Reload;
	uint8 Timer_2_Reload_H, Timer_2_Reload_L;
	
	Timer_2_Reload = (uint16) ((65536) - ((OSC_FREQ * tick_ms) / (OSC_PER_INST * 1000UL)));
	Timer_2_Reload_H = (uint8) (Timer_2_Reload >> 8);
	Timer_2_Reload_L = (uint8) (Timer_2_Reload & 0xFF);
	TH2 = Timer_2_Reload_H;
	RCAP2H = Timer_2_Reload_H;
	TL2 = Timer_2_Reload_L;
	RCAP2L = Timer_2_Reload_L;
	ET2 = 1;  // Enable the interrupt
	TR2 = 1; // Start timer
	EA = 1; // Global interrupt enable
}

void Timer2_ISR(void) interrupt INTERRUPT_Timer_2_Overflow
{
	TF2 = 0;
	Timeout_Start(); // Using Timer 0
	// Stuff goes here ?


	switch(player_state_g)
	{
		case DATA_SEND_1:
			if(DATA_REQ == 0) //if DATA_REQ is inactive:
			{
				SPI_Transfer_End();
				BIT_EN = 0;
				if(index2_g >= 512)
				{
					if(num_sectors_g >= SecPerClus_g)
						player_state_g = FIND_CLUSTER_2;
					else
						player_state_g = LOAD_BUFFER_2;
				}
				else
				{
					player_state_g = DATA_IDLE_1;
				}
			}
			else
			{
				if(index1_g >= 512)
				{
					if(index2_g >= 512) //If both buffers are empty:
					{
						if(num_sectors_g >= SecPerClus_g)
							player_state_g = FIND_CLUSTER_2;
						else
							player_state_g = LOAD_BUFFER_2;
					}
					else
					{
						player_state_g = DATA_SEND_2;
					}
				}
				else //Data req == ACTIVE && buffer1 has data to send && have not timed out:
				{    //transfer data until time is up or data_req goes inactive:
					while((TF0 == 0) && (DATA_REQ == 1) && (index1_g < 512))
					{
						SPI_Transfer_Fast(block_data_1[index1_g]);
						index1_g++;
					}
					SPI_Transfer_End();
				}
			}
			break;
		case DATA_SEND_2:
			if(DATA_REQ == 0) //if DATA_REQ is inactive
			{
				SPI_Transfer_End();
				BIT_EN = 0;
				if(index1_g == 512)
				{
					if(num_sectors_g >= SecPerClus_g)
						player_state_g = FIND_CLUSTER_1;
					else
						player_state_g = LOAD_BUFFER_1;
				}
				else
				{
					player_state_g = DATA_IDLE_2;
				}
			}
			else
			{
				if(index2_g >= 512)
				{
					if(index1_g >= 512)
					{
						if(num_sectors_g >= SecPerClus_g)
							player_state_g = FIND_CLUSTER_1;
						else	
							player_state_g = LOAD_BUFFER_1;	
					}
					else
					{
						player_state_g = DATA_SEND_1;
					}
				}
				else //Data req == ACTIVE && buffer1 has data to send && have not timed out:
				{    //transfer data until time is up or data_req goes inactive:
					while((TF0 == 0) && (DATA_REQ == 1) && (index2_g < 512))
					{
						SPI_Transfer_Fast(block_data_2[index2_g]);
						index2_g++;
					}
					SPI_Transfer_End();
				}
			}
			break;
		case LOAD_BUFFER_1:
			SPI_Transfer_End();
			BIT_EN = 0;
			nCS0 = 0;
			send_command_ISR(CMD17, (sector_g + num_sectors_g));
			read_block_ISR(512, block_data_1);
			nCS0 = 1;
			num_sectors_g++;
			index1_g = 0;
			player_state_g = DATA_IDLE_2;
    		break;
		case LOAD_BUFFER_2:
			SPI_Transfer_End();
			BIT_EN = 0;
			nCS0 = 0;
			send_command_ISR(CMD17, (sector_g + num_sectors_g));
			read_block_ISR(512, block_data_2);
			nCS0 = 1;
			num_sectors_g++;
			index2_g = 0;
			player_state_g = DATA_IDLE_1;
			break;
		case FIND_CLUSTER_1:
			SPI_Transfer_End();
			BIT_EN = 0;
			cluster_g = Find_Next_Clus_ISR(cluster_g, block_data_1);
			if(cluster_g != EOF)
			{
				sector_g = First_Sector_ISR(cluster_g);
				num_sectors_g = 0;
				player_state_g = DATA_IDLE_2;
			}
			else
			{
				player_status_g = END_OF_SONG;
				player_state_g = DATA_IDLE_2;
			}
			break;
		case FIND_CLUSTER_2:
			SPI_Transfer_End();
			BIT_EN = 0;
			cluster_g = Find_Next_Clus_ISR(cluster_g, block_data_2);
			if(cluster_g != EOF)
			{
				sector_g = First_Sector_ISR(cluster_g);
				num_sectors_g = 0;
				player_state_g = DATA_IDLE_1;
			}
			else
			{
				player_status_g = END_OF_SONG;
				player_state_g = DATA_IDLE_1;
			}
			break;
		case DATA_IDLE_1:
			while(DATA_REQ == INACTIVE && TF0 == 0);
			if(TF0 == 0) // Did not timeout, means DATA_REQ is active
			{
				player_state_g = DATA_SEND_1;
			}
			break;
		case DATA_IDLE_2:
			while(DATA_REQ == INACTIVE && TF0 == 0);
			if(TF0 == 0) // Did not timeout, means DATA_REQ is active
			{
				player_state_g = DATA_SEND_2;
			}
			break;
	}


	if((index1_g > 511) && (index2_g > 511))
	{
		if(player_status_g == END_OF_SONG)
		{
			player_status_g = STOP;
		}
	}
}

uint8 send_command_ISR(uint8 command, uint32 idata argument)
{
	uint8 SPI_send, return_val;
	uint16 idata SPI_return;
	return_val = NO_ERRORS;
	if(command < 64) // less than 7 bits
	{
		/********
		 * Byte 1
		 ********/
		SPI_send = command | 0x40;
		SPI_return = SPI_Transfer(SPI_send);

		/********
		 * Byte 2
		 ********/
		if((SPI_return & 0xF000) == 0) // Check errors, 0 means no errors
		{
			SPI_send = argument >> 24; // MSB of argument
			SPI_return = SPI_Transfer(SPI_send);
		}
		else
		{
			return_val = SPI_ERROR;
		}

		/********
		 * Byte 3
		 ********/
		if((return_val == NO_ERRORS) && ((SPI_return & 0xF000) == 0))
		{
			argument = argument & 0x00FFFFFF;
			SPI_send = argument >> 16;
			SPI_return = SPI_Transfer(SPI_send);
		}
		else
		{
			return_val = SPI_ERROR;
		}

		/********
		 * Byte 4
		 ********/
		if((return_val == NO_ERRORS) && ((SPI_return & 0xF000) == 0))
		{
			argument = argument & 0x0000FFFF;
			SPI_send = argument >> 8;
			SPI_return = SPI_Transfer(SPI_send);
		}
		else
		{
			return_val = SPI_ERROR;
		}

		/********
		 * Byte 5
		 ********/
		if((return_val == NO_ERRORS) && ((SPI_return & 0xF000) == 0))
		{
			argument = argument & 0x000000FF;
			SPI_send = argument; // LSB of argument
			SPI_return = SPI_Transfer(SPI_send);
		}
		else
		{
			return_val = SPI_ERROR;
		}

		/********
		 * Byte 6
		 ********/
		if((return_val == NO_ERRORS) && ((SPI_return & 0xF000) == 0))
		{
			if(command == 0) // CMD0
			{
				SPI_send = 0x95; // CMD0 CRC
			}
			else if(command == 8) // CMD8
			{
				SPI_send = 0x87; // CMD8 CRC
			}
			else
			{
				SPI_send = 0x01; // Don't care
			}
			SPI_return = SPI_Transfer(SPI_send);
		}
		else
		{
			return_val = SPI_ERROR;
		}
	}
	else // Command invalid
	{
		return_val = ILLEGAL_COMMAND;
	}
	return return_val;
}

uint8 read_block_ISR(uint16 number_of_bytes, uint8 xdata * array)
{
	uint16 idata index;
	uint16 idata timeout;
	uint16 idata SPI_Return;
	uint8 idata errorStatus;
	uint8 idata errorFlag;
	uint8 idata dat;
	uint8 idata array_out[5];

	errorStatus = 0;
	errorFlag = 0;
	
	//printf("Beginning read_block...\n");
	for(index = 0; index < 5; index++)
	{
		array_out[index] = 0;
	}
	//printf("Waiting for R1 response...\n");
	errorFlag = receive_response(SIZE_OF_R1, array_out); //Receive R1 response:

	if(errorFlag != NO_ERRORS)
	{
		errorStatus = errorFlag;
	}
	else if(array_out[0] != NO_ERRORS)
	{
		errorStatus = array_out[0];
	}

	if(errorStatus == NO_ERRORS)
	{
		//printf("Got a good R1 response, now waiting for data start token...\n");
		timeout = 0;
		do
		{
			SPI_Return = SPI_Transfer(0xFF);
			errorFlag = (SPI_Return >> 8);
			dat = (SPI_Return & 0x00FF);
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

//This function will find the next cluster # of a file.
//The first sector of that cluster is loaded into array_name.
uint32 Find_Next_Clus_ISR(uint32 Cluster_num, uint8 xdata * array_name)
{
	uint32 idata returnVal;
	uint32 idata thisFatSecNum;
	uint32 idata thisFatEntOffset;
	uint32 idata FAToffset;
	uint8 errorVal;
	
	errorVal = 0;
	returnVal = 0;

	thisFatSecNum = StartofFAT_g + ((Cluster_num * FATtype_g) / BytesPerSec_g); //FATtype_g should be: 4 for FAT32, 2 for FAT16.
	
	//Reading in first sector of next cluster:
	nCS0 = 0;
	errorVal = send_command(CMD17, thisFatSecNum);
	if(errorVal != NO_ERRORS)
	{
		//printf("Error sending CMD17 in Find_Next_Clus: %2.2BX.\n", errorVal);
		nCS0 = 1;
		return 0;
	}

	errorVal = read_block(BytesPerSec_g, array_name);
	nCS0 = 1;
	if(errorVal != NO_ERRORS)
	{
		//printf("Error reading block in Find_Next_Clus.\nSector Number %i.\nErrorVal: %2.2BX.\n", thisFatSecNum, errorVal);
		return 0;
	}
	
	FAToffset = Cluster_num * FATtype_g;  //FATtype_g should be 4 for FAT32.
	thisFatEntOffset = FAToffset % BytesPerSec_g;

	if(FATtype_g == FAT32)
	{
		returnVal = read32(thisFatEntOffset, array_name);
		returnVal = (returnVal & 0x0fffffff); // Top 4 bits must be masked because only lower 28 bits are used.
	}
	else if(FATtype_g == FAT16)
	{
		returnVal = (uint32)read16(thisFatEntOffset, array_name);
	}

	return returnVal;
}

//This function will return the first sector in a cluster.
//If cluster_num is 0 it will return FirstRootDirSec_g.
uint32 First_Sector_ISR(uint32 Cluster_num)
{
	uint32 idata returnVal;

	if(Cluster_num == 0)
	{
		returnVal = FirstRootDirSec_g;
	}
	else
	{
		returnVal = ((Cluster_num - 2) * SecPerClus_g) + FirstDataSec_g;
	}

	return returnVal;
}

//This function will start timer0 for a 10ms interval.
void Timeout_Start(void)
{
	TMOD &= 0xF0; // Clear timer0 setup.
	TMOD |= 0x01; // Set timer 0 to 16 bit mode.

	ET0 = 0; //Not using timer interrupt.
	TH0 = TIMEOUT_RELOAD_H;
	TL0 = TIMEOUT_RELOAD_L;
	TF0 = 0; //Clear overflow flag.
	TR0 = 1; //Start timer.
}
