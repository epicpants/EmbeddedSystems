#include "print.h"
#include "main.h"
#include "MP3.h"
#include "Directory_Functions.h"
#include "SPI.h"
#include "SD.h"

//#define RELOAD_10MS (65536 - 10 * (OSC_FREQ / (OSC_PER_INST * 1000)))
//#define TIMEOUT_RELOAD_H ( RELOAD_10MS >> 8)
//#define TIMEOUT_RELOAD_L (0x00FF & RELOAD_10MS)

/*				Globals					*/
uint32 idata cluster_g, sector_g;
uint16 idata index1_g, index2_g;
uint8 num_sectors_g;
player_state idata player_state_g;
player_status idata player_status_g;

void Play_MP3_File(uint32 first_sng_clus)
{
	// Something goes here
	cluster_g = first_sng_clus;
	sector_g = First_Sector(first_sng_clus);
	num_sectors_g = 0;

	// Load Buffer 1
	BIT_EN = 0;
	nCS0 = 0;

	send_command(CMD17, (sector_g + num_sectors_g));
	read_block(512, block_data_1);
	//print_memory_block(512, block_data_1);
	nCS0 = 1;
	num_sectors_g++;
	
	// Load Buffer 2
	BIT_EN = 0;
	nCS0 = 0;
	send_command(CMD17, (sector_g + num_sectors_g));
	read_block(512, block_data_2);
	//print_memory_block(512, block_data_2);
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
			break;
		}
		else
		{
			switch(player_state_g)
			{
				case DATA_SEND_1:
					GREENLED2 = 0;
					if(DATA_REQ == INACTIVE) //if DATA_REQ is inactive:
					{
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
							BIT_EN = 1;
							while(BIT_EN != 1);
							while((DATA_REQ == ACTIVE) && (index1_g < 512))
							{
			
								SPI_Transfer_Fast(block_data_1[index1_g]);
									//YELLOWLED2 = 0;
								index1_g++;
								
							}
							BIT_EN = 0;
							//SPI_Transfer_End();
						}
					}
					GREENLED2 = 1;
					break;
				case DATA_SEND_2:
					REDLED2 = 0;
					if(DATA_REQ == INACTIVE) //if DATA_REQ is inactive
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
							BIT_EN = 1;
							while(BIT_EN != 1);
							while((DATA_REQ == ACTIVE) && (index2_g < 512))
							{
								SPI_Transfer_Fast(block_data_2[index2_g]);
								index2_g++;
							}
							BIT_EN = 0;
						}
					}
					REDLED2 = 1;
					break;
				case LOAD_BUFFER_1:
					GREENLED = 0;
					nCS0 = 0;
					send_command(CMD17, (sector_g + num_sectors_g));
					read_block_fast(512, block_data_1);
					nCS0 = 1;
					//print_memory_block(512, block_data_1);
					num_sectors_g++;
					index1_g = 0;
					player_state_g = DATA_IDLE_2;
					GREENLED = 1;
		    		break;
				case LOAD_BUFFER_2:
					YELLOWLED = 0;
					nCS0 = 0;
					send_command(CMD17, (sector_g + num_sectors_g));
					read_block_fast(512, block_data_2);
					nCS0 = 1;
					//print_memory_block(512, block_data_2);
					num_sectors_g++;
					index2_g = 0;
					player_state_g = DATA_IDLE_1;
					YELLOWLED = 1;
					break;
				case FIND_CLUSTER_1:
					AMBERLED = 0;
					cluster_g = Find_Next_Clus(cluster_g, block_data_1);
					if(cluster_g != EOFILE)
					{
						sector_g = First_Sector(cluster_g);
						num_sectors_g = 0;
						player_state_g = DATA_IDLE_2;
					}
					else
					{
						player_status_g = END_OF_SONG;
						player_state_g = DATA_IDLE_2;
					}
					AMBERLED = 1;
					break;
				case FIND_CLUSTER_2:
					BLUELED = 0;
					cluster_g = Find_Next_Clus(cluster_g, block_data_2);
					if(cluster_g != EOFILE)
					{
						sector_g = First_Sector(cluster_g);
						num_sectors_g = 0;
						player_state_g = DATA_IDLE_1;
					}
					else
					{
						player_status_g = END_OF_SONG;
						player_state_g = DATA_IDLE_1;
					}
					BLUELED = 1;
					break;
				case DATA_IDLE_1:
					REDLED = 0;
					while(DATA_REQ == INACTIVE);
					player_state_g = DATA_SEND_1;
					REDLED = 1;
					break;
				case DATA_IDLE_2:
					//YELLOWLED2 = 0;
					while(DATA_REQ == INACTIVE);
					player_state_g = DATA_SEND_2;
					//YELLOWLED2 = 1;
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
	}
}
