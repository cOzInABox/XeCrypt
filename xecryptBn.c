#include <stdio.h>
#include "xecryptTypes.h"
#include "xecrypt.h"
#include "xecryptBn.h"

u32 xcgetBeU32(void* prt)
{
	unsigned char* ptr = (unsigned char*)prt;
	u32 ret = (ptr[0]&0xFF)<<24;
	ret |= (ptr[1]&0xFF)<<16;
	ret |= (ptr[2]&0xFF)<<8;
	ret |= ptr[3]&0xFF;
	return ret;
}

unsigned long long xcgetBeU64(void* prt)
{
	unsigned char* ptr = (unsigned char*)prt;
	u64 res = xcgetBeU32(ptr);
	res = res << 32;
	res |= (xcgetBeU32(ptr+4)&0xFFFFFFFF);
	return res;
}

void xcsetBeU32(u32 val, void* prt)
{
	unsigned char* ptr = (unsigned char*)prt;
	ptr[0] = (val>>24)&0xFF;
	ptr[1] = (val>>16)&0xFF;
	ptr[2] = (val>>8)&0xFF;
	ptr[3] = val&0xFF;
}

void xcsetBeU64(unsigned long long num, void* prt)
{
	int i;
	unsigned char* ptr = (unsigned char*)prt;
	for(i = 0; i < 8; i++)
	{
		ptr[7-i] = num&0xFF;
		num = num>>8;
	}
}

unsigned long long XeCryptLoadQuad(const unsigned char* data)
{
	int i;
	u64 quad = 0;
	if(((uintptr_t)data&7)==0) // check alignment
	{
		u64* pQaud = (u64*)data;
		return bswap64(pQaud[0]); // thanks Ced!
	}
	for(i=0; i<8; i++)
	{
		quad = (quad<<8)|(data[i]&0xFF);
	}
	return quad;
}

void XeCryptStoreQuad(u64 qwin, unsigned char* data)
{
	int i;
	if(((uintptr_t)data&7)==0) // check alignment
	{
		u64* pQaud = (u64*)data;
		pQaud[0] = bswap64(qwin);  // thanks Ced!
		return;
	}
	for(i=8; i>0; i--)
	{
		data[i-1] = (qwin&0xFF);
		qwin = (qwin>>8);
	}
}

void XeCryptBnQw_Copy(const u64 *pqwInp, u64 *pqwOut, u32 cqw)
{
	if((cqw != 0)&&(pqwInp != pqwOut))
	{
		u32 i;
		for(i = 0; i < cqw; i++)
		{
			pqwOut[i] = pqwInp[i];
		}
	}
}

void XeCryptBnDw_Copy(const u32 *pdwInp, u32 *pdwOut, u32 cdw)
{
	if((cdw != 0)&&(pdwInp != pdwOut))
	{
		u32 i;
		for(i = 0; i < cdw; i++)
		{
			pdwOut[i] = pdwInp[i];
		}
	}
}

void XeCryptBnQw_Zero(u64* pqw, u32 cqw)
{
	if((pqw != NULL)&&(cqw != 0))
	{
		u32 i;
		for(i = 0; i < cqw; i++)
		{
			pqw[i] = 0ULL;
		}
	}
}

void XeCryptMemcpyRev(u8* dest, const u8* src, int len)
{
	int i, j;
	for(i=0, j=len-1; i<len; i++, j--)
	{
		dest[i]=src[j];
	}
}

void BnQwBeBufSwap(BYTE* data, int cqw)
{
	int i;
	BYTE* pstart = &data[0];
	BYTE* pend = &data[(cqw-1)*8];
	for(i = 0; i < (cqw/2); i++)
	{
		u64 val1 = xcgetBeU64(pstart);
		u64 val2 = xcgetBeU64(pend);
		xcsetBeU64(val1, pend);
		xcsetBeU64(val2, pstart);
		pend -= 8;
		pstart += 8;
	}
}

