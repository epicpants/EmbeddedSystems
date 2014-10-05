#include <main.h>
#include <switch.h>
#include <LED.h>



void main(void)
{
	uint8 current_state, new_state;
	uint16 speed_ms;
	bit DIRECTION;
	
	current_state = 0;
	DIRECTION = LEFT;
	speed_ms = 1000;
	while(1)
	{
		new_state = LED_Change_State(current_state, DIRECTION);
		current_state = new_state;
		Delay_ms(speed_ms);
	}
}