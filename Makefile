CC = cc
CFLAGS = -std=c99 -pedantic -Wall

all: qr

qr: qr.c png_create.c png_utils.c
	$(CC) $(CFLAGS) -o qr qr.c png_create.c png_utils.c
