#ifndef _SPI_H
#define _SPI_H

#include <Main.h>

#define INIT_OK (0)
#define INIT_ERR (1)
#define TIMEOUT_ERROR (0x8000)
uint8 SPI_Master_Init(uint32 clock_rate);
uint16 SPI_Transfer(uint8 send_value);

#endif