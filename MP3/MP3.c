#include "main.h"
#include "MP3.h"
#include "Directory_Functions.h"

/*
uint32 FirstDataSec_g, StartofFAT_g, FirstRootDirSec_g, RootDirSecs_g;
uint16 BytesPerSec_g;
uint8 SDtype_g, SecPerClus_g, FATtype_g, BytesPerSecShift_g,FATshift_g;
*/

/*				Globals					*/
uint32 cluster_g, sector_g;
uint16 index1_g, index2_g;
uint8 num_sectors_g;
player_state player_state_g;
player_status player_status_g;

void Play_MP3_File(uint32 first_cluster_of_song)
{
	// Something goes here
	cluster_g = first_cluster_of_song;
	sector_g = First_Sector(first_cluster_of_song);
	num_sectors_g = 0;
	// Load Buffer 1
	// Load Buffer 2
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
	//Timeout_start(); // Using Timer 0
	// Stuff goes here ?


	switch(player_state_g)
	{
		case DATA_SEND_1:
			break;
		case DATA_SEND_2:
			break;
		case LOAD_BUFFER_1:
    		break;
		case LOAD_BUFFER_2:
			break;
		case FIND_CLUSTER_1:
			break;
		case FIND_CLUSTER_2:
			break;
		case DATA_IDLE_1:
			break;
		case DATA_IDLE_2:
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

