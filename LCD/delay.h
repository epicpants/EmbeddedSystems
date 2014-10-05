#ifndef _DELAY_H
#define _DELAY_H

#include <main.h>

#define PRELOAD1 (65536 - (uint16)(OSC_FREQ / (OSC_PER_INST * 1020)))
#define PRELOADH1 (PRELOAD1 / 256)
#define PRELOADL1 (PRELOAD1 % 256)

void Delay_ms(uint32 timeout_val_ms);
#endif