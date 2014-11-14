/*
 * Authors: Jon Eftink and Tyler Ryan
 * File: SPI.h
 * Brief: function prototypes for SPI initialization and communication
 */
#ifndef _SPI_H
#define _SPI_H

#include "Main.h"

#define INIT_OK (0)
#define INIT_ERR (1)
#define TIMEOUT_ERROR (0x8000)
#define NO_ERRORS (0)
#define ILLEGAL_COMMAND (4)
#define SPI_ERROR (0x87)
uint8 SPI_Master_Init(uint32 clock_rate);
uint16 SPI_Transfer(uint8 send_value);
uint8 send_command(uint8 command, uint32 argument);
uint8 receive_response(uint8 number_of_bytes, uint8 * array_name);

#endif