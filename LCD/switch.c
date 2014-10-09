/*
 * Authors: Jon Eftink and Tyler Ryan
 * File: switch.c
 * Brief: Switch reading functions
 */
#include <switch.h>

sbit SWITCH_PIN1 = P2^0;
sbit SWITCH_PIN2 = P2^1;
sbit SWITCH_PIN3 = P2^2;
sbit SWITCH_PIN4 = P2^3;

/***********************************************************************
DESC:  Read state of switch 1
INPUT: desired debounce time
RETURNS: bit indicating whether switch 1 is pressed
CAUTION: 
************************************************************************/
bit SWITCH1_GET_INPUT(uint8 DEBOUNCE_PERIOD)
{
	bit RETURN_VALUE = SWITCH_NOT_PRESSED;
	
	SWITCH_PIN1 = 1;
	if(SWITCH_PIN1 == 0) // if pressed
	{
		Delay_ms(DEBOUNCE_PERIOD);
		if(SWITCH_PIN1 == 0)
		{
			RETURN_VALUE = SWITCH_PRESSED;
		}
	}
	return RETURN_VALUE;
}

/***********************************************************************
DESC:  Read state of switch 2
INPUT: desired debounce time
RETURNS: bit indicating whether switch 2 is pressed
CAUTION: 
************************************************************************/
bit SWITCH2_GET_INPUT(uint8 DEBOUNCE_PERIOD)
{
	bit RETURN_VALUE = SWITCH_NOT_PRESSED;
	
	SWITCH_PIN2 = 1;
	if(SWITCH_PIN2 == 0) // if pressed
	{
		Delay_ms(DEBOUNCE_PERIOD);
		if(SWITCH_PIN2 == 0)
		{
			RETURN_VALUE = SWITCH_PRESSED;
		}
	}
	return RETURN_VALUE;
}

/***********************************************************************
DESC:  Read state of switch 3
INPUT: desired debounce time
RETURNS: bit indicating whether switch 3 is pressed
CAUTION: 
************************************************************************/
bit SWITCH3_GET_INPUT(uint8 DEBOUNCE_PERIOD)
{
	bit RETURN_VALUE = SWITCH_NOT_PRESSED;
	
	SWITCH_PIN3 = 1;
	if(SWITCH_PIN3 == 0) // if pressed
	{
		Delay_ms(DEBOUNCE_PERIOD);
		if(SWITCH_PIN3 == 0)
		{
			RETURN_VALUE = SWITCH_PRESSED;
		}
	}
	return RETURN_VALUE;
}

/***********************************************************************
DESC:  Read state of switch 4
INPUT: desired debounce time
RETURNS: bit indicating whether switch 4 is pressed
CAUTION: 
************************************************************************/
bit SWITCH4_GET_INPUT(uint8 DEBOUNCE_PERIOD)
{
	bit RETURN_VALUE = SWITCH_NOT_PRESSED;
	
	SWITCH_PIN4 = 1;
	if(SWITCH_PIN4 == 0) // if pressed
	{
		Delay_ms(DEBOUNCE_PERIOD);
		if(SWITCH_PIN4 == 0)
		{
			RETURN_VALUE = SWITCH_PRESSED;
		}
	}
	return RETURN_VALUE;
}

/***********************************************************************
DESC:  Checks switch states and hold statuses
INPUT: current state of all switches, hold status of all switches
RETURNS: updated state of all switches, updated hold status of all switches
CAUTION: 
************************************************************************/
void CHECK_SWITCH_STATES(uint8 * array_sw_states, uint8 * array_sw_held)
{
	array_sw_states[0] = SWITCH1_GET_INPUT(DEBOUNCE_TIME);
	if(array_sw_states[0] == SWITCH_NOT_PRESSED)
	{
		array_sw_held[0] = NO;
	}
	array_sw_states[1] = SWITCH2_GET_INPUT(DEBOUNCE_TIME);
	if(array_sw_states[1] == SWITCH_NOT_PRESSED)
	{
		array_sw_held[1] = NO;
	}
	array_sw_states[2] = SWITCH3_GET_INPUT(DEBOUNCE_TIME);
	if(array_sw_states[2] == SWITCH_NOT_PRESSED)
	{
		array_sw_held[2] = NO;
	}
	array_sw_states[3] = SWITCH4_GET_INPUT(DEBOUNCE_TIME);
	if(array_sw_states[3] == SWITCH_NOT_PRESSED)
	{
		array_sw_held[3] = NO;
	}
}