#include <main.h>
#include <switch.h>
#include <LED.h>
#include <LCD.h>
#include <stdio.h>


void main(void)
{
	uint8 current_state, new_state;
	uint16 speed_ms;
	bit DIRECTION;
	uint8 SW_States[] = {SWITCH_NOT_PRESSED, SWITCH_NOT_PRESSED, SWITCH_NOT_PRESSED, SWITCH_NOT_PRESSED};
	uint8 SW_Held[] = {NO, NO, NO, NO};
	uint8 dir_str_basic[] = "Dir: ";
	uint8 dir_str_left[] = "Left";
	uint8 dir_str_right[] = "Right";
	uint8 ms_str_basic[] = "ms";
	uint8 speed_str[6];
	uint8 num_bytes;
	
	LCD_Init();
	current_state = 0;
	DIRECTION = LEFT;
	speed_ms = 1000;
	while(1)
	{
		CHECK_SWITCH_STATES(SW_States, SW_Held);
		DIRECTION = CHECK_LED_DIRECTION(SW_States, SW_Held, DIRECTION);
		speed_ms = CHECK_LED_SPEED(SW_States, SW_Held, speed_ms);
		new_state = LED_Change_State(current_state, DIRECTION);
		current_state = new_state;
		num_bytes = sprintf(speed_str, "%u", speed_ms);
		LCD_Write(COMMAND, DISPLAY_CLEAR);
		Delay_ms(2);
		LCD_Print(0, dir_str_basic);
		if(DIRECTION == LEFT)
		{
			LCD_Print(0, dir_str_left);
		}
		else // DIRECTION == RIGHT
		{
			LCD_Print(0, dir_str_right);
		}
		LCD_Write(COMMAND, 0xC0); // Set DDRAM address at 0x40 (second line)
		Delay_ms(1);
		LCD_Print(num_bytes, speed_str);
		LCD_Print(0, ms_str_basic);
		Delay_ms(speed_ms);
	}
}