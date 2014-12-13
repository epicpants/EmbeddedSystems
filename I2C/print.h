/*
 * Authors: Jon Eftink and Tyler Ryan
 * File: print.h
 * Brief: function prototypes and constants for printing lines and blocks
 * of memory via UART
 */
#include "main.h"

#ifndef _PRINT_H
#define _PRINT_H

#ifndef CR
#define CR (0x0D) // Carriage return in hex
#endif
#ifndef LF
#define LF (0x0A) // Line feed in hex
#endif
#ifndef ESC
#define ESC (0x1B)
#endif

void print_memory_block(uint16 num_bytes, uint8 * address_p);
void print_line(uint8 * array_in_p);

#endif