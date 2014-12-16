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

#define EOF (0x0FFFFFFF)
#define ACTIVE (0) // Are these switched?
#define INACTIVE (1) 


void Play_MP3_File(uint32 first_cluster_of_song);
void Timer_2_Interrupt_Init(uint8 tick_ms);
uint8 send_command_ISR(uint8 command, uint32 idata argument);
uint8 read_block_ISR(uint16 number_of_bytes, uint8 xdata * array);
uint32 Find_Next_Clus_ISR(uint32 Cluster_num, uint8 xdata * array_name);
uint32 First_Sector_ISR(uint32 Cluster_num);
//void Timer2_ISR(void) interrupt INTERRUPT_Timer_2_Overflow;
void Timeout_Start(void);

extern uint8 idata SecPerClus_g;
extern uint8 xdata block_data_1[512];
extern uint8 xdata block_data_2[512];


#endif
