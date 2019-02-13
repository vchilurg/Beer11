/*
 * config.h
 *
 *  Created on: Oct 16, 2016
 *      Author: Wes
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include <msp430.h>
#include <stdint.h>
#include <stdbool.h>
#include <float.h>

// Default defines lack descriptive names for timer capture modes
#define CM__NONE CM_0
#define CM__RISE CM_1
#define CM__FALL CM_2
#define CM__BOTH CM_3

// Convenient names for attributes - also to remind me that they exist
#define __force_inline         __attribute__((always_inline)) static inline
#define __never_inline         __attribute__((noinline))
#define __isr(_vec, _name)     __attribute__((interrupt(_vec))) void _name (void)
#define __ramfunc              __attribute__((ramfunc))
#define __no_return            __attribute__((noreturn))
#define __constructor          __attribute__((constructor))
#define __used                 __attribute__((used))
#define __unused               __attribute__((unused))
#define __deprecated           __attribute__((deprecated))
#define __deprecated_msg(_msg) __attribute__((deprecated(_msg)))
#define __aligned(_align)      __attribute__((aligned(_align)))
#define __packed               __attribute__((packed))
#define __no_init              __attribute__((noinit))
#define __persistent           __attribute__((persistent))
#define __transparent_union    __attribute__((transparent_union))

// Convenient names for built-in functions in the compiler
#define __is_constant(_expr)   __builtin_constant_p(_exp)
#define __expect(_expr, _val)  __builtin_expect(_expr, _val)

static const char digits  [11] = "0123456789";
static const char digits_s[11] = " 123456789";

__force_inline void bcd16toa_0(char buf[4], uint16_t n) {
	buf[0] = digits[(n & 0xF000) >> 12]; buf[1] = digits[(n & 0x0F00) >> 8];
	buf[2] = digits[(n & 0x00F0) >>  4]; buf[3] = digits[(n & 0x000F)];
}

__force_inline void bcd16toa_s(char buf[4], uint16_t n) {
	buf[0] = digits_s[(n & 0xF000) >> 12];
	buf[1] = (buf[0] == ' ' ? digits_s : digits)[(n & 0x0F00) >> 8];
	buf[2] = (buf[1] == ' ' ? digits_s : digits)[(n & 0x00F0) >> 4];
	buf[3] = digits[(n & 0x000F)];
}

__force_inline void bcd8toa_0(char buf[2], uint8_t n) {
	buf[0] = digits[n >> 4]; buf[1] = digits[n & 0x0F];
}

__force_inline void bcd8toa_s(char buf[2], uint8_t n) {
	buf[0] = digits_s[n >> 4]; buf[1] = digits[n & 0x0F];
}

static inline void itoa_0(char* buf, unsigned len, unsigned n) {
	while (len > 0) { buf[--len] = digits[n % 10]; n /= 10; }
}

static inline void itoa_s(char* buf, unsigned len, unsigned n) {
	itoa_0(buf, len, n);
	for (unsigned i = 0; i < len && buf[i] == '0'; ++i) buf[i] = ' ';
}

#endif /* CONFIG_H_ */
