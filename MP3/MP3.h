#ifndef MP3_H
#define MP3_H
#include "PORT.h"
#include "Main.h"
#include "delay.h"


typedef uint8 player_state;
enum
{
	FIND_CLUSTER_1,
	FIND_CLUSTER_2,
	LOAD_BUFFER_1,
	LOAD_BUFFER_2,
	DATA_SEND_1,
	DATA_SEND_2,
	DATA_IDLE_1,
	DATA_IDLE_2
}

typedef uint8 player_status;
enum
{
	PLAYING,
	STOP,
	
}






#endif
