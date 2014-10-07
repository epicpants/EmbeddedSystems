#include <LCD.h>
#include <delay.h>

void LCD_Init(void)
{
	Delay_ms(30);
	LCD_Write(COMMAND, FUNCTION_SET);
	Delay_ms(5);
	LCD_Write(COMMAND, FUNCTION_SET);
	Delay_ms(1);
	LCD_Write(COMMAND, FUNCTION_SET);
	Delay_ms(1);
	LCD_Write(COMMAND, DISPLAY_CONTROL);
	Delay_ms(1);
	LCD_Write(COMMAND, DISPLAY_CLEAR);
	Delay_ms(2);
	LCD_Write(COMMAND, ENTRY_MODE_SET);
	Delay_ms(1);
}

void LCD_Write(bit REG_SELECT, uint8 LCD_Data)
{
	RS = REG_SELECT;
	E = 1;
	P0 = LCD_Data;
	E = 0;
	P0 = 0xFF;
}

void LCD_Print(uint8 num_bytes, uint8 * array_name)
{
	uint8 value, index;
	if(num_bytes == 0)
	{
		value = *array_name;
		index = 0;
		while(value != NUL)
		{
			LCD_Write(DDRAM, value);
			index++;
			value = *(array_name + index);
		}
	}
	else
	{
		value = *array_name;
		for(index = 0; index < num_bytes && value != NUL; index++)
		{
			LCD_Write(DDRAM, value);
			value = *(array_name + index + 1);
		}
	}
	Delay_ms(1); // necessary?
}