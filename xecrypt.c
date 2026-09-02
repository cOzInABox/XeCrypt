#ifdef _MSC_VER
#define _CRT_RAND_S
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef _MSC_VER
#include <time.h>
#else
#include <windows.h>
#endif
#include "xecryptTypes.h"
#include "xecrypt.h"
#include "xecryptBn.h"


// on 360 this function loads a quad, then stores two u32s byte reversed.. the effect is endian reversed quad words
void XeCryptBnQw_SwapDwQwLeBe(const unsigned char * pqwInp, unsigned char * pqwOut, unsigned int cqw)
{
	unsigned int i;
	unsigned long long temp;
	for(i = 0; i < cqw; i++)
	{
		temp = bswap64(XeCryptLoadQuad(&pqwInp[i*8]));
		XeCryptStoreQuad(temp, &pqwOut[i*8]);
	}
}

void XeCryptUidEccEncode(u8* pbaCpuKey)
{
	int cnt = 0;
	u32 acc1 = 0, acc2 = 0, dwTmp;
	u8 bTmp;
	for(; cnt < 0x80; cnt++, acc1 >>= 1)
	{
		bTmp = pbaCpuKey[(cnt >> 3)];
		dwTmp = (bTmp >> (cnt & 7)) & 1;
		if(cnt < 0x6A)
		{
			acc1 = dwTmp ^ acc1;
			if(acc1 & 1)
				acc1 = acc1 ^ 0x360325;
			acc2 = dwTmp ^ acc2;
		}
		else if(cnt < 0x7F)
		{
			if(dwTmp != (acc1 & 1))
				pbaCpuKey[(cnt >> 3)] = ((1 << (cnt & 7)) ^ (bTmp & 0xFF));
			acc2 = (acc1 & 1) ^ acc2;
		}
		else if(dwTmp != acc2)
			pbaCpuKey[0xF] = (0x80 ^ bTmp) & 0xFF;
	}
}

int XeCryptHammingWeight(u8* data, int len)
{
	int i, j, wght = 0;
	for(i = 0; i < len; i++)
	{
		u8 val = data[i];
		for(j = 0; j < 8; j++)
		{
			wght += val&1;
			val >>= 1;
		}
	}
	return wght;
}

void XeCryptHmacSha(const unsigned char *pbKey, unsigned int cbKey, const unsigned char *pbInp1, unsigned int cbInp1, const unsigned char *pbInp2, unsigned int cbInp2, const unsigned char *pbInp3, unsigned int cbInp3, unsigned char *pbOut, unsigned int cbOut)
{
	int i;
	SHA_CTX Ctx;
	unsigned char Digest[SHA_DIGEST_LENGTH];
	unsigned char K[XECRYPT_HMAC_SHA_MAX_KEY_SZ];
	unsigned char  OPad[(SHA_DIGEST_LENGTH + XECRYPT_HMAC_SHA_MAX_KEY_SZ)];
	unsigned char *IPad;
	unsigned int Length = (cbInp1 + cbInp2 + cbInp3);
	IPad = (unsigned char*)malloc(Length + XECRYPT_HMAC_SHA_MAX_KEY_SZ);

	//pad Key out to block size, but don't exceed it
	memset(K, 0, XECRYPT_HMAC_SHA_MAX_KEY_SZ);
	if (cbKey <= XECRYPT_HMAC_SHA_MAX_KEY_SZ)
		memcpy(K, pbKey, cbKey);
	else
		memcpy(K, pbKey, XECRYPT_HMAC_SHA_MAX_KEY_SZ);

	for(i = 0; i < XECRYPT_HMAC_SHA_MAX_KEY_SZ; i++)
	{
		OPad[i] = K[i] ^ 0x5C;
		IPad[i] = K[i] ^ 0x36;
	}

	if(cbInp1 !=0)
		memcpy(&IPad[XECRYPT_HMAC_SHA_MAX_KEY_SZ], pbInp1, cbInp1);
	if(cbInp2 !=0)
		memcpy(&IPad[(XECRYPT_HMAC_SHA_MAX_KEY_SZ + cbInp1)], pbInp2, cbInp2);
	if(cbInp3 !=0)
		memcpy(&IPad[(XECRYPT_HMAC_SHA_MAX_KEY_SZ  + cbInp1 + cbInp2)], pbInp3, cbInp3);
	SHA1_Init(&Ctx);
	SHA1_Update(&Ctx, IPad, (Length + XECRYPT_HMAC_SHA_MAX_KEY_SZ));
	SHA1_Final(Digest, &Ctx);

	memcpy(&OPad[XECRYPT_HMAC_SHA_MAX_KEY_SZ], Digest, SHA_DIGEST_LENGTH);
	SHA1_Init(&Ctx);
	SHA1_Update(&Ctx, OPad, (SHA_DIGEST_LENGTH + XECRYPT_HMAC_SHA_MAX_KEY_SZ));
	SHA1_Final(Digest, &Ctx);

	if(cbOut <= SHA_DIGEST_LENGTH)
		memcpy(pbOut, Digest, cbOut);
	else
		memcpy(pbOut, Digest, SHA_DIGEST_LENGTH);
	free(IPad);
}

