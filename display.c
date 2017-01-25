/*
 * Klok, a 7-segment Clock
 *
 * Copyright (c) Ralf Ramsauer <ralf@ramses-pyramidenbau.de>, 2017
 *
 * This work is licensed under the terms of the GNU GPL, version 2.  See
 * the COPYING file in the top-level directory.
 */

#include <ctype.h>
#include <string.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include "display.h"

#define SEGMENT_SELECT_PORT PORTB
#define SEGMENT_SELECT_DDR  DDRB
#define SEGMENT_DATA_PORT   PORTA
#define SEGMENT_DATA_DDR    DDRA

#define _MemoryBarrier() __asm__ __volatile__("":::"memory")

static unsigned char display[SEGMENTS];

static const unsigned char panic[SEGMENTS] PROGMEM = {
	0,
	BOTTOM_LEFT | TOP_LEFT | TOP | BOTTOM,
	BOTTOM_LEFT | TOP_LEFT,
	BOTTOM_LEFT | TOP_LEFT | TOP | TOP_RIGHT | BOTTOM_RIGHT,
	BOTTOM_LEFT | TOP_LEFT | TOP | TOP_RIGHT | BOTTOM_RIGHT | MID,
	BOTTOM_LEFT | TOP_LEFT | TOP | TOP_RIGHT | MID,
};

static const unsigned char ascii[128] PROGMEM = {
	['0'] =
		238,
		40,
		205,
		109,
		43,
		103,
		231,
		44,
		239,
		111,

	['a'] =
		BOTTOM_LEFT | TOP_LEFT | TOP | TOP_RIGHT | BOTTOM_RIGHT | MID, /* a */
		ALL, /* b */
		BOTTOM | BOTTOM_LEFT | TOP_LEFT | TOP, /* c */
		238, /* d */
		BOTTOM | MID | TOP | BOTTOM_LEFT | TOP_LEFT, /* e */
		MID | TOP | BOTTOM_LEFT | TOP_LEFT, /* f */
		BOTTOM | MID | BOTTOM_RIGHT | BOTTOM_LEFT | TOP_LEFT, /* g */
		MID | BOTTOM_RIGHT | BOTTOM_LEFT | TOP_LEFT | TOP_RIGHT, /* h */
		BOTTOM_LEFT | TOP_LEFT, /* i */
		BOTTOM_RIGHT | TOP_RIGHT, /* j */
		TOP_LEFT | MID | BOTTOM_RIGHT, /* k */
		TOP_LEFT | BOTTOM_LEFT | BOTTOM, /* l */
		BOTTOM_LEFT | TOP_LEFT | TOP | TOP_RIGHT | BOTTOM_RIGHT, /* m */
		BOTTOM_LEFT | TOP_LEFT | TOP | TOP_RIGHT | BOTTOM_RIGHT, /* n */
		238, /* o */
		BOTTOM_LEFT | TOP_LEFT | TOP | TOP_RIGHT | MID, /* p */
		BOTTOM_RIGHT | TOP_LEFT | TOP | TOP_RIGHT | BOTTOM_RIGHT | MID, /* q */
		BOTTOM_LEFT | BOTTOM_RIGHT | TOP_LEFT | TOP | TOP_RIGHT | BOTTOM_RIGHT | MID, /* r */
		TOP | TOP_LEFT | MID | BOTTOM_RIGHT | BOTTOM, /* s */
		BOTTOM_LEFT | TOP_LEFT | TOP, /* t */
		BOTTOM_LEFT | TOP_LEFT | BOTTOM | BOTTOM_RIGHT | TOP_RIGHT, /* u */
		BOTTOM_LEFT | TOP_LEFT | BOTTOM | BOTTOM_RIGHT | TOP_RIGHT, /* v */
		BOTTOM_LEFT | TOP_LEFT | BOTTOM | BOTTOM_RIGHT | TOP_RIGHT, /* v */
		MID | BOTTOM_RIGHT | BOTTOM_LEFT | TOP_LEFT | TOP_RIGHT, /* x */
		MID | BOTTOM_RIGHT | TOP_LEFT | TOP_RIGHT, /* y */
		205, /* 2 */

	['A'] =
		BOTTOM_LEFT | TOP_LEFT | TOP | TOP_RIGHT | BOTTOM_RIGHT | MID, /* a */
		ALL, /* b */
		BOTTOM | BOTTOM_LEFT | TOP_LEFT | TOP, /* c */
		238, /* d */
		BOTTOM | MID | TOP | BOTTOM_LEFT | TOP_LEFT, /* e */
		MID | TOP | BOTTOM_LEFT | TOP_LEFT, /* f */
		BOTTOM | MID | BOTTOM_RIGHT | BOTTOM_LEFT | TOP_LEFT, /* g */
		MID | BOTTOM_RIGHT | BOTTOM_LEFT | TOP_LEFT | TOP_RIGHT, /* h */
		BOTTOM_LEFT | TOP_LEFT, /* i */
		BOTTOM_RIGHT | TOP_RIGHT, /* j */
		TOP_LEFT | MID | BOTTOM_RIGHT, /* k */
		TOP_LEFT | BOTTOM_LEFT | BOTTOM, /* l */
		BOTTOM_LEFT | TOP_LEFT | TOP | TOP_RIGHT | BOTTOM_RIGHT, /* m */
		BOTTOM_LEFT | TOP_LEFT | TOP | TOP_RIGHT | BOTTOM_RIGHT, /* n */
		238, /* o */
		BOTTOM_LEFT | TOP_LEFT | TOP | TOP_RIGHT | MID, /* p */
		BOTTOM_RIGHT | TOP_LEFT | TOP | TOP_RIGHT | BOTTOM_RIGHT | MID, /* q */
		BOTTOM_LEFT | BOTTOM_RIGHT | TOP_LEFT | TOP | TOP_RIGHT | BOTTOM_RIGHT | MID, /* r */
		TOP | TOP_LEFT | MID | BOTTOM_RIGHT | BOTTOM, /* s */
		BOTTOM_LEFT | TOP_LEFT | TOP, /* t */
		BOTTOM_LEFT | TOP_LEFT | BOTTOM | BOTTOM_RIGHT | TOP_RIGHT, /* u */
		BOTTOM_LEFT | TOP_LEFT | BOTTOM | BOTTOM_RIGHT | TOP_RIGHT, /* v */
		BOTTOM_LEFT | TOP_LEFT | BOTTOM | BOTTOM_RIGHT | TOP_RIGHT, /* v */
		MID | BOTTOM_RIGHT | BOTTOM_LEFT | TOP_LEFT | TOP_RIGHT, /* x */
		MID | BOTTOM_RIGHT | TOP_LEFT | TOP_RIGHT, /* y */
		205, /* 2 */

	['.'] = DOT,
	[','] = DOT,
	['!'] = TOP_RIGHT | BOTTOM_RIGHT | DOT,
};

