CROSS_COMPILE = arm-linux-gnueabihf-

CC       = $(CROSS_COMPILE)gcc
LD       = $(CROSS_COMPILE)gcc
AS       = $(CROSS_COMPILE)as
AR       = $(CROSS_COMPILE)ar
OBJCOPY  = $(CROSS_COMPILE)objcopy
OBJDUMP  = $(CROSS_COMPILE)objdump
STRIP    = $(CROSS_COMPILE)strip
NM       = $(CROSS_COMPILE)nm

CFLAGS=-I/usr/include/glib-2.0 -I/usr/lib/arm-linux-gnueabihf/glib-2.0/include -std=c11
CFLAGS+=-D_POSIX_C_SOURCE=200809L -D_SVID_SOURCE

LDFLAGS=-lglib-2.0 -L/usr/lib/arm-linux-gnueabihf

all: ssd1306_demo

ssd1306_demo: ssd1306_demo.o ssd1306.o
	$(LD) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $<
