/* bigdUtils.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bigdigits.h"

/* support function */
static BYTE hex2byte(char c)
{
	BYTE b;
	
	switch(c) {
	case '0': 	b = 0; break;
	case '1':	b = 1; break;
	case '2':	b = 2; break;
	case '3':	b = 3; break;
	case '4':	b = 4; break;
	case '5':	b = 5; break;
	case '6':	b = 6; break;
	case '7':	b = 7; break;
	case '8':	b = 8; break;
	case '9':	b = 9; break;
	case 'A': 	
	case 'a':	b = 10; break;
	case 'B':	
	case 'b':	b = 11; break;
	case 'C':	
	case 'c':	b = 12; break;
	case 'D':	
	case 'd':	b = 13; break;
	case 'E':	
	case 'e':	b = 14; break;
	case 'F': 	
	case 'f':	b = 15; break;
	default: b = 0;
	}
	
	return b;
}

DIGIT_T *mpMalloc(UINT ndigits)
{
	return (DIGIT_T *)malloc(NBYTE(ndigits));
}

DIGIT_T *mpMallocB(UINT nbits, UINT *ndigits)
{
	*ndigits = (nbits % BITS_PER_DIGIT) ? (nbits / BITS_PER_DIGIT) + 1 : nbits / BITS_PER_DIGIT;
	return mpMalloc(*ndigits);
}

void mpFree(DIGIT_T *p)
{
	if(p)
		free(p);
	p = NULL;
}


/* main functions */
void mpPrint(const DIGIT_T *p, UINT len)
{
	while (len--)
		printf("%08x ", p[len]);
}

void mpPrintNL(const DIGIT_T *p, UINT len)
{
	UINT i = 0;

	while (len--) {
		if ((i % 8) == 0 && i)
			printf("\n");
		printf("%08x ", p[len]);
		i++;
	}
	printf("\n");
}

void mpPrintTrim(const DIGIT_T *p, UINT len)
{
	/* Trim leading zeroes */
	while (len--)
		if (p[len] != 0)
			break;
	len++;
	while (len--)
		printf("%08x ", p[len]);
}

void mpPrintTrimNL(const DIGIT_T *p, UINT len)
{
	UINT i = 0;

	/* Trim leading zeroes */
	while (len--)
		if (p[len] != 0)
			break;
	len++;
	while (len--) {
		if ((i % 8) == 0 && i)
			printf("\n");
		printf("%08x ", p[len]);
		i++;
	}
	printf("\n");
}

void mpMakeRandom(DIGIT_T a[], UINT ndigits)
{
#if STRONG_RANDOM	
	prng((BYTE *)a, NBYTE(ndigits));
#else
	UINT i;
		
	for (i = 0; i < ndigits; i++)
		a[i] = spPseudoRand(0, MAX_DIGIT);
#endif	
}

BYTE *mpASC2BIN(const char *s, UINT len, UINT *nread)
{
	BYTE *p = NULL;
	
	/* init big integer storage */
	p = (BYTE*)malloc(len);
	memset(p, 0x00, len);
	
	/* Convert using ASCII table */
	if(len>strlen(s))
		*nread = (UINT)strlen(s);
	else
		*nread = len;
	
	memcpy(p, s, *nread);
	
	return p;
}

char *mpBIN2ASC(const BYTE *p, UINT len)
{
	char *s = NULL;
	
	/* init string storage */
	s = (char *)malloc(len+1);
	memset(s, 0x00, len+1);
	
	memcpy(s, p, len);
	
	return s;
}

BYTE *mpHex2Bin(const char *s, UINT len, UINT *nread)
{
	BYTE *p = NULL;
	UINT i;
	
	if(strlen(s) % 2){
		*nread = 0;
		return NULL;
	}
	
	p = (BYTE *)malloc(len);
	memset(p, 0x00, len);
		
	if(len < strlen(s)/2)
		*nread = 2*len;
	else
		*nread = (UINT)strlen(s);
	
	i = 0;
	while(i<*nread){
		/* read 2 characters each = 1 byte */
		p[i/2] = hex2byte(s[strlen(s)-i-1]) + (hex2byte(s[strlen(s)-i-2]) << 4);
		i+=2;
	}
	
	return p;
}

char *mpBin2Hex(const BYTE *p, UINT len)
{
	char *s = NULL;
	UINT i;
	
	s = (char *)malloc(2*len + 1);
	memset(s, 0x00, 2*len+1);
	
	i = 0;
	while (len--){
		sprintf(s+i, "%02X", p[len]);
		i+=2;
	}
	
	return s;
}

BYTE *mpDec2Bin(const char *s, UINT *nread)
{
	return NULL;
}

BYTE *mpOct2Hex(const char *s, UINT *nread)
{
	return NULL;
}