void display_digitt(DIGIT_T* pin, int cdw)
{
	int i;
	printf("   ");
	for(i = 0; i < cdw; i++)
	{
		if((i%4) == 0)
			printf("\n   ");
		printf("0x%02X, 0x%02X, 0x%02X, 0x%02X, ", (pin[i]>>24)&0xFF, (pin[i]>>16)&0xFF, (pin[i]>>8)&0xFF, pin[i]&0xFF);
	}
	printf("\n");
}

// convert xbox big number 64 BE in byte buffer to bigdigits 32 LE stored in DIGIT_T
DIGIT_T * mpSetBeQwBigNum(int cqw, u8* data)
{
	DIGIT_T * rval = mpMalloc((cqw*2));
	if(rval != NULL)
	{
		int i;
		mpSetZero(rval, (cqw*2));
		for(i = 0; i < cqw*2; i += 2)
		{
			rval[i+1] = xcgetBeU32(&data[i*4]);
			rval[i] = xcgetBeU32(&data[(i+1)*4]);
		}
	}
	return rval;
}

void mpGetBeQwBigNum(int cqw, u8* data, DIGIT_T* dig)
{
	if((data != NULL) && (dig != NULL) && (cqw != 0))
	{
		int i;
		for(i = 0; i < cqw*2; i += 2)
		{
			xcsetBeU32(dig[i+1], &data[i*4]);
			xcsetBeU32(dig[i], &data[(i+1)*4]);
		}
	}
}

DIGIT_T * load32LeValBigNum(int cqw, u32 val)
{
	DIGIT_T * rval = mpMalloc((cqw*4));
	if(rval != NULL)
	{
		mpSetZero(rval, (cqw*4));
		rval[0] = val;
	}
	rval[cqw*2] = 0xF1F1F1F1;
	return rval;
}

u64 XeCryptBnQwNeModInv(u64 val)
{
	int i;
	u64 t1, t2, t3;
	t1 = val *3;
	t2 = t1 ^ 2;
	t1 = t2 * val;
	t1 = (~t1)+2;
	for(i = 5; i < 0x20; i <<= 1)
	{
		t3 = t1 + 1;
		t2 = t3 * t2;
		t1 = t1* t1;
	}
	t1 = t1 + 1;
	return (t1*t2);
}

// replaces 64 bit opcode that isn't apparent on PC
u64 XeCryptMulHdu(u64 val1, u64 val2, u64* hival, u64* loval)
{
	u64 ret;
	DIGIT_T v1[2];
	DIGIT_T v2[2];
	DIGIT_T	a[4];
	v1[0] = val1&0xFFFFFFFF;
	v1[1] = (val1 >> 32)&0xFFFFFFFF;
	v2[0] = val2&0xFFFFFFFF;
	v2[1] = (val2 >> 32)&0xFFFFFFFF;
	mpMultiply(a, v1, v2, 2);
	if(loval)
	{
		ret = a[1];
		ret = ret << 32;
		ret |= a[0]&0xFFFFFFFF;
		*loval = ret;
	}
	ret = a[3];
	ret = ret << 32;
	ret |= a[2]&0xFFFFFFFF;
	if(hival)
		*hival = ret;
	return ret;
}

void XeCryptBnQwNeModMul(const u64 * pqwA, const u64 * pqwB, u64 * pqwC, u64 qwMI, const u64 * pqwM, u32 cqw)
{
	u32 i, j;
	u64 buf1[0x21];
	u64 buf2[0x21];
	u64 mmiStat;
	XeCryptBnQw_Zero(buf1, 0x21);
	XeCryptBnQw_Zero(buf2, 0x21);

	mmiStat = qwMI*bswap64(pqwA[0]);
	for(i = 0; i < cqw; i++)
	{
		// outer_first:
		u64 mmi = (mmiStat*bswap64(pqwB[i]))+(qwMI*(buf1[1] - buf2[1]));
		u64 acc1 = 0;
		u64 acc2 = 0;

		for(j = 0; j < cqw; j++)
		{
			u64 hiVal, loVal;
			XeCryptMulHdu(bswap64(pqwB[i]), bswap64(pqwA[j]), &hiVal, &loVal);
			loVal = loVal+buf1[j+1];
			if(loVal < buf1[j+1])
				hiVal++;
			loVal = loVal+acc1;
			if(loVal < acc1)
				hiVal++;
			acc1 = hiVal;
			buf1[j] = loVal;

			XeCryptMulHdu(mmi, bswap64(pqwM[j]), &hiVal, &loVal);
			loVal = loVal+buf2[j+1];
			if(loVal < buf2[j+1])
				hiVal++;
			loVal = loVal+acc2;
			if(loVal < acc2)
				hiVal++;
			acc2 = hiVal;
			buf2[j] = loVal;
		}
		buf1[cqw] = acc1;
		buf2[cqw] = acc2;
	}
	for(i = 0; i < cqw; i++)
	{
		if(buf1[cqw-i] >  buf2[cqw-i])
		{
			u64 car = 0;
			for(j = 0; j < cqw; j++)
			{
				u64 val = (buf1[j+1] - buf2[j+1]) - car;
				pqwC[j] = bswap64(val);
				val = (val ^ buf1[j+1]) | (buf2[j+1] ^ buf1[j+1]);
				car = (buf1[j+1] ^ val) >> 63;
			}
			return;
		}
		if(buf1[cqw-i] <  buf2[cqw-i])
		{
			u64 car1 = 0, car2 = 0;
			for(j = 0; j < cqw; j++)
			{
				u64 val1 = bswap64(pqwM[j]);
				u64 val2 = (buf1[j+1] + val1) + car1;
				u64 val3 = (val2 - buf2[j+1]) - car2;
				pqwC[j] = bswap64(val3);
				val1 = val1 ^ val2;
				val3 = val3 ^ val2;
				car1 = (((buf1[j+1] ^ val2) | val1) ^ val2) >> 63;
				car2 = (((buf2[j+1] ^ val2) | val3) ^ val2) >> 63;
			}
			return;
		}
	}
}

// pqwOut = pqwIn ^ pqwInExp mod (pqwInMod)
BOOL XeCryptBnQwNeModExp(u64 *pqwOut, const u64 *pqwIn, const u64 *pqwInExp, const u64 *pqwInMod, u32 cqw)
{
	BOOL ret = FALSE;
	DIGIT_T* dtOut = load32LeValBigNum(cqw, 0);
	DIGIT_T* dtIn = mpSetBeQwBigNum(cqw, (u8*)pqwIn);
	DIGIT_T* dtInExp = mpSetBeQwBigNum(cqw, (u8*)pqwInExp);
	DIGIT_T* dtInMod = mpSetBeQwBigNum(cqw, (u8*)pqwInMod);
	if((dtOut != NULL) && (dtIn != NULL) && (dtInExp != NULL) && (dtInMod != NULL))
	{
		if(mpModExp(dtOut, dtIn, dtInExp, dtInMod, cqw*2) == 0)
		{
			mpGetBeQwBigNum(cqw, (u8*)pqwOut, dtOut);
			ret = TRUE;
		}
	}
	if(dtOut) mpFree(dtOut);
	if(dtIn) mpFree(dtIn);
	if(dtInExp) mpFree(dtInExp);
	if(dtInMod) mpFree(dtInMod);
	return ret;
}

// pqwOut = pqwInp1*pqwInp2 (note that pqwInp2 has size cqw, pqwInp1 must be cqw*2 in size!)
BOOL XeCryptBnQwNeMul(u64 *pqwOut, const u64 *pqwInp1, const u64 *pqwInp2, u32 cqw)
{
	BOOL ret = FALSE;
	DIGIT_T* dtOut = load32LeValBigNum(cqw*2, 0);
	DIGIT_T* dtInp1 = mpSetBeQwBigNum(cqw, (u8*)pqwInp1);
	DIGIT_T* dtInp2 = mpSetBeQwBigNum(cqw, (u8*)pqwInp2);
	if((dtOut != NULL) && (dtInp1 != NULL) && (dtInp2 != NULL))
	{
		if(mpMultiply(dtOut, dtInp1, dtInp2, cqw*2) == 0)
		{
			mpGetBeQwBigNum(cqw*2, (u8*)pqwOut, dtOut);
			ret = TRUE;
		}
	}
	if(dtOut) mpFree(dtOut);
	if(dtInp1) mpFree(dtInp1);
	if(dtInp2) mpFree(dtInp2);
	return ret;
}

