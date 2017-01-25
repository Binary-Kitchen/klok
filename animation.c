#include <util/delay.h>

#include "animation.h"
#include "display.h"

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
	unsigned int i;
	unsigned int delay;

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
