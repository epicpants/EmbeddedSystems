/*
 * Authors: Jon Eftink and Tyler Ryan
 * File: print.h
 * Brief: function prototypes and constants for printing lines and blocks
 * of memory via UART
 */
#include <main.h>

#ifndef _PRINT_H
#define _PRINT_H

// Carriage return in hex
#define CR (0x0D)
// Line feed in hex
#define LF (0x0A)

void print_memory_block(uint16 num_bytes, uint8 * address_p);
void print_line(uint8 * array_in_p);

#endif