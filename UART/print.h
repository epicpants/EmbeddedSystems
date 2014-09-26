#include <main.h>

#ifndef _PRINT_H
#define _PRINT_H

#define CR (0x0D)
#define LF (0x0A)

void print_memory_block(uint16 num_bytes, uint8 * address_p);
void print_line(uint8 * array_in_p);

#endif