static inline void nops(void) {
	unsigned int i;
	for (i = 0; i < 10; i++)
		asm volatile("nop");
}

ISR(TIMER0_OVF_vect)
{
	static unsigned char segment = 0;
	unsigned char i;

	SEGMENT_DATA_PORT = 0;
	SEGMENT_SELECT_PORT = 0;
	_MemoryBarrier();
	for (i = 0; i < 5; i++)
		__asm__ __volatile__("nop\n");
	SEGMENT_SELECT_PORT = (1<<segment);
	_MemoryBarrier();
	SEGMENT_DATA_PORT = display[segment];
	_MemoryBarrier();

	if (++segment == SEGMENTS)
		segment = 0;
}

void display_init(void)
{
	SEGMENT_SELECT_PORT = 0;
	SEGMENT_DATA_PORT = 0;
	SEGMENT_SELECT_DDR |= (1<<(SEGMENTS+1)) - 1;
	SEGMENT_DATA_DDR = 0xff;

	/* install timer every 1ms */
	TCCR0 = (1 << CS01) | (1 << CS00); /* prescaler: clkio / 64 */
	TCNT0 = 0;
	TIMSK |= (1 << TOIE0);
}

void display_panic(void)
{
	memcpy_P(display, panic, sizeof(panic));
}

void display_num(unsigned char pos, unsigned char num)
{
	if (pos >= SEGMENTS)
		return;

	if (num > 9)
		return;

	display[pos] = pgm_read_byte(ascii + '0' + num);
}

void display_dot(unsigned char pos, bool set)
{
	if (pos >= SEGMENTS)
		return;

	if (set)
		display[pos] |= DOT;
	else
		display[pos] &= ~DOT;
}

void display_scroll_left(unsigned char fillup)
{
	unsigned char segment;
	for (segment = 5; segment > 0; segment--)
		display[segment] = display[segment - 1];

	display[0] = fillup;
	*display = fillup;
}

void display_set_character(unsigned char pos, unsigned char mask)
{
	if (pos >= SEGMENTS)
		return;

	display[pos] = mask;
}

void display_clear(void)
{
	memset(display, 0, SEGMENTS);
}

unsigned char display_get_character(unsigned char pos)
{
	if (pos < SEGMENTS)
		return display[pos];
	return 0;
}

void display_or_character(unsigned char pos, unsigned char mask)
{
	if (pos >= SEGMENTS)
		return;
	display[pos] |= mask;
}

unsigned char display_get_alphanum(char c)
{
	if (c > 0)
		return pgm_read_byte(ascii + c);
	return 0;
}

void display_alphanum(unsigned char pos, char c, bool dot) {
	if (pos >= SEGMENTS)
		return;

	display[pos] = display_get_alphanum(c) | (dot ? DOT : 0);
}
