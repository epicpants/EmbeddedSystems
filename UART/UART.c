#include <UART.h>

void UART_INIT(void)
{
	SCON = 0x50;
	PCON |= (SMOD1 << 7);
	PCON &= ((SMOD1 << 7) | 0x3F);
	BDRCON = 0;
	BRL = BRL_VALUE;
	BDRCON = (0x1C | (SPD << 1));
	ES = 0;
	TI = 1;
}