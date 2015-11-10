/*
 * Copyright (c) 2015, Vladimir Komendantskiy
 * MIT License
 *
 * SSD1306 demo of block and font drawing.
 */

#include <glib.h>
#include <glib/gprintf.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "ssd1306.h"
#include "font5x7.h"

#define FRAME_LENGTH        500000000L  // ns
#define NSEC_PER_SEC       1000000000L

static const unsigned char block8x8_patterns[5][8] = {
	{0, 0x7E, 0x7C, 0x78, 0x70, 0x60, 0x40, 0},    // big triangle
	{0, 0,    0x3C, 0x38, 0x30, 0x20, 0,    0},    // medium-size triangle
	{0, 0,    0,    0x18, 0x18, 0,    0,    0},    // 2x2 square
	{0, 0,    0x3C, 0x24, 0x24, 0x3C, 0,    0},    // 4x4 square outline
	{0, 0x7E, 0x42, 0x42, 0x42, 0x42, 0x7E, 0}     // 6x6 square outline
};

int ssd1306_demo(struct display_info* disp)
{
	struct timespec t;
	int status = 0, cycle = 0, step = 0, offset = 0;
	struct sized_array payload;

	while (1) {
		clock_gettime(CLOCK_MONOTONIC ,&t);

		// set the next timestamp
                t.tv_nsec += FRAME_LENGTH;
                while (t.tv_nsec >= NSEC_PER_SEC) {
                       t.tv_nsec -= NSEC_PER_SEC;
                        t.tv_sec++;
                }

		payload.size = sizeof(display_draw);
		payload.array = display_draw;

		status = ssd1306_send(disp, &payload);
		if (status < 0)
			return -1;

		switch (step = cycle % 8) {
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
			for (offset = 0;
			     offset < sizeof(disp->buffer);
			     offset += 8) {
				memcpy(disp->buffer + offset,
				       block8x8_patterns[step], 8);
			}
			break;
		case 5:
			memcpy(disp->buffer, font, sizeof(disp->buffer));
			break;
		default:
			break;
		}

		status = ssd1306_send_buffer(disp);
		if (status < 0)
			return status;

		cycle++;
		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);
	}

	return 0;
}

void show_error(void)
{
	const gchar* errmsg;
	errmsg = g_strerror(errno);
	printf("\nERROR: %s\n\n", errmsg);
}

void show_usage(char* progname)
{
	printf("\nUsage:\n%s <I2C bus device node>\n", progname);
}

int main(int argc, char** argv)
{
	char filename[32];
	const gchar* errmsg;
	struct display_info disp;

	if (argc < 2) {
		show_usage(argv[0]);
		return -1;
	}

	memset(&disp, 0, sizeof(disp));

	sprintf(filename, argv[1]);
	disp.address = SSD1306_I2C_ADDR;

	if (ssd1306_open(&disp, filename) < 0 ||
	    ssd1306_init(&disp)           < 0 ||
	    ssd1306_demo(&disp)           < 0)
	{
		show_error();
	}

	return 0;
}
