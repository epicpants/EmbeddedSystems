#include <delay.h>

void Delay_ms(uint32 timeout_val_ms)
{
	uint32 millisec;
	TMOD &= 0xF0; // clear T0 bits
	TMOD |= 0x01;
	ET0 = 0; // turn off interrupts
	for(millisec = 0; millisec < timeout_val_ms; millisec++)
	{
		TH0 = PRELOADH1;
		TL0 = PRELOADL1;
		TF0 = 0; // clears overflow flag
		TR0 = 1; // starts the timer
		while(TF0 == 0); // wait for overflow
		TR0 = 0;
	}
}