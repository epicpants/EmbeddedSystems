/*
 * Authors: Jon Eftink and Tyler Ryan
 * File: main.c
 * Brief: Entry point for the program. Runs memory block printing to UART 
 * and runs LED flashing routine, modifiable by switches and displayed on LCD
 */
#include <main.h>
#include <switch.h>
#include <LED.h>
#include <LCD.h>
#include <IntToString.h>
#include <memory_test.h>
#include <print.h>
#include <UART.h>
#include <stdio.h>


void main(void)
{
	uint8 current_state, new_state;
	uint16 speed_ms = 1000;
	bit DIRECTION = LEFT;
	uint8 SW_States[] = {SWITCH_NOT_PRESSED, SWITCH_NOT_PRESSED, SWITCH_NOT_PRESSED, SWITCH_NOT_PRESSED};
	uint8 SW_Held[] = {NO, NO, NO, NO};
	uint8 dir_str_basic[] = "Dir: ";
	uint8 dir_str_left[] = "Left";
	uint8 dir_str_right[] = "Right";
	uint8 ms_str_basic[] = "ms";
	uint8 speed_str[6];
	uint8 num_bytes;
	uint16 counter = 0;
	uint8 * generic_p;
	uint8 UART_num_bytes = 64;
	
	UART_INIT();
	LCD_Init();
	current_state = 0;

	generic_p = code_memory_init();
    print_memory_block(UART_num_bytes, generic_p);
    generic_p = xdata_memory_init();
    print_memory_block(UART_num_bytes, generic_p);
	
	while(1)
	{
		
		CHECK_SWITCH_STATES(SW_States, SW_Held);
		DIRECTION = CHECK_LED_DIRECTION(SW_States, SW_Held, DIRECTION);
		speed_ms = CHECK_LED_SPEED(SW_States, SW_Held, speed_ms);
		if(counter % speed_ms == 0)
		{
			new_state = LED_Change_State(current_state, DIRECTION);
			current_state = new_state;
			counter = 0;
		}	
		num_bytes = IntToString(speed_str, speed_ms);
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
		Delay_ms(STEP_SIZE);
		counter += STEP_SIZE;
	}
}