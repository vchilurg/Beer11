/*
 * rtc.c
 *
 *  Created on: Apr 9, 2015
 *      Author: Wes
 */

#include "rtc.h"

char time_display[6] = "  :  ";
char alarm_display[6] = "  :  ";

__force_inline void rtc_format_display() {
	bcd8toa_s(time_display + 0, RTCHOUR);
	bcd8toa_0(time_display + 3, RTCMIN);
	bcd8toa_s(alarm_display + 0, RTCAHOUR & 0x7F);
	bcd8toa_0(alarm_display + 3, RTCAMIN  & 0x7F);
}

void rtc_init() {
	RTCCTL0 = RTCKEY;// | RTCAIE | RTCTEVIE;
	RTCCTL1 = RTCMODE | RTCBCD | RTCTEV__MIN | RTCHOLD;
	RTCHOUR = 0; RTCMIN = 0; RTCSEC = 0;
	RTCAMINHR = 0; RTCADOWDAY = 0;
	RTCCTL1 = RTCMODE | RTCBCD | RTCTEV__MIN;
	RTCCTL0 = RTCKEY | RTCAIE | RTCTEVIE;//RTCPWD = 0;
	rtc_format_display();
}

void rtc_clear_alarm() {
	RTCCTL0 = RTCKEY | RTCTEVIE;
	RTCPWD = 0;
	RTCAHOUR = 0; RTCAMIN = 0;
}
void rtc_set_alarm(uint8_t hr, uint8_t min) {
	RTCCTL0 = RTCKEY | RTCTEVIE;
	RTCAHOUR = 0x80 | hr; RTCAMIN = 0x80 | min;
	RTCCTL0 = RTCKEY | RTCAIE | RTCTEVIE;//RTCPWD = 0;
	rtc_format_display();
}
void rtc_set_alarm_min(uint16_t min) {
	uint16_t hm = min;
	if (hm >= 0x60) hm += 0xA0;
	rtc_set_alarm(hm >> 8, hm & 0xFF);
}

static bool rtc_event_nop() { return false; }
bool (*rtc_alarm_fun)() = rtc_event_nop;
void rtc_set_alarm_fun(bool (*fp)()) { rtc_alarm_fun = fp ? fp : rtc_event_nop; }

__isr(RTC_VECTOR, rtc_event) {
	uint16_t saved_rtciv = __even_in_range(RTCIV, RTCIV_RT1PSIFG);
	if (saved_rtciv == RTCIV_RTCTEVIFG) {
		rtc_format_display();
		__low_power_mode_off_on_exit();
	} else if (saved_rtciv == RTCIV_RTCAIFG)
		if (rtc_alarm_fun()) __low_power_mode_off_on_exit();
}