void XeCryptRc4(unsigned char *pbKey, unsigned int cbKey, unsigned char *pbInpOut, unsigned int cbInpOut)
{
	RC4_KEY	RC4Key;
	RC4_set_key(&RC4Key, cbKey, pbKey);
	RC4(&RC4Key, cbInpOut, pbInpOut, pbInpOut);
}

void XeCryptRc4Key(PXECRYPT_RC4_STATE pRc4State, unsigned char *pbKey, unsigned int cbKey)
{
	RC4_set_key(pRc4State, cbKey, pbKey);
}

void XeCryptRc4Ecb(PXECRYPT_RC4_STATE pRc4State, unsigned char *pbInpOut, unsigned int cbInpOut)
{
	RC4(pRc4State, cbInpOut, pbInpOut, pbInpOut);
}

BOOL XeRandomIsInit = FALSE;
RC4_KEY XeCryptRandomState;
void XeCryptRandomInit(void)
{
	unsigned char pb[0x10];
	unsigned int i, ran;
#ifndef _MSC_VER
	srand(time(NULL));
#endif //_MSC_VER

	for(i=0; i < 0x10; i++)
	{
#ifdef _MSC_VER
		rand_s(&ran);
#else
		ran = rand();
#endif // _MSC_VER
		pb[i] = (ran&0xFF);
	}
	XeCryptRc4Key(&XeCryptRandomState, pb, 0x10);
	XeRandomIsInit = TRUE;
}

// xbox actually uses Rc4Ecb against a per-session PRNG seeded random RC4 state to make random bytes
void XeCryptRandom(unsigned char *pb, unsigned int cb)
{
	unsigned int count, remain;
	count = (cb / 0x10) * 0x10;
	remain = cb % 0x10;

	if(XeRandomIsInit == FALSE)
		XeCryptRandomInit();
	memset(pb, 0, cb);

	XeCryptRc4Ecb(&XeCryptRandomState, pb, count);
	if (remain != 0)
	{
		unsigned char out[0x10];
		memset(out, 0, 0x10);
		XeCryptRc4Ecb(&XeCryptRandomState, out, 0x10);
		memcpy(&pb[count], out, remain);
	}
}

void XeCryptAesKey(PXECRYPT_AES_STATE pAesState, const unsigned char * pbKey)
{
	aes_encrypt_key(pbKey, XECRYPT_AES_KEY_SIZE, &pAesState->encCtx);
	aes_decrypt_key(pbKey, XECRYPT_AES_KEY_SIZE, &pAesState->decCtx);
}

void XeCryptAesEcb(PXECRYPT_AES_STATE pAesState, unsigned char * pbInp, unsigned char * pbOut, BOOL fEncrypt)
{
	if(fEncrypt)
		aes_ecb_encrypt(pbInp, pbOut, XECRYPT_AES_BLOCK_SIZE, &pAesState->encCtx);
	else
		aes_ecb_decrypt(pbInp, pbOut, XECRYPT_AES_BLOCK_SIZE, &pAesState->decCtx);
}

void XeCryptAesCbc(PXECRYPT_AES_STATE pAesState, unsigned char * pbInp, unsigned int cbInp, unsigned char * pbOut, unsigned char * pbFeed, BOOL fEncrypt)
{
	if(fEncrypt)
		aes_cbc_encrypt(pbInp, pbOut, cbInp, pbFeed, &pAesState->encCtx);
	else
		aes_cbc_decrypt(pbInp, pbOut, cbInp, pbFeed, &pAesState->decCtx);
}

void XeCryptShaInit(PXECRYPT_SHA_STATE pShaState)
{
	SHA1_Init(pShaState);
}

void XeCryptShaUpdate(PXECRYPT_SHA_STATE pShaState, unsigned char * pbInp, unsigned int cbInp)
{
	if(cbInp != 0)
	{
		unsigned char* pData = (unsigned char*)malloc(cbInp);
		memcpy(pData, pbInp, cbInp);
		SHA1_Update(pShaState, pData, cbInp);
		free(pData);
	}
}

void XeCryptShaFinal(PXECRYPT_SHA_STATE pShaState, unsigned char * pbOut, unsigned int cbOut)
{
	unsigned char shaout[XECRYPT_SHA_DIGEST_SIZE];
	SHA1_Final(shaout, pShaState);
	if(cbOut <= SHA_DIGEST_LENGTH)
		memcpy(pbOut, shaout, cbOut);
	else
		memcpy(pbOut, shaout, SHA_DIGEST_LENGTH);
}

void XeCryptSha(unsigned char * pbInp1, unsigned int cbInp1, unsigned char * pbInp2, unsigned int cbInp2, unsigned char * pbInp3, unsigned int cbInp3, unsigned char * pbOut, unsigned int cbOut)
{
	XECRYPT_SHA_STATE ctx;
	XeCryptShaInit(&ctx);
	if(cbInp1 != 0)
		XeCryptShaUpdate(&ctx, pbInp1, cbInp1);
	if(cbInp2 != 0)
		XeCryptShaUpdate(&ctx, pbInp2, cbInp2);
	if(cbInp3 != 0)
		XeCryptShaUpdate(&ctx, pbInp3, cbInp3);
	XeCryptShaFinal(&ctx, pbOut, cbOut);
}

