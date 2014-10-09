/*
 * Authors: Jon Eftink and Tyler Ryan
 * File: LED.c
 * Brief: LED operations, such as flashing in a pattern 
 * and adjusting to switch inputs
 */
#include <LED.h>

// Single LED on, from right to left
uint8 LED_patterns[] = {0xEF, 0xDF, 0xBF, 0x7F};

/***********************************************************************
DESC:  Flashes the next LED in the pattern
INPUT: DIRECTION of the pattern, current_state (location) in the pattern
RETURNS: updated state (location) of the pattern
CAUTION: 
************************************************************************/
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
	Delay_ms(100);
	P2 |= 0xF0;	// turn off LED
	return current_state;
}

/***********************************************************************
DESC:  Changes LED pattern direction depending on state of switches
INPUT: state of switches 1 and 4, holding status of switches, current direction
RETURNS: updated direction, hold status of switches 1 and 4
CAUTION: 
************************************************************************/
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

/***********************************************************************
DESC:  Changes speed of LED pattern depending on state of switches
INPUT: state of switches 2 and 3, holding status of switches, current speed
RETURNS: updated speed, hold status of switches 2 and 3
CAUTION: 
************************************************************************/
uint16 CHECK_LED_SPEED(uint8 * array_sw_states, uint8 * array_sw_held, uint16 current_speed)
{
	uint16 return_value = current_speed;
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