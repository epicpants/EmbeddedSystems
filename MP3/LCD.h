/*
 * Authors: Jon Eftink and Tyler Ryan
 * File: LCD.h
 * Brief: Function prototypes for basic LCD functions 
 */
#ifndef _LCD_H
#define _LCD_H
#include <Main.h>
#include <PORT.h>

#define NUL (0x00)
#define COMMAND (0)
#define DDRAM (1)
#define FUNCTION_SET (0x38) // set 8-bit interface, 2 line mode, 5x8 font
#define DISPLAY_CONTROL (0x0F) // set display on, cursor on, cursor blink on
#define DISPLAY_CLEAR (0x01) // replace all characters in display with spaces
#define ENTRY_MODE_SET (0x04) // set auto increment DDRAM address, shift cursor

void LCD_Init(void);
void LCD_Write(bit REG_SELECT, uint8 LCD_Data);
void LCD_Print(uint8 num_bytes, uint8 * array_name);
#endif