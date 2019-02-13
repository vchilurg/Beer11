/*
 * rtc.h
 *
 *  Created on: Apr 9, 2015
 *      Author: Wes
 */

#ifndef SENSORS_RTC_H_
#define SENSORS_RTC_H_

#include "../config.h"

extern char time_display[6];
extern char alarm_display[6];

void rtc_init();
// All times are in BCD format
void rtc_set_alarm(uint8_t hr, uint8_t min);
void rtc_set_alarm_min(uint16_t min);
void rtc_clear_alarm();
void rtc_set_alarm_fun(bool (*)());

#endif /* SENSORS_RTC_H_ */