void XeCryptRotSum(unsigned char *pbOut, unsigned char *pbInp, unsigned int cqwInp)
{
	unsigned int i;
	if(cqwInp != 0)
	{
		unsigned long long qw1 = XeCryptLoadQuad(&pbOut[0]); // loads existing rotsum state
		unsigned long long qw2 = XeCryptLoadQuad(&pbOut[8]);
		unsigned long long qw3 = XeCryptLoadQuad(&pbOut[0x10]);
		unsigned long long qw4 = XeCryptLoadQuad(&pbOut[0x18]);
		for(i = 0; i < cqwInp; i++)
		{
			unsigned long long tqw1 = XeCryptLoadQuad(&pbInp[i*8]); // loads data from input buffer
			unsigned long long tqw2 = tqw1+qw2;
			if(tqw2 < tqw1)
				qw2 = 1;
			else
				qw2 = 0;
			qw4 = (~tqw1) + qw4 +1; // subf qw4, tqw1, qw4
			qw1 = qw2+qw1;
			qw2 = ((tqw2<<29)&0xFFFFFFFFE0000000ULL)|((tqw2>>35)&0x1FFFFFFFULL); // expensive rotate
			if(qw4 > tqw1)
				tqw1 = 1;
			else
				tqw1 = 0;
			qw3 = (~tqw1)+qw3+1; // subf qw3, tqw1, qw3
			qw4 = ((qw4 << 31) &0xFFFFFFFF80000000ULL)|((qw4 >> 33) &0x7FFFFFFFULL); // expensive rotate
		}
		XeCryptStoreQuad(qw1, &pbOut[0]); // stores rotsum state
		XeCryptStoreQuad(qw2, &pbOut[8]);
		XeCryptStoreQuad(qw3, &pbOut[0x10]);
		XeCryptStoreQuad(qw4, &pbOut[0x18]);
	}
}

void XeCryptRotSumSha(unsigned char * pbInp1, unsigned int cbInp1, unsigned char * pbInp2, unsigned int cbInp2, unsigned char * pbOut, unsigned int cbOut)
{
	int i;
	XECRYPT_SHA_STATE ctx;
	unsigned char bRotSum[XECRYPT_ROTSUM_DIGEST_SIZE];
	memset(bRotSum, 0x0, XECRYPT_ROTSUM_DIGEST_SIZE);
	XeCryptRotSum(bRotSum, pbInp1, (cbInp1/8));
	XeCryptRotSum(bRotSum, pbInp2, (cbInp2/8));
	XeCryptShaInit(&ctx);
	XeCryptShaUpdate(&ctx, bRotSum, XECRYPT_ROTSUM_DIGEST_SIZE);
	XeCryptShaUpdate(&ctx, bRotSum, XECRYPT_ROTSUM_DIGEST_SIZE);
	XeCryptShaUpdate(&ctx, pbInp1, cbInp1);
	XeCryptShaUpdate(&ctx, pbInp2, cbInp2);
	for(i = 0; i < XECRYPT_ROTSUM_DIGEST_SIZE; i++)// invert the bits in the rotsum buffer
		bRotSum[i] = ~bRotSum[i];
	XeCryptShaUpdate(&ctx, bRotSum, XECRYPT_ROTSUM_DIGEST_SIZE);
	XeCryptShaUpdate(&ctx, bRotSum, XECRYPT_ROTSUM_DIGEST_SIZE);
	XeCryptShaFinal(&ctx, pbOut, cbOut);
}
 
void XeCryptBnDwLePkcs1Format(const u8* pbHash, u32 dwType, u8* pbSig, u32 cbSig)
{
	if((cbSig < 0x27)||(cbSig > 0x200))
		return;
	memset(pbSig, 0xFF, cbSig);
	pbSig[cbSig-1] = 0;
	pbSig[cbSig-2] = 1;
	XeCryptMemcpyRev(pbSig, pbHash, 0x14);
	if(dwType == 0)
	{
		unsigned char tbuf[]= {0x14, 0x04, 0x00, 0x05, 0x1A, 0x02, 0x03, 0x0E, 0x2B, 0x05, 0x06, 0x09, 0x30, 0x21, 0x30, 0x00};
		memcpy(&pbSig[0x14], tbuf, sizeof(tbuf));
	}
	else if(dwType == 1)
	{
		unsigned char tbuf[]= {0x14, 0x04, 0x1A, 0x02, 0x03, 0x0E, 0x2B, 0x05, 0x06, 0x07, 0x30, 0x1F, 0x30, 0x00};
		memcpy(&pbSig[0x14], tbuf, sizeof(tbuf));
	}
	else
		pbSig[0x14] = 0;
}

void XeCryptBnQwBeSigFormat(PXECRYPT_SIG pSig, const u8* pbHash, const u8* pbSalt)
{
	XECRYPT_SHA_STATE ctxsha;
	XECRYPT_RC4_STATE ctxrc4;
	unsigned char* sig; 
	sig = (unsigned char*)pSig;
	memset(pSig->aqwPad, 0x0, 28*8); // zero padding
	memcpy(pSig->abSalt, pbSalt, 0xA);// copy in salt
	pSig->bEnd = 0xBC;
	pSig->bOne = 1;
	XeCryptShaInit(&ctxsha);
	XeCryptShaUpdate(&ctxsha, (unsigned char*)pSig->aqwPad, 8);
	XeCryptShaUpdate(&ctxsha, (unsigned char*)pbHash, 0x14);
	XeCryptShaUpdate(&ctxsha, (unsigned char*)pbSalt, 0xA);
	XeCryptShaFinal(&ctxsha, pSig->abHash, 0x14);
	XeCryptRc4Key(&ctxrc4, pSig->abHash, 0x14);
	XeCryptRc4Ecb(&ctxrc4, sig, 0xEB);
	sig[0] &= 0x7F;
	BnQwBeBufSwap(sig, 0x100/8);
}

