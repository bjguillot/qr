/*	PNG File Generator
*	by B.J. Guillot (bguillot@acm.org)
*	Copyright (C) 2016
*	All Rights Reserved
*	Released under MIT License
*	Version 1.0
*	2016-07-01
*/

/*
*	The png_create function takes a 8-bit (256 color) "image" (2D unsigned char array)
*	of specified "width" and "height" (pixels) and writes it out to passed-in "filename".
*	Will pad the top, bottom, left, and right margins with number of pixels specified in "border". 
*
*       Compiled and tested on Mac OS X El Capitan 10.11.5.
*
*       TODO: Does not currently compress image data, need to add Huffman encoding to IDAT chunk.
*	TODO: Eventually want to add support for 1-bit (black and white) images to save space.
*/

#include <stdio.h>	//for fopen, fclose, FILE
#include <stdint.h>	//for uint32_t, etc.
#include <stdlib.h>	//for malloc,free
#include <string.h>	//for memcpy
#include "png_utils.h"	//for crc32, adler32, swap_uint32

// -----------------------------------------------------------------------------------------------

void png_create(uint32_t width, uint32_t height, unsigned char **image, char* filename, uint32_t border) {
	FILE *pngfile;
	pngfile = fopen(filename, "wb");

	unsigned char file_header[] = {137, 80, 78, 71, 13, 10, 26, 10};
	unsigned char idat_chunk[] = {'I', 'D', 'A', 'T', 120, 156, 1}; // IDAT chunk; CMF=120 (CM=8; CI=7); FLG=156 (FCHECK=28; FDICT=0; FLEVEL=2); Block Format=1
	unsigned char file_footer[] = {0, 0, 0, 0, 'I', 'E', 'N', 'D', 0xae, 0x42, 0x60, 0x82}; // 4-byte zero length; IEND chunk; CRC-32
	int status = fwrite(file_header, sizeof(file_header), 1, pngfile);

	ihdr_t ihdr;
	uint32_t chunk_length = swap_uint32(sizeof(ihdr));
	status = fwrite(&chunk_length, sizeof(uint32_t), 1, pngfile);
	ihdr.width = swap_uint32(width + border*2);
	ihdr.height = swap_uint32(height + border*2);
	ihdr.bit_depth = 8; 		// 8-bit image
	ihdr.color_type = 0; 		// grayscale samples
	ihdr.comp_method = 0;		// deflate/inflate
	ihdr.filter_method = 0;		// adaptive filtering
	ihdr.interlace_method = 0;	// no interlacing

	int buffer_size = ((height + border*2) + 1) * (width + border*2) + 15;
	unsigned char* buffer = (unsigned char*) malloc(buffer_size);
	
	memcpy(buffer, "IHDR", 4);
	memcpy(buffer + 4, &ihdr, sizeof(ihdr));
	status = fwrite(buffer, sizeof(ihdr) + 4, 1, pngfile);

	uint32_t crc32_calc = swap_uint32(crc32(buffer, sizeof(ihdr) + 4));
	status = fwrite(&crc32_calc, sizeof(crc32_calc), 1, pngfile);

	memcpy(buffer, idat_chunk, sizeof(idat_chunk));
	uint16_t len = (height + 1 + border*2) * (width + border*2);
	uint16_t nlen = ~len; // 1's complement
	memcpy(buffer + 7, &len, sizeof(uint16_t));
	memcpy(buffer + 9, &nlen, sizeof(uint16_t));

	int index = 11;

	for (int i=0; i < border; i++) { // top border
		buffer[index++] = 0; //filter byte
		for (int j = 0; j < width + (border*2); j++)
			buffer[index++] = 255;
	}

	for (int i=0; i < height; i++) {
		buffer[index++] = 0; // first byte per scanline is meaningless filter byte

		for (int j = 0; j < border; j++) // left border
			buffer[index++] = 255;

		for (int j = 0; j < width; j++) {
			buffer[index++] = image[i][j];
		}

		for (int j = 0; j < border; j++) // right border
			buffer[index++] = 255;
	}

	for (int i=0; i < border; i++) { // bottom border
		buffer[index++] = 0; //filter byte
		for (int j = 0; j < width + (border*2); j++)
			buffer[index++] = 255;
	}

	uint32_t adler32_calc = swap_uint32(adler32(&buffer[11], len));
	memcpy(buffer + 11 + len, &adler32_calc, sizeof(adler32_calc));

	chunk_length = swap_uint32(buffer_size-4);
	status = fwrite(&chunk_length, sizeof(uint32_t), 1, pngfile);
	status = fwrite(buffer, buffer_size, 1, pngfile);

	crc32_calc = swap_uint32(crc32(buffer, buffer_size));
	status = fwrite(&crc32_calc, sizeof(crc32_calc), 1, pngfile);

	status = fwrite(file_footer, sizeof(file_footer), 1, pngfile); // IEND chunk
	fclose(pngfile);

	free(buffer);
}
