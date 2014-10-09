/*
 * Authors: Jon Eftink and Tyler Ryan
 * File: LED.h
 * Brief: Function prototypes for LED operations 
 */
#ifndef _LED_H
#define _LED_H

#include <delay.h>
#include <switch.h>

#define LEFT (0)
#define RIGHT (1)
#define STEP_SIZE (100)
#define MAX_SPEED (2000)
#define MIN_SPEED (100)

uint8 LED_Change_State(uint8 current_state, bit DIRECTION);
bit CHECK_LED_DIRECTION(uint8 * array_sw_states, uint8 * array_sw_held, bit CURRENT_DIRECTION);
uint16 CHECK_LED_SPEED(uint8 * array_sw_states, uint8 * array_sw_held, uint16 current_speed);
#endif