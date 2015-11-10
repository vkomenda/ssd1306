/*
 * Copyright (c) 2015, Vladimir Komendantskiy
 * MIT License
 *
 * SSD1306 is a 128x64 dot matrix display driver and controller by Solomon
 * Systech. It is used by HelTec display modules.
 *
 * Reference:
 *
 * [1] SSD1306 Advance Information. 128x64 Dot Matrix OLED/PLED Segment/Common
 *     Driver with Controller. (Solomon Systech)
 */

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <fcntl.h>

// real-time features
#include <sys/mman.h>
#include <sched.h>

#include "ssd1306.h"

#define SSD1306_PACKET_SIZE 32

int ssd1306_close(struct display_info* disp)
{
	if (close(disp->file) < 0)
		return -1;

	return 0;
}

void cleanup(int status, void* disp)
{
	ssd1306_close((struct display_info*)disp);
}

int ssd1306_open(struct display_info* disp, char* filename)
{
	disp->file = open(filename, O_RDWR);
	if (disp->file < 0)
		return -1;

	if (ioctl(disp->file, I2C_SLAVE, disp->address) < 0)
		return -2;

	on_exit(cleanup, (void*)disp);

	return 0;
}

int ssd1306_send(struct display_info* disp, struct sized_array* payload)
{
	if (write(disp->file, payload->array, payload->size) != payload->size)
		return -1;

	return 0;
}

int ssd1306_init(struct display_info* disp)
{
	struct sched_param sch;
	int status = 0;
	struct sized_array payload;

	sch.sched_priority = 49;

	status = sched_setscheduler(0, SCHED_FIFO, &sch);
	if (status < 0)
		return status;

	status = mlockall(MCL_CURRENT | MCL_FUTURE);
	if (status < 0)
		return status;

	payload.size = sizeof(display_config);
	payload.array = display_config;

	status = ssd1306_send(disp, &payload);
	if (status < 0)
		return status;

	memset(disp->buffer, 0, sizeof(disp->buffer));

	return 0;
}

int ssd1306_send_buffer(struct display_info* disp)
{
	int offset = 0, status = 0;
	struct sized_array payload;

	for (offset = 0; offset < 1024; offset += SSD1306_PACKET_SIZE) {
		unsigned char packet[SSD1306_PACKET_SIZE + 1];

		packet[0] = SSD1306_CTRL_BYTE_DATA_STREAM;
		memcpy(packet + 1, disp->buffer + offset, SSD1306_PACKET_SIZE);

		payload.size = SSD1306_PACKET_SIZE + 1;
		payload.array = packet;

		status = ssd1306_send(disp, &payload);
		if (status < 0)
			return -1;
	}

	return 0;
}
