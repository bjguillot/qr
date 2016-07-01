/*	PNG Utilities
*	Most of these routines are copied or heavily derived from
*	the reference CRC-32 and ADLER-32 implementations.
*	No copyright is asserted for anything in this png_util.c file.
*	B.J. Guillot (bguillot@acm.org)
*	Version 1.0
*	2016-06-30
*/

#include <stdint.h>	//for uint32_t, etc.

// See reference implementation at https://www.w3.org/TR/PNG-CRCAppendix.html

uint32_t crc_table[256];	// Table of CRCs of all 8-bit messages.
int crc_table_computed = 0;	// Flag: has the table been computed? Initially false.
   
//------------------------------------------------------------------------------------------------------------

// Make the table for a fast CRC.
void make_crc_table(void) {
	uint32_t c;
	int n, k;
   
	for (n = 0; n < 256; n++) {
		c = (uint32_t) n;
		for (k = 0; k < 8; k++) {
			if (c & 1)
				c = 0xedb88320L ^ (c >> 1);
			else
				c = c >> 1;
		}
		crc_table[n] = c;
	}
	crc_table_computed = 1;
}
   
//------------------------------------------------------------------------------------------------------------

// Update a running CRC with the bytes buf[0..len-1]--the CRC
// should be initialized to all 1's, and the transmitted value
// is the 1's complement of the final running CRC (see the
// crc() routine below)).
   
uint32_t update_crc(uint32_t crc, unsigned char *buf, int len) {
	uint32_t c = crc;
	int n;
   
	if (!crc_table_computed)
		make_crc_table();
	for (n = 0; n < len; n++) {
		c = crc_table[(c ^ buf[n]) & 0xff] ^ (c >> 8);
	}
	return c;
}
   
//------------------------------------------------------------------------------------------------------------

// Return the CRC-32 of the bytes buf[0..len-1].
uint32_t crc32(unsigned char *buf, int len) {
	return update_crc(0xffffffffL, buf, len) ^ 0xffffffffL;
}

//------------------------------------------------------------------------------------------------------------

// Copied from reference implementation at https://www.ietf.org/rfc/rfc1950.txt
uint32_t update_adler(uint32_t adler, unsigned char *buf, int len) {
	uint32_t s1 = adler & 0xffff;
	uint32_t s2 = (adler >> 16) & 0xffff;
	int n;

	for (n = 0; n < len; n++) {
		s1 = (s1 + buf[n]) % 65521;
		s2 = (s2 + s1)     % 65521;
	}
	return (s2 << 16) + s1;
}

//------------------------------------------------------------------------------------------------------------

// Return the ADLER-32 of the bytes buf[0..len-1].
uint32_t adler32(unsigned char *buf, int len) {
	return update_adler(1, buf, len);
}

//------------------------------------------------------------------------------------------------------------

// Byte swap unsigned int
uint32_t swap_uint32(uint32_t val) {
	val = ((val << 8) & 0xFF00FF00 ) | ((val >> 8) & 0xFF00FF );
	return (val << 16) | (val >> 16);
}

//------------------------------------------------------------------------------------------------------------
