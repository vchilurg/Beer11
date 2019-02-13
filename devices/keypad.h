/*
 * keypad.h
 *
 *  Created on: Sep 22, 2015
 *      Author: Wes
 */

#ifndef KEYPAD_H_
#define KEYPAD_H_

#include "../config.h"

typedef enum KeypadKeycode {
	KEYPAD_KEY_0 = 0,
	KEYPAD_KEY_1,
	KEYPAD_KEY_2,
	KEYPAD_KEY_3,
	KEYPAD_KEY_4,
	KEYPAD_KEY_5,
	KEYPAD_KEY_6,
	KEYPAD_KEY_7,
	KEYPAD_KEY_8,
	KEYPAD_KEY_9,
	KEYPAD_KEY_A,
	KEYPAD_KEY_B,
	KEYPAD_KEY_C,
	KEYPAD_KEY_D,
	KEYPAD_KEY_STAR,
	KEYPAD_KEY_HASH,
	KEYPAD_KEY_NONE
} KeypadKeycode;

void keypad_init();
void keypad_set_handler(bool (*)(KeypadKeycode));

#endif /* KEYPAD_H_ */
