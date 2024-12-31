#ifndef _UTILS_H__
#define _UTILS_H__

#define	IntToASCII(c)	(c)>9?((c-10)+'A'):((c)+'0');

/// @brief 将字节数组转化为十六进制的字符串
void ArrayToHexString(const unsigned char *inbuf, int len, char *outbuf);

#endif