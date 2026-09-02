#ifndef _XECRYPTBN_H
#define _XECRYPTBN_H

#include "xecrypt.h"
#include "bign/bigdigits.h"
#include "bign/pkcs1-rsa.h"

#ifdef __cplusplus
extern "C" {
#endif

unsigned long long XeCryptLoadQuad(const unsigned char* data);
void XeCryptStoreQuad(unsigned long long qwin, unsigned char* data);

void XeCryptBnQw_Copy(const u64 *pqwInp, u64 *pqwOut, u32 cqw);
void XeCryptBnDw_Copy(const u32 *pdwInp, u32 *pdwOut, u32 cdw);
void XeCryptBnQw_Zero(u64* pqw, u32 cqw);
void XeCryptMemcpyRev(u8* dest, const u8* src, int len);
void BnQwBeBufSwap(BYTE* data, int cqw);

void display_digitt(DIGIT_T* pin, int cdw);
DIGIT_T * mpSetBeQwBigNum(int cqw, u8* data);
void mpGetBeQwBigNum(int cqw, u8* data, DIGIT_T* dig);
DIGIT_T * load32LeValBigNum(int cqw, u32 val);
u64 XeCryptBnQwNeModInv(u64 val);
u64 XeCryptMulHdu(u64 val1, u64 val2, u64* hival, u64* loval);
void XeCryptBnQwNeModMul(const u64 * pqwA, const u64 * pqwB, u64 * pqwC, u64 qwMI, const u64 * pqwM, u32 cqw);
BOOL XeCryptBnQwNeModExp(u64 *pqwOut, const u64 *pqwIn, const u64 *pqwInExp, const u64 *pqwInMod, u32 cqw);
BOOL XeCryptBnQwNeMul(u64 *pqwOut, const u64 *pqwInp1, const u64 *pqwInp2, u32 cqw);
BOOL XeCryptBnQwNeMod(const u64 *pqwInp1, const u64 *pqwInp2, u64 *pqwOut, u32 cqwIn1, u32 cqw);
int XeCryptBnQwNeCompare(const u64 *pqwA, const u64 *pqwB, u32 cqw);
BOOL XeCryptBnQwNeSub(u64 *pqwOut, const u64 *pqwInp1, const u64 *pqwInp2, u32 cqw);
BOOL XeCryptBnQwNeAdd(u64 *pqwOut, const u64 *pqwInp1, const u64 *pqwInp2, u32 cqw);

#ifdef __cplusplus
}
#endif


#endif //_XECRYPTBN_H
