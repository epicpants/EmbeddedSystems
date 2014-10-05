#include <LED.h>

uint8 LED_patterns[] = {0xEF, 0xDF, 0xBF, 0x7F};   // may want to come back to this

uint8 LED_Change_State(uint8 current_state, bit DIRECTION)
{
	if(DIRECTION == LEFT)
	{
		current_state = (current_state + 1) % 4;
	}
	else
	{
		current_state = ((current_state + 4) - 1) % 4;	 // + 4 prevents underflow
	}
	P2 = LED_patterns[current_state];
	return current_state;
}