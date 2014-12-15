#include "main.h"
#include "MP3.h"

uint32 FirstDataSec_g, StartofFAT_g, FirstRootDirSec_g, RootDirSecs_g;
uint16 BytesPerSec_g;
uint8 SDtype_g, SecPerClus_g, FATtype_g, BytesPerSecShift_g,FATshift_g;

void Play_MP3_File(uint32 first_cluster_of_song)
{
	
}

void Timer_2_Interrupt_Init(uint8 tick_ms)
{
	uint16 Timer_2_Reload;
	uint8 Timer_2_Reload_H, Timer_2_Reload_L;
	
	Timer_2_Reload = (uint16) ((65536
}

void Timer2_ISR(void) interrupt INTERRUPT_Timer_2_Overflow
{
}

