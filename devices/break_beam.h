/*
 * break_beam.h
 *
 *  Created on: Nov 28, 2016
 *      Author: matt
 */

#ifndef DEVICES_BREAK_BEAM_H_
#define DEVICES_BREAK_BEAM_H_

#include "../ui/ui_internal.h"

#define MASH_TUN_BB BIT7
#define KETTLE_BB	BIT6

void init_bb();
void set_bb_state();

#endif /* DEVICES_BREAK_BEAM_H_ */