void XeCryptBnQwBeSigCompute(PXECRYPT_SIG pSig, const u8* pbHash, const u8* pbSalt, const PXECRYPT_RSA pRsa, u8* pbDecSig, u8* pbComSig)
{
	u32 exp = bswap32(pRsa->dwPubExp);
	u32 cnt = bswap32(pRsa->cqw);
	PXECRYPT_RSAPUB_4096 keyp = (PXECRYPT_RSAPUB_4096)pRsa;
	u64 mod_inv = XeCryptBnQwNeModInv(bswap64(keyp->aqwM[0]));

	XeCryptBnQw_Copy((u64*)pSig, (u64*)pbDecSig, cnt);
	XeCryptBnQw_Copy((u64*)pbDecSig, (u64*)pbComSig, cnt);
	while(exp >>= 1)
		XeCryptBnQwNeModMul((u64*)pbComSig, (u64*)pbComSig, (u64*)pbComSig, mod_inv, keyp->aqwM, cnt);
	XeCryptBnQwNeModMul((u64*)pbComSig, (u64*)pbDecSig, (u64*)pbDecSig, mod_inv, keyp->aqwM, cnt);
	// do sigformat on the hash
	XeCryptBnQwBeSigFormat((PXECRYPT_SIG)pbComSig, pbHash, pbSalt);
}

BOOL XeCryptBnQwBeSigVerify(PXECRYPT_SIG pSig, const u8* pbHash, const u8* pbSalt, const PXECRYPT_RSA pRsa)
{
	if(bswap32(pRsa->cqw) == 0x20)
	{
		u32 exp = bswap32(pRsa->dwPubExp);
		if((exp == 0x00000003) || (exp == 0x00010001))
		{
			BYTE pbDecSig[0x100];
			BYTE pbComSig[0x100];
			XeCryptBnQwBeSigCompute(pSig, pbHash, pbSalt, pRsa, pbDecSig, pbComSig);
			if(memcmp(pbDecSig, pbComSig, 0x100) == 0)
				return TRUE;
		}
	}
	return FALSE;
}

BOOL XeCryptBnQwBeSigCreate(PXECRYPT_SIG pSig, const u8* pbHash, const u8* pbSalt, const XECRYPT_RSA *pRsa)
{
	BOOL ret = FALSE;
	u32 cnt = bswap32(pRsa->cqw);
	if(cnt == 0x20)
	{
		u32 exp = bswap32(pRsa->dwPubExp);
		if((exp == 0x00000003) || (exp == 0x00010001))
		{
			PXECRYPT_RSAPRV_4096 keyp = (PXECRYPT_RSAPRV_4096)pRsa;
			XeCryptBnQwBeSigFormat(pSig, pbHash, pbSalt);
			if(XeCryptBnQwNeCompare((u64*)pSig, (u64*)&keyp->aqwM[0], cnt) == -1)
			{
				u64 buf1[0x40];
				u64 buf2[0x20];
				u64 exp64 = (exp&0xFFFFFFFF);
				XeCryptBnQw_Zero(buf1, 0x40);
				XeCryptBnQw_Zero(buf2, 0x20);
				buf1[0] = bswap64(2ULL);
				buf2[0] = bswap64(((exp64-1) << 11));
				if(XeCryptBnQwNeModExp(buf2, buf1, buf2, keyp->aqwM, cnt))
				{
					XeCryptBnQwNeMul(buf1, buf2, (u64*)pSig, cnt);
					if(XeCryptBnQwNeMod(buf1, keyp->aqwM, (u64*)pSig, cnt*2, cnt))
					{
						ret = TRUE;
					}
				}
			}
		}
	}
	return ret;
}

// uses chinese remainder theorem to RSA encrypt
BOOL XeCryptBnQwNeModExpRoot(u64 *pqwOut, const u64 *pqwIn, const u64 *pqwPP, const u64 *pqwQQ, const u64 *pqwDP, const u64 *pqwDQ, const u64 *pqwCR, u32 cqw)
{
	BOOL ret = FALSE;
	if(cqw <= 0x20)
	{
		u64 buf1[0x20];
		u64 buf2[0x40];
		u64 buf3[0x20];
		if(XeCryptBnQwNeMod(pqwIn, pqwPP, buf3, cqw*2, cqw))
		{
			if(XeCryptBnQwNeModExp(buf1, buf3, pqwDP, pqwPP, cqw))
			{
				if(XeCryptBnQwNeMod(pqwIn, pqwQQ, buf3, cqw*2, cqw))
				{
					if(XeCryptBnQwNeModExp(buf2, buf3, pqwDQ, pqwQQ, cqw))
					{
						if(XeCryptBnQwNeSub(buf3, buf1, buf2, cqw))
						{
							while(XeCryptBnQwNeAdd(buf3, buf3, pqwPP, cqw) == 0);
						}
						XeCryptBnQwNeMul(pqwOut, buf3, pqwCR, cqw);
						if(XeCryptBnQwNeMod(pqwOut, pqwPP, buf3, cqw*2, cqw))
						{
							XeCryptBnQwNeMul(pqwOut, buf3, pqwQQ, cqw);
							XeCryptBnQw_Zero(buf2+cqw, cqw);
							XeCryptBnQwNeAdd(pqwOut, pqwOut, buf2, cqw*2);
							ret = TRUE;
						}
					}
				}
			}
		}
	}
	return ret;
}

