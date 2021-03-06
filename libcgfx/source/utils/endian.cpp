#include <utils/endian.h>

void endianSwap(uint16_t& u16) {
	uint8_t high = u16 >> 8;
	uint8_t low  = u16 & 0xff;
	u16 = (low << 8) | high;
}

void endianSwap(uint32_t& u32) {
	uint8_t b3 = (u32 >> 24) & 0xff;
	uint8_t b2 = (u32 >> 16) & 0xff;
	uint8_t b1 = (u32 >>  8) & 0xff;
	uint8_t b0 =  u32        & 0xff;
	u32 = (b0 << 24) | (b1 << 16) | (b2 << 8) | b3;
}

void endianSwap(float& fl) {
	char* flbytes = (char*)&fl;
	char flcopy[4] = { flbytes[0], flbytes[1], flbytes[2], flbytes[3] };
	flbytes[0] = flcopy[3];
	flbytes[1] = flcopy[2];
	flbytes[2] = flcopy[1];
	flbytes[3] = flcopy[0];
}