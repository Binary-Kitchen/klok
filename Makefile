# PPMDecoder -  Decode PPM signals on an AVR and convert it to I2C / UART
#
# Copyright (c) OTH Regensburg, 2016
#
# Authors:
#  Ralf Ramsauer <ralf.ramsauer@oth-regensburg.de>
#
# This work is licensed under the terms of the GNU GPL, version 2.  See
# the COPYING file in the top-level directory.


TARGET = klok

MCU ?= atmega16
F_OSC ?= 16000000
UART_BAUD ?= 115200

OBJS = main.o display.o twimaster.o ds1307.o animation.o

CC = avr-gcc
OBJCOPY = avr-objcopy
AVRDUDE = avrdude

CFLAGS := -g -O2 -mmcu=$(MCU)
CFLAGS += -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CFLAGS += -Wall -Wextra -Wstrict-prototypes
CFLAGS += -DF_OSC=$(F_OSC) -DF_CPU=F_OSC -DUART_BAUD=$(UART_BAUD)UL -DMCU=$(MCU)

all: $(TARGET).hex

$(TARGET).elf: $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET).elf $^

$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -O ihex -R .eeprom $^ $@

program: $(TARGET).hex
	$(AVRDUDE) -p $(MCU) -P usb -c avr910 -P /dev/ttyUSB0 -U flash:w:$^

clean:
	rm -f $(OBJS)
	rm -f $(TARGET).elf $(TARGET).hex
