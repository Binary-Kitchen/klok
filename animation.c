#include <avr/pgmspace.h>
#include <util/delay.h>

#include "animation.h"
#include "display.h"

static const char hello_P[] PROGMEM = "Hello, world! This is KLOK.";

static void print_scroll(const char *str, unsigned int delay)
{
	unsigned char next;
	unsigned int i;
	while (*str) {
		next = display_get_alphanum(*str);
		display_scroll_left(next);
		for (i = 0; i < delay; i++)
			/* we need compile time constants... */
			_delay_ms(1);
		str++;
	}
}

static void print_scroll_P(const char *str, unsigned int delay)
{
	unsigned char next;
	unsigned int i;

	while (pgm_read_byte(str)) {
		next = display_get_alphanum(pgm_read_byte(str));
		display_scroll_left(next);
		for (i = 0; i < delay; i++)
			/* we need compile time constants... */
			_delay_ms(1);
		str++;
	}
}

static void circle(unsigned char pos, unsigned char width, unsigned int delay)
{
	unsigned int i;

	display_clear();
	for (i = pos; i <= pos + width; i++) {
		display_set_character(i, BOTTOM);
		_delay_ms(delay);
	}

	display_or_character(pos + width, BOTTOM_LEFT);
	_delay_ms(delay);

	display_or_character(pos + width, TOP_LEFT);
	_delay_ms(delay);

	for (i = pos ; i <= pos + width; i++) {
		display_or_character(i, TOP);
		_delay_ms(delay);
	}

	display_or_character(pos, TOP_RIGHT);
	_delay_ms(delay);

	display_or_character(pos, BOTTOM_RIGHT);
	_delay_ms(delay);
}

void animation(void)
{
	static bool init = false;
	unsigned int i;
	unsigned int delay;

	if (!init) {
		print_scroll_P(hello_P, 200);
		init = true;
	}

	delay = 25;
	circle(0, 5, delay);
	_delay_ms(250);
	circle(1, 3, delay);
	_delay_ms(250);
	circle(2, 1, delay);
	_delay_ms(250);
	for (i = 0; i < SEGMENTS; i++)
		circle(i, 0, delay);
	for (i = 0; i < SEGMENTS; i++)
		circle(SEGMENTS - 1 - i, 0, delay);
}
