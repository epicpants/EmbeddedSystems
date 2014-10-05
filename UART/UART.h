/*
 * Authors: Jon Eftink and Tyler Ryan
 * File: UART.h
 * Brief: function prototype for UART initialization along with 
 * UART constants and baud rate calculations
 */
#include <Main.h>
#include <PORT.h>

#ifndef _UART_H
#define _UART_H

#define SMOD1 (0)
#define SPD (1)
#define BAUD_RATE (9600UL)
#define BRL_VALUE ((uint8)(256-((1+(5*SPD))*(1+(1*SMOD1))*OSC_FREQ/(32*OSC_PER_INST*BAUD_RATE))))

// Public function prototypes
void UART_INIT(void);

#endif