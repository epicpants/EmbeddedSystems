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
	P2 &= LED_patterns[current_state]; // turn on LED
	Delay_ms(100); // may need to adjust this
	P2 |= 0xF0;	// turn off LED
	return current_state;
}

bit CHECK_LED_DIRECTION(uint8 * array_sw_states, uint8 * array_sw_held, bit CURRENT_DIRECTION)
{
	bit RETURN_VALUE = CURRENT_DIRECTION;
	if(array_sw_states[0] == SWITCH_PRESSED)
	{
		// Determine if switch is being held
		if(array_sw_held[0] == NO)
		{
			RETURN_VALUE = RIGHT;
			array_sw_held[0] = YES;
		}
	}
	if(array_sw_states[3] == SWITCH_PRESSED)
	{
		// Determine if switch is being held
		if(array_sw_held[3] == NO)
		{
			RETURN_VALUE = LEFT;
			array_sw_held[3] = YES;
		}
	}
	return RETURN_VALUE;
}

uint8 CHECK_LED_SPEED(uint8 * array_sw_states, uint8 * array_sw_held, uint8 current_speed)
{
	uint8 return_value = current_speed;
	if(array_sw_states[1] == SWITCH_PRESSED)
	{
		// Determine if switch is being held
		if(array_sw_held[1] == NO)
		{
			return_value = current_speed + STEP_SIZE;
			if(return_value > MAX_SPEED)
			{
				return_value = MAX_SPEED;
			}
			array_sw_held[1] = YES;
		}
	}
	if(array_sw_states[2] == SWITCH_PRESSED)
	{
		// Determine if switch is being held
		if(array_sw_held[2] == NO)
		{
			return_value = current_speed - STEP_SIZE;
			if(return_value < MIN_SPEED)
			{
				return_value = MIN_SPEED;
			}
			array_sw_held[2] = YES;
		}
	}
	return return_value;
}