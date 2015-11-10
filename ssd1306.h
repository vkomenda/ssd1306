/*
 * Copyright (c) 2015, Vladimir Komendantskiy
 * MIT License
 *
 * IOCTL interface to SSD1306 modules.
 *
 * Command sequences are sourced from an Arduino library by Sonal Pinto.
 */

#ifndef SSD1306_H
#define SSD1306_H

#define SSD1306_I2C_ADDR                   0x3c

// Control byte
#define SSD1306_CTRL_BYTE_CMD_SINGLE       0x80
#define SSD1306_CTRL_BYTE_CMD_STREAM       0x00
#define SSD1306_CTRL_BYTE_DATA_STREAM      0x40

// Fundamental commands (page 28)
#define SSD1306_CMD_SET_CONTRAST           0x81
#define SSD1306_CMD_DISPLAY_RAM            0xA4
#define SSD1306_CMD_DISPLAY_ALLON          0xA5
#define SSD1306_CMD_DISPLAY_NORMAL         0xA6
#define SSD1306_CMD_DISPLAY_INVERTED       0xA7
#define SSD1306_CMD_DISPLAY_OFF            0xAE
#define SSD1306_CMD_DISPLAY_ON             0xAF

// Addressing Command Table (page 30)
#define SSD1306_CMD_SET_MEMORY_ADDR_MODE   0x20
#define SSD1306_CMD_SET_COLUMN_RANGE       0x21
#define SSD1306_CMD_SET_PAGE_RANGE         0x22

// Hardware Config (page 31)
#define SSD1306_CMD_SET_DISPLAY_START_LINE 0x40
#define SSD1306_CMD_SET_SEGMENT_REMAP      0xA1
#define SSD1306_CMD_SET_MUX_RATIO          0xA8
#define SSD1306_CMD_SET_COM_SCAN_MODE      0xC8
#define SSD1306_CMD_SET_DISPLAY_OFFSET     0xD3
#define SSD1306_CMD_SET_COM_PIN_MAP        0xDA
#define SSD1306_CMD_NOP                    0xE3

// Timing and Driving Scheme (page 32)
#define SSD1306_CMD_SET_DISPLAY_CLK_DIV    0xD5
#define SSD1306_CMD_SET_PRECHARGE          0xD9
#define SSD1306_CMD_SET_VCOMH_DESELCT      0xDB

// Charge Pump (page 62)
#define SSD1306_CMD_SET_CHARGE_PUMP        0x8D

static const unsigned char display_config[] = {
	SSD1306_CTRL_BYTE_CMD_STREAM,
	SSD1306_CMD_DISPLAY_OFF,
	SSD1306_CMD_SET_MUX_RATIO,
	0x3F,
	// Set the display offset to 0
	SSD1306_CMD_SET_DISPLAY_OFFSET,
	0x00,
	// Display start line to 0
	SSD1306_CMD_SET_DISPLAY_START_LINE,
	// Mirror the x-axis. In case you set it up such that the pins are north.
	// 0xA0 - in case pins are south - default
	SSD1306_CMD_SET_SEGMENT_REMAP,
	// Mirror the y-axis. In case you set it up such that the pins are north.
	// 0xC0 - in case pins are south - default
	SSD1306_CMD_SET_COM_SCAN_MODE,
	// Default - alternate COM pin map
	SSD1306_CMD_SET_COM_PIN_MAP,
	0x12,
	// set contrast
	SSD1306_CMD_SET_CONTRAST,
	0x7F,
	// Set display to enable rendering from GDDRAM (Graphic Display Data RAM)
	SSD1306_CMD_DISPLAY_RAM,
	// Normal mode!
	SSD1306_CMD_DISPLAY_NORMAL,
	// Default oscillator clock
	SSD1306_CMD_SET_DISPLAY_CLK_DIV,
	0x80,
	// Enable the charge pump
	SSD1306_CMD_SET_CHARGE_PUMP,
	0x14,
	// Set precharge cycles to high cap type
	SSD1306_CMD_SET_PRECHARGE,
	0x22,
	// Set the V_COMH deselect volatage to max
	SSD1306_CMD_SET_VCOMH_DESELCT,
	0x30,
	// Horizonatal addressing mode - same as the KS108 GLCD
	SSD1306_CMD_SET_MEMORY_ADDR_MODE,
	0x00,
	// Turn the Display ON
	SSD1306_CMD_DISPLAY_ON
};

static const unsigned char display_draw[] = {
	SSD1306_CTRL_BYTE_CMD_STREAM,
	// column 0 to 127
	SSD1306_CMD_SET_COLUMN_RANGE,
	0x00,
	0x7F,
	// page 0 to 7
	SSD1306_CMD_SET_PAGE_RANGE,
	0x00,
	0x07
};

struct display_info {
	int address;
	int file;
	unsigned char buffer[1024];
};

struct sized_array {
	int size;
	const unsigned char* array;
};

extern int ssd1306_close       (struct display_info* disp);
extern int ssd1306_open        (struct display_info* disp, char* filename);
extern int ssd1306_send        (struct display_info* disp, struct sized_array* payload);
extern int ssd1306_init        (struct display_info* disp);
extern int ssd1306_send_buffer (struct display_info* disp);

#endif // SSD1306_H