BOOL XeCryptBnQwNeRsaPrvCrypt(const u64 *pqwIn, u64 *pqwOut, const XECRYPT_RSA *pRsa)
{
	u64 *pP, *pQ, *pDP, *pDQ, *pCR;
	u32 cnt = bswap32(pRsa->cqw);
	if(cnt == 0x40) // 4096 bit
	{
		PXECRYPT_RSAPRV_4096 keyp = (PXECRYPT_RSAPRV_4096)pRsa;
		pP = keyp->aqwP; pQ = keyp->aqwQ; pDP = keyp->aqwDP; pDQ = keyp->aqwDQ; pCR = keyp->aqwCR;
	}
	else if(cnt == 0x20) // 2048 bit
	{
		PXECRYPT_RSAPRV_2048 keyp = (PXECRYPT_RSAPRV_2048)pRsa;
		pP = keyp->aqwP; pQ = keyp->aqwQ; pDP = keyp->aqwDP; pDQ = keyp->aqwDQ; pCR = keyp->aqwCR;
	}
	else if(cnt == 0x18) // 1536 bit
	{
		PXECRYPT_RSAPRV_1536 keyp = (PXECRYPT_RSAPRV_1536)pRsa;
		pP = keyp->aqwP; pQ = keyp->aqwQ; pDP = keyp->aqwDP; pDQ = keyp->aqwDQ; pCR = keyp->aqwCR;
	}
	else if(cnt == 0x10) // 1024 bit
	{
		PXECRYPT_RSAPRV_1024 keyp = (PXECRYPT_RSAPRV_1024)pRsa;
		pP = keyp->aqwP; pQ = keyp->aqwQ; pDP = keyp->aqwDP; pDQ = keyp->aqwDQ; pCR = keyp->aqwCR;
	}
	else
		return FALSE;
	cnt = cnt>>1;
	return XeCryptBnQwNeModExpRoot(pqwOut, pqwIn, pP, pQ, pDP, pDQ, pCR, cnt);
}

BOOL XeCryptBnQwNeRsaPubCrypt(const u64 *pqwIn, u64 *pqwOut, const XECRYPT_RSA *pRsa)
{
	BOOL ret = FALSE;
	u32 cnt = bswap32(pRsa->cqw);
	if((cnt != 0) && (cnt <= 0x40))
	{
		PXECRYPT_RSAPRV_4096 keyp = (PXECRYPT_RSAPRV_4096)pRsa;
		u64 buf[0x40];
		u64 exp = (bswap32(pRsa->dwPubExp)&0xFFFFFFFF);
		XeCryptBnQw_Zero(buf, cnt);
		buf[0] = bswap64(exp);
		return XeCryptBnQwNeModExp(pqwOut, pqwIn, buf, keyp->aqwM, cnt);
	}
	return ret;
}

BOOL XeCryptBnDwLePkcs1Verify(const unsigned char* pbHash, const unsigned char* pbSig, DWORD cbSig)
{
	BOOL ret = FALSE;
	if((cbSig >= 0x27) && (cbSig <= 0x200))
	{
		u8 buf[0x200];
		int typ = 2;
		if(pbSig[0x16] == 0)
			typ = 0;
		else if(pbSig[0x16] == 0x1A) // else typ = 2
			typ = 1;
		XeCryptBnDwLePkcs1Format(pbHash, typ, buf, cbSig);
		if(memcmp(buf, pbSig, cbSig) == 0)
			ret = TRUE;
	}
	return ret;
}

void XeCryptShowBuffer(unsigned char *buffer, int size)
{
	int i;
	for (i = 0; i < size; i++)
	{
		if (!(i % 0x10))
			printf("\n  ");
		printf(" %02X", buffer[i]);
	}
	printf("\n");
}


BOOL XeKeysPkcs1Verify(const unsigned char* pbHash, const unsigned char* pbSig, const XECRYPT_RSA *pRsaPub)
{
	BOOL ret = FALSE;
	DWORD cnt = bswap32(pRsaPub->cqw);
	if((cnt != 0) && (cnt <= 0x40))
	{
		u8 buf[0x200];

		XeCryptBnQw_SwapDwQwLeBe(pbSig, buf, cnt);
		XeCryptBnQwNeRsaPubCrypt((u64*)buf, (u64*)buf, pRsaPub);
		XeCryptBnQw_SwapDwQwLeBe(buf, buf, cnt);

		ret = XeCryptBnDwLePkcs1Verify(pbHash, buf, cnt<<3);
	}
	return ret;
}