// pqwOut = pqwInp1 % pqwInp2
// pqwInp1 is cqwIn1 in size, the others are cqw in size
BOOL XeCryptBnQwNeMod(const u64 *pqwInp1, const u64 *pqwInp2, u64 *pqwOut, u32 cqwIn1, u32 cqw)
{
	BOOL ret = FALSE;
	DIGIT_T* dtOut = load32LeValBigNum(cqw, 0);
	DIGIT_T* dtInp1 = mpSetBeQwBigNum(cqwIn1, (u8*)pqwInp1);
	DIGIT_T* dtInp2 = mpSetBeQwBigNum(cqw, (u8*)pqwInp2);
	if((dtOut != NULL) && (dtInp1 != NULL) && (dtInp2 != NULL))
	{
		if(mpModulo(dtOut, dtInp1, cqwIn1*2, dtInp2, cqw*2) == 0)
		{
			mpGetBeQwBigNum(cqw, (u8*)pqwOut, dtOut);
			ret = TRUE;
		}
	}
	if(dtOut) mpFree(dtOut);
	if(dtInp1) mpFree(dtInp1);
	if(dtInp2) mpFree(dtInp2);
	return ret;
}

int XeCryptBnQwNeCompare(const u64 *pqwA, const u64 *pqwB, u32 cqw)
{
	u32 i;
	for(i = cqw-1; i >= 0; i--)
	{
		u64 valA = bswap64(pqwA[i]);
		u64 valB = bswap64(pqwB[i]);
		if(valA != valB)
		{
			if(valA > valB)
				return 1;
			return -1;
		}
	}
	return 0;
}

// returns true when result is negative (pqwInp2 > pqwInp1)
BOOL XeCryptBnQwNeSub(u64 *pqwOut, const u64 *pqwInp1, const u64 *pqwInp2, u32 cqw)
{
	BOOL ret = FALSE;
	DIGIT_T* dtOut = load32LeValBigNum(cqw, 0);
	DIGIT_T* dtInp1 = mpSetBeQwBigNum(cqw, (u8*)pqwInp1);
	DIGIT_T* dtInp2 = mpSetBeQwBigNum(cqw, (u8*)pqwInp2);
	if((dtInp1 != NULL) && (dtInp2 != NULL))
	{
		if(mpSubtract(dtOut, dtInp1, dtInp2, cqw*2))
			ret = TRUE;
		mpGetBeQwBigNum(cqw, (u8*)pqwOut, dtOut);
	}
	if(dtOut) mpFree(dtOut);
	if(dtInp1) mpFree(dtInp1);
	if(dtInp2) mpFree(dtInp2);
	return ret;
}

// returns true on carry if overflow
BOOL XeCryptBnQwNeAdd(u64 *pqwOut, const u64 *pqwInp1, const u64 *pqwInp2, u32 cqw)
{
	BOOL ret = FALSE;
	DIGIT_T* dtOut = load32LeValBigNum(cqw, 0);
	DIGIT_T* dtInp1 = mpSetBeQwBigNum(cqw, (u8*)pqwInp1);
	DIGIT_T* dtInp2 = mpSetBeQwBigNum(cqw, (u8*)pqwInp2);
	if((dtInp1 != NULL) && (dtInp2 != NULL))
	{
		if(mpAdd(dtOut, dtInp1, dtInp2, cqw*2))
			ret = TRUE;
		mpGetBeQwBigNum(cqw, (u8*)pqwOut, dtOut);
	}
	if(dtOut) mpFree(dtOut);
	if(dtInp1) mpFree(dtInp1);
	if(dtInp2) mpFree(dtInp2);
	return ret;
}