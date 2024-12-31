#include "utils.h"
#include <stdint.h>

void ArrayToHexString(const unsigned char *inbuf, int len, char *outbuf)
{
    uint8_t temp;
	int i = 0;

	for (i = 0; i < len; i++) {
		temp = ((inbuf[i]&0xf0)>>4);
		outbuf[2 * i + 0] = IntToASCII(temp);
		temp = (inbuf[i]&0x0f);
		outbuf[2 * i + 1] = IntToASCII(temp);
	}
}