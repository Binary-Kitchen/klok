/*
 * Klok, a 7-segment Clock
 *
 * Copyright (c) Ralf Ramsauer <ralf@ramses-pyramidenbau.de>, 2017
 *
 * This work is licensed under the terms of the GNU GPL, version 2.  See
 * the COPYING file in the top-level directory.
 */

#include "rtc.h"
#include "i2cmaster.h"
#include "errno.h"

#define DS1307 0xd0

#define DS1307_SECOND 0
#define DS1307_MINUTE 1
#define DS1307_HOUR 2

#define DS1307_WEEK_DAY 3
#define DS1307_DAY 4
#define DS1307_MONTH 5
#define DS1307_YEAR 6

#define READ_DEC_OR_FAIL(REGISTER) \
	ret = ds1307_read_dec(REGISTER); \
	if (ret < 0) \
		return ret;

#define WRITE_DEC_OR_FAIL(REGISTER, VALUE) \
	ret = ds1307_write_dec(REGISTER, VALUE); \
	if (ret < 0) \
		return ret;

static inline char bcd_to_dec(char v)
{
	char n;

	n = v;
	n = n & 0xF0;
	n = n / 16;
	v = v & 0x0F;
	n = n * 10 + v;

	return(n);
}

static inline char dec_to_bcd(char v){
	char n;

	n = v;
	n = n / 10;
	n = n * 16;
	v = v % 10;
	n = n + v;
	return n;
}

int rtc_init(void)
{
	i2c_init();
	return 0;
}

static int ds1307_read(unsigned char address)
{
	unsigned char byte;

	if (i2c_start(DS1307 + I2C_WRITE))
		return -EBUSY;

	if (i2c_write(address))
		return -EINVAL;

	if (i2c_rep_start(DS1307 + I2C_READ))
		return -EBUSY;

	byte = i2c_readNak();
	i2c_stop();

	return byte;
}

static int ds1307_write(unsigned char address, unsigned char byte)
{
	if(i2c_start(DS1307 + I2C_WRITE))
		return -EBUSY;

	if (i2c_write(address))
		return -EINVAL;

	if (i2c_write(byte))
		return -EINVAL;

	i2c_stop();

	return 0;
}

static int ds1307_read_dec(unsigned char address)
{
	int ret;

	ret = ds1307_read(address);
	if (ret < 0)
		return ret;

	return bcd_to_dec(ret);
}

static int ds1307_write_dec(unsigned char address, unsigned char dec)
{
	return ds1307_write(address, dec_to_bcd(dec));
}

int rtc_set_time(const struct time *time)
{
	int ret;

	WRITE_DEC_OR_FAIL(DS1307_SECOND, time->second);
	WRITE_DEC_OR_FAIL(DS1307_MINUTE, time->minute);
	WRITE_DEC_OR_FAIL(DS1307_HOUR, time->hour);

	return 0;
}

int rtc_set_date(const struct date *date){
	int ret;

	WRITE_DEC_OR_FAIL(DS1307_DAY, date->day);
	WRITE_DEC_OR_FAIL(DS1307_MONTH, date->month);
	WRITE_DEC_OR_FAIL(DS1307_YEAR, date->year - 2000);
	WRITE_DEC_OR_FAIL(DS1307_WEEK_DAY, date->week_day);

	return 0;
}

int rtc_get_date(struct date *date){
	int ret;

	READ_DEC_OR_FAIL(DS1307_DAY);
	date->day = ret;
	READ_DEC_OR_FAIL(DS1307_MONTH);
	date->month = ret;
	READ_DEC_OR_FAIL(DS1307_YEAR);
	date->year = ret + 2000;
	READ_DEC_OR_FAIL(DS1307_WEEK_DAY);
	date->week_day = ret;

	return 0;
}

int rtc_get_time(struct time *time){
	int ret;

	READ_DEC_OR_FAIL(DS1307_SECOND);
	time->second = ret;
	READ_DEC_OR_FAIL(DS1307_HOUR);
	time->hour = ret;
	READ_DEC_OR_FAIL(DS1307_MINUTE);
	time->minute = ret;

	return 0;
}