BOOL XeKeysPkcs1Create(const unsigned char* pbHash, unsigned char* pbSig, const XECRYPT_RSA* pRsaPrv)
{
	BOOL ret = FALSE;
	DWORD cnt = bswap32(pRsaPrv->cqw);
	if ((cnt != 0) && (cnt <= 0x40))
	{
		u8 buf[0x200];
		int typ = 2;
		if (pbSig[0x16] == 0)
			typ = 0;
		else if (pbSig[0x16] == 0x1A) // else typ = 2
			typ = 1;
		XeCryptBnDwLePkcs1Format(pbHash, typ, buf, cnt << 3);
		XeCryptBnQw_SwapDwQwLeBe(buf, buf, cnt);
		XeCryptBnQwNeRsaPrvCrypt((u64*)buf, (u64*)buf, pRsaPrv);
		XeCryptBnQw_SwapDwQwLeBe(buf, pbSig, cnt);
		ret = TRUE;
	}
	return ret;
}

BOOL XeCryptBnQwNeRsaKeyGen(unsigned int cbits, unsigned int dwPubExp, XECRYPT_RSA * pRsaPub, XECRYPT_RSA * pRsaPrv)
{
	PKCS1_RSA_PRIVATE_KEY pkpriv;
	// cbits must be 1024, 1536, 2048 or 4096 bit
	if ((cbits != 0) && (cbits != 768) && (cbits != 1024) && (cbits != 2048))
		return FALSE;
	// exponent must be an odd number greater than 3
	if ((dwPubExp < 3) || ((dwPubExp & 1) == 0))
		return FALSE;
	if (PKCS1_RSA_GenKey(&pkpriv, cbits / 32, dwPubExp) != ERR_OK)
		return FALSE;
	//printf("modulus:");
	//display_digitt(pkpriv.modulus, pkpriv.len);
	//printf("PublicExponent:");
	//display_digitt(pkpriv.PublicExponent, pkpriv.len);
	//printf("d:");
	//display_digitt(pkpriv.exponent, pkpriv.len);
	//printf("p:");
	//display_digitt(pkpriv.p, pkpriv.plen);
	//printf("q:");
	//display_digitt(pkpriv.q, pkpriv.qlen);
	//printf("dP:");
	//display_digitt(pkpriv.dP, pkpriv.plen);
	//printf("dQ:");
	//display_digitt(pkpriv.dQ, pkpriv.qlen);
	//printf("qInv:");
	//display_digitt(pkpriv.qInv, pkpriv.qlen);

	if (cbits == 4096) // 4096 bit 0x40
	{
		PXECRYPT_RSAPRV_4096 priv = (PXECRYPT_RSAPRV_4096)pRsaPrv;
		PXECRYPT_RSAPUB_4096 pub = (PXECRYPT_RSAPUB_4096)pRsaPub;
		memset(priv, 0, sizeof(XECRYPT_RSAPRV_4096));
		memset(pub, 0, sizeof(XECRYPT_RSAPUB_4096));
		priv->Rsa.cqw = bswap32(0x40);
		mpGetBeQwBigNum(0x40, (u8*)priv->aqwM, pkpriv.modulus);
		mpGetBeQwBigNum(0x20, (u8*)priv->aqwP, pkpriv.p);
		mpGetBeQwBigNum(0x20, (u8*)priv->aqwQ, pkpriv.q);
		mpGetBeQwBigNum(0x20, (u8*)priv->aqwDP, pkpriv.dP);
		mpGetBeQwBigNum(0x20, (u8*)priv->aqwDQ, pkpriv.dQ);
		mpGetBeQwBigNum(0x20, (u8*)priv->aqwCR, pkpriv.qInv);
		mpGetBeQwBigNum(0x40, (u8*)pub->aqwM, pkpriv.modulus);
	}
	else if (cbits == 2048) // 2048 bit 0x20
	{
		PXECRYPT_RSAPRV_2048 priv = (PXECRYPT_RSAPRV_2048)pRsaPrv;
		PXECRYPT_RSAPUB_2048 pub = (PXECRYPT_RSAPUB_2048)pRsaPub;
		memset(priv, 0, sizeof(XECRYPT_RSAPRV_2048));
		memset(pub, 0, sizeof(XECRYPT_RSAPUB_2048));
		priv->Rsa.cqw = bswap32(0x20);
		mpGetBeQwBigNum(0x20, (u8*)priv->aqwM, pkpriv.modulus);
		mpGetBeQwBigNum(0x10, (u8*)priv->aqwP, pkpriv.p);
		mpGetBeQwBigNum(0x10, (u8*)priv->aqwQ, pkpriv.q);
		mpGetBeQwBigNum(0x10, (u8*)priv->aqwDP, pkpriv.dP);
		mpGetBeQwBigNum(0x10, (u8*)priv->aqwDQ, pkpriv.dQ);
		mpGetBeQwBigNum(0x10, (u8*)priv->aqwCR, pkpriv.qInv);
		mpGetBeQwBigNum(0x20, (u8*)pub->aqwM, pkpriv.modulus);
	}
	else if (cbits == 1536) // 1536 bit 0x18
	{
		PXECRYPT_RSAPRV_1536 priv = (PXECRYPT_RSAPRV_1536)pRsaPrv;
		PXECRYPT_RSAPUB_1536 pub = (PXECRYPT_RSAPUB_1536)pRsaPub;
		memset(priv, 0, sizeof(XECRYPT_RSAPRV_1536));
		memset(pub, 0, sizeof(XECRYPT_RSAPUB_1536));
		priv->Rsa.cqw = bswap32(0x18);
		mpGetBeQwBigNum(0x18, (u8*)priv->aqwM, pkpriv.modulus);
		mpGetBeQwBigNum(0xC, (u8*)priv->aqwP, pkpriv.p);
		mpGetBeQwBigNum(0xC, (u8*)priv->aqwQ, pkpriv.q);
		mpGetBeQwBigNum(0xC, (u8*)priv->aqwDP, pkpriv.dP);
		mpGetBeQwBigNum(0xC, (u8*)priv->aqwDQ, pkpriv.dQ);
		mpGetBeQwBigNum(0xC, (u8*)priv->aqwCR, pkpriv.qInv);
		mpGetBeQwBigNum(0x18, (u8*)pub->aqwM, pkpriv.modulus);
	}
	else if (cbits == 1024) // 1024 bit 0x10
	{
		PXECRYPT_RSAPRV_1024 priv = (PXECRYPT_RSAPRV_1024)pRsaPrv;
		PXECRYPT_RSAPUB_1024 pub = (PXECRYPT_RSAPUB_1024)pRsaPub;
		memset(priv, 0, sizeof(XECRYPT_RSAPRV_1024));
		memset(pub, 0, sizeof(XECRYPT_RSAPUB_1024));
		priv->Rsa.cqw = bswap32(0x10);
		mpGetBeQwBigNum(0x10, (u8*)priv->aqwM, pkpriv.modulus);
		mpGetBeQwBigNum(0x8, (u8*)priv->aqwP, pkpriv.p);
		mpGetBeQwBigNum(0x8, (u8*)priv->aqwQ, pkpriv.q);
		mpGetBeQwBigNum(0x8, (u8*)priv->aqwDP, pkpriv.dP);
		mpGetBeQwBigNum(0x8, (u8*)priv->aqwDQ, pkpriv.dQ);
		mpGetBeQwBigNum(0x8, (u8*)priv->aqwCR, pkpriv.qInv);
		mpGetBeQwBigNum(0x10, (u8*)pub->aqwM, pkpriv.modulus);
	}
	pRsaPrv->dwPubExp = bswap32(dwPubExp);
	pRsaPub->cqw = pRsaPrv->cqw;
	pRsaPub->dwPubExp = pRsaPrv->dwPubExp;
	PKCS1_RSA_FreeKey(&pkpriv);
	return TRUE;
}

//void  XeCryptDesParity(const unsigned char* pbInp, unsigned int cbInp, unsigned char* pbOut);

void XeCryptDesKey(XECRYPT_DES_STATE* pDesState, const unsigned char* pbKey)
{
	des_setup(pbKey, XECRYPT_DES_KEY_SIZE, 0, pDesState);
}

void XeCryptDesEcb(XECRYPT_DES_STATE* pDesState, const unsigned char* pbInp, unsigned char* pbOut, BOOL fEncrypt)
{
	if(fEncrypt)
		des_ecb_encrypt(pbInp, pbOut, pDesState);
	else
		des_ecb_decrypt(pbInp, pbOut, pDesState);
}

void XeCryptDesCbc(XECRYPT_DES_STATE* pDesState, const unsigned char* pbInp, unsigned int cbInp, unsigned char* pbOut, unsigned char* pbFeed, BOOL fEncrypt)
{
	unsigned int i, j;
	if(fEncrypt)
	{
		for(j = 0; j < cbInp; j += XECRYPT_DES_BLOCK_SIZE)
		{
			for(i = 0; i < XECRYPT_DES_BLOCK_SIZE; i++)
				pbOut[j+i] = (pbInp[j+i]^pbFeed[i])&0xFF;
			des_ecb_encrypt(&pbOut[j], &pbOut[j], pDesState);
			memcpy(pbFeed, &pbOut[j], XECRYPT_DES_BLOCK_SIZE);
		}
	}
	else
	{
		unsigned char tmp[XECRYPT_DES_BLOCK_SIZE];
		for(j = 0; j < cbInp; j += XECRYPT_DES_BLOCK_SIZE)
		{
			memcpy(tmp, &pbInp[j], XECRYPT_DES_BLOCK_SIZE);
			des_ecb_decrypt(&pbInp[j], &pbOut[j], pDesState);
			for(i = 0; i < XECRYPT_DES_BLOCK_SIZE; i++)
				pbOut[j+i] = (pbInp[j+i]^pbFeed[i])&0xFF;
			memcpy(pbFeed, tmp, XECRYPT_DES_BLOCK_SIZE);
		}
	}
}

void XeCryptDes3Key(XECRYPT_DES3_STATE* pDes3State, const unsigned char* pbKey)
{
	des3_setup(pbKey, XECRYPT_DES3_KEY_SIZE, 0, pDes3State);
}

void XeCryptDes3Ecb(XECRYPT_DES3_STATE* pDes3State, const unsigned char* pbInp, unsigned char * pbOut, BOOL fEncrypt)
{
	if(fEncrypt)
		des3_ecb_encrypt(pbInp, pbOut, pDes3State);
	else
		des3_ecb_decrypt(pbInp, pbOut, pDes3State);
}

