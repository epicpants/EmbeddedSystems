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
	nCS0 = 1;
	num_sectors_g++;
	
	// Load Buffer 2
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
			break;
		}
			
	}
}

void Timer_2_Interrupt_Init(uint8 tick_ms)
{
	uint16 Timer_2_Reload;
	uint8 Timer_2_Reload_H, Timer_2_Reload_L;
	
	TR2 = 0;
	Timer_2_Reload = (uint16) ((65536) - ((OSC_FREQ * tick_ms) / (OSC_PER_INST * 1000UL)));
	Timer_2_Reload_H = (uint8) (Timer_2_Reload >> 8);
	Timer_2_Reload_L = (uint8) (Timer_2_Reload & 0x00FF);
	TH2 = Timer_2_Reload_H;
	RCAP2H = Timer_2_Reload_H;
	TL2 = Timer_2_Reload_L;
	RCAP2L = Timer_2_Reload_L;
	EA = 1; // Global interrupt enable
	ET2 = 1;  // Enable the interrupt
	TR2 = 1; // Start timer

}

void Timer2_ISR(void) interrupt INTERRUPT_Timer_2_Overflow
{
	TF2 = 0;
	Timeout_Start(); // Using Timer 0

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
					while((DATA_REQ == ACTIVE) && (index1_g < 512) && (TF0 == 0))
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
					while((DATA_REQ == ACTIVE) && (index2_g < 512) && (TF0 == 0))
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


//This function will start timer0 for a 10ms interval.
void Timeout_Start(void)
{
	uint16 idata reload_val;
	TR0 = 0;
	TMOD &= 0xF0; // Clear timer0 setup.
	TMOD |= 0x01; // Set timer 0 to 16 bit mode.
	ET0 = 0; //Not using timer interrupt.
	reload_val = (uint16)(65536 - 10 * (OSC_FREQ / (OSC_PER_INST * 1000UL)));
	TH0 = (uint8)(reload_val >> 8);
	TL0 = (uint8)(reload_val & 0x00FF);
	TF0 = 0; //Clear overflow flag.
	TR0 = 1; //Start timer.
}
