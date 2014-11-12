#ifndef _Serial_In_H
#define _Serial_In_H

#include "main.h"
#include "UART.h"

#define BS (0x08)
#define space (0x20)
#define DEL (0x7F)
#define LF (0x0A)
#define CR (0x0D)
// ------ Public function prototypes -------------------------------

uint32 long_serial_input(void);



#endif