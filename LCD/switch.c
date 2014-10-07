#include <switch.h>

sbit SWITCH_PIN1 = P2^0;
sbit SWITCH_PIN2 = P2^1;
sbit SWITCH_PIN3 = P2^2;
sbit SWITCH_PIN4 = P2^3;

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