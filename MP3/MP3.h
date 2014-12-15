#ifndef MP3_H
#define MP3_H
#include "PORT.h"
#include "Main.h"
#include "delay.h"


typedef enum  
{
	FIND_CLUSTER_1,
	FIND_CLUSTER_2,
	LOAD_BUFFER_1,
	LOAD_BUFFER_2,
	DATA_SEND_1,
	DATA_SEND_2,
	DATA_IDLE_1,
	DATA_IDLE_2
} player_state;

typedef enum 
{
	PLAYING,
	END_OF_SONG,
	STOP
} player_status;



void Play_MP3_File(uint32 first_cluster_of_song);
void Timer_2_Interrupt_Init(uint8 tick_ms);
//void Timer2_ISR(void) interrupt INTERRUPT_Timer_2_Overflow;


#endif