void XeCryptDes3Cbc(XECRYPT_DES3_STATE* pDes3State, const unsigned char* pbInp, unsigned int cbInp, unsigned char* pbOut, unsigned char* pbFeed, BOOL fEncrypt)
{
	unsigned int i, j;
	if(fEncrypt)
	{
		for(j = 0; j < cbInp; j += XECRYPT_DES3_BLOCK_SIZE)
		{
			for(i = 0; i < XECRYPT_DES3_BLOCK_SIZE; i++)
				pbOut[j+i] = (pbInp[j+i]^pbFeed[i])&0xFF;
			des3_ecb_encrypt(&pbOut[j], &pbOut[j], pDes3State);
			memcpy(pbFeed, &pbOut[j], XECRYPT_DES3_BLOCK_SIZE);
		}
	}
	else
	{
		unsigned char tmp[XECRYPT_DES3_BLOCK_SIZE];
		for(j = 0; j < cbInp; j += XECRYPT_DES3_BLOCK_SIZE)
		{
			memcpy(tmp, &pbInp[j], XECRYPT_DES3_BLOCK_SIZE);
			des3_ecb_decrypt(&pbInp[j], &pbOut[j], pDes3State);
			for(i = 0; i < XECRYPT_DES3_BLOCK_SIZE; i++)
				pbOut[j+i] = (pbInp[j+i]^pbFeed[i])&0xFF;
			memcpy(pbFeed, tmp, XECRYPT_DES3_BLOCK_SIZE);
		}
	}
}

void XeCryptMd5Init(XECRYPT_MD5_STATE * pMd5State)
{
	MD5_Init(pMd5State);
}

void XeCryptMd5Update(XECRYPT_MD5_STATE * pMd5State, const unsigned char* pbInp, unsigned int cbInp)
{
	MD5_Update(pMd5State, pbInp, cbInp);
}

void XeCryptMd5Final(XECRYPT_MD5_STATE * pMd5State, unsigned char* pbOut, unsigned int cbOut)
{
	unsigned char tbuf[XECRYPT_MD5_DIGEST_SIZE];
	MD5_Final(tbuf, pMd5State);
	if(cbOut >= XECRYPT_MD5_DIGEST_SIZE)
		memcpy(pbOut, tbuf, XECRYPT_MD5_DIGEST_SIZE);
	else
		memcpy(pbOut, tbuf, cbOut);
}

void XeCryptMd5(const unsigned char* pbInp1, unsigned int cbInp1, const unsigned char* pbInp2, unsigned int cbInp2, const unsigned char* pbInp3, unsigned int cbInp3, unsigned char* pbOut, unsigned int cbOut)
{
	XECRYPT_MD5_STATE ctx;
	MD5_Init(&ctx);
	if((cbInp1 != 0)&&(pbInp1 != NULL))
		MD5_Update(&ctx, pbInp1, cbInp1);
	if((cbInp2 != 0)&&(pbInp2 != NULL))
		MD5_Update(&ctx, pbInp2, cbInp2);
	if((cbInp3 != 0)&&(pbInp3 != NULL))
		MD5_Update(&ctx, pbInp3, cbInp3);
	XeCryptMd5Final(&ctx, pbOut, cbOut);
}


/*  //devkit CD signing...
 BYTE sig[0x20*8];
 XECRYPT_SIG* psig = (XECRYPT_SIG*)sig;
 u64 fini[0x20];
 if(XeCryptBnQwBeSigCreate(psig, someHash, (BYTE*)salt, pkey))
 {
	 dprintf("XeCryptBnQwBeSigCreate all good\n");
	 display_buffer_hex((BYTE*)sig, 0x20*8);
	 prvCrypt((u64*)sig);
	 if(XeCryptBnQwNeRsaPrvCrypt((u64*)&sig, fini, pkey))
	 {
		 dprintf("XeCryptBnQwNeRsaPrvCrypt all good\n");
		 if(XeCryptBnQwBeSigVerify((XECRYPT_SIG*)fini, someHash, (BYTE*)salt, pkey))
		 {
			 dprintf("XeCryptBnQwBeSigVerify all good\n");
			 display_buffer_hex((BYTE*)fini, 0x20*8);
		 }
		 else
			 dprintf("XeCryptBnQwBeSigVerify failed!\n");
	 }
	 else
		 dprintf("XeCryptBnQwNeRsaPrvCrypt failed!\n");
 }
 else
	 dprintf("XeCryptBnQwBeSigCreate failed!\n");
*/

/*
void rotSumTest()
{
unsigned char rotsum[0x14];
unsigned char sigbuf[0x100];
unsigned char tdata[] = {
0x48, 0x58, 0x50, 0x4D, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x23, 0xE5, 0x01, 0x2E,
0xA8, 0x2C, 0x75, 0x5C, 0xCD, 0x47, 0x4C, 0x10, 0x11, 0x38, 0xD9, 0x38, 0xEA, 0x8D, 0x32, 0x7B,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
display_buffer_hex(tdata, 0x30, VERB_LV1);
XeCryptRotSumSha(tdata, 0x30, NULL, 0, rotsum, 0x14);
display_buffer_hex(rotsum, 0x14, VERB_LV1);
XeCryptBnDwLePkcs1Format(rotsum, 0, sigbuf, 0x100);
display_buffer_hex(sigbuf, 0x100, VERB_LV1);
XeCryptBnQw_SwapDwQwLeBe(sigbuf, sigbuf, 0x100/8);
display_buffer_hex(sigbuf, 0x100, VERB_LV1);
}
*/
