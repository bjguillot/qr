// structure must be "packed" so that it exactly matches the on-disk file format

typedef struct __attribute__((__packed__)) {
	uint32_t width;
	uint32_t height;
	uint8_t bit_depth;
	uint8_t color_type;
	uint8_t comp_method;
	uint8_t filter_method;
	uint8_t interlace_method;
} ihdr_t;

uint32_t swap_uint32(uint32_t val);
uint32_t crc32(unsigned char *buf, int len);
uint32_t adler32(unsigned char *buf, int len);
