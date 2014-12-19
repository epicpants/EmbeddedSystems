

#ifndef _MP3_H
#define _MP3_H

#include "PORT.h"
#include "Main.h"


void Play_MP3_File(uint32 first_sng_clus);

extern uint8 idata SecPerClus_g;
extern uint8 xdata block_data_1[512];
extern uint8 xdata block_data_2[512];

typedef enum 
{
	PLAYING,
	END_OF_SONG,
	STOP
} player_status;

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

#define EOFILE (0x0FFFFFFF)
#define ACTIVE   (0) // Are these switched?
#define INACTIVE (1) 

#endif
