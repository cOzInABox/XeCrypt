/* Base64 Encode/Decode Functions */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bigdigits.h"

/* base64 encoding table */
static const char *base64 = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/* base64 decoding table */
static const char dectab[256] = {	
   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63,
   52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, 64, -1, -1,
   -1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
   15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1,
   -1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
   41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1,
   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 
}; 


char *mpBase64Encode(DIGIT_T *p, UINT len)
{
	BYTE *pp;
	char *str;
	UINT i;
	UINT j;
	UINT k;
	UINT nbyte;
	UINT slen;
	DWORD b;
	DWORD mask = 0x003F;
	UINT idx[4];
	
	nbyte = len*BITS_PER_DIGIT / 8;
	
	/* make input data 3-byte blocks */
	if(nbyte % 3) {
		nbyte = nbyte + 3 - (nbyte % 3);
		pp = (BYTE *)malloc(nbyte);
		memset(pp, 0x00, nbyte);
		memcpy(pp, p, len*BITS_PER_DIGIT / 8);	
	}
	else
		pp = (BYTE *)p;
		
	/* init output string */
	slen = 4*nbyte/3;
	str = (char *)malloc(slen+1);
	memset(str, 0x00, slen+1);
	
	i = 0;
	k = 0;		
	while(i<nbyte){
		/* convert 3 bytes = 24 bits each */
		b = 0x0000;
		for(j=0;j<3;j++)
			b |= *(pp+i+j) << (j*8);
		
		/* split into four 6-bit codes and append to output string */
		for(j=0;j<4;j++){
			idx[j] = (b & (mask << (j*6))) >> (j*6);
			*(str+j+k) = *(base64+idx[j]);
		}
		
		/* Ouput Next 4 digits */
		k+=4;
		
		/* Next 3 bytes */
		i+=3;
		
	}
	
	free(pp);

	return str;
}

DIGIT_T *mpBase64Decode(UINT *len, char *str)
{
	BYTE *p;
	int i;
	UINT j;
	UINT k;
	DWORD b;
	UINT nbyte;
	DWORD mask = 0x00FF;
	
	if((strlen(str) % 4)){
		*len = 0;
		return NULL;
	}
	
	/* init output big integer */
	nbyte = (UINT)strlen(str)*3/4;
	p = (BYTE *)malloc(nbyte);
	memset(p, 0x00, nbyte);
	
	*len = nbyte / (BITS_PER_DIGIT / 8);
	
	
	i = 0;
	k = 0;
	while(i < (int)strlen(str)){
		/* convert each 4 digits to 3 bytes */
		b = 0x0000;
		for(j=0;j<4;j++)
			b |= (((BYTE)dectab[(BYTE)*(str+i+j)]) << (6*j));
		
		for(j=0;j<3;j++)
			*(p+k+j) = (BYTE)((b >> (j*8)) & mask); 
		
		/* next 3 bytes */
		k+= 3;
		
		/* next 4 digits */
		i+= 4;
	}
	
	return (DIGIT_T *)p;	
}

