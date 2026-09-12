#ifndef _XECRYPT_H
#define _XECRYPT_H

#include "rc4/rc4.h"
#include "sha/sha.h"
#include "aes/aes.h"
#include "des/des.h"
#include "md5/md5.h"
#include "xecryptTypes.h"

// some important constants
#define XECRYPT_SHA_DIGEST_SIZE			(20)
#define XECRYPT_HMAC_SHA_MAX_KEY_SZ		(64)

#define XECRYPT_DES_BLOCK_SIZE			(8)
#define XECRYPT_DES_KEY_SIZE			(8)

#define XECRYPT_DES3_BLOCK_SIZE			(8)
#define XECRYPT_DES3_KEY_SIZE			(24)

#define XECRYPT_MD5_DIGEST_SIZE			(16)

#define XECRYPT_AES_BLOCK_SIZE			(16)
#define XECRYPT_AES_KEY_SIZE			(16)
#define XECRYPT_AES_FEED_SIZE			(16)

// these states are in other files, but commented here for easier access
// XECRYPT_DES_STATE, XECRYPT_DES3_STATE
// XECRYPT_MD5_STATE

typedef RC4_KEY XECRYPT_RC4_STATE, *PXECRYPT_RC4_STATE;
typedef SHA_CTX XECRYPT_SHA_STATE, *PXECRYPT_SHA_STATE;
typedef struct{
	aes_encrypt_ctx encCtx;
	aes_decrypt_ctx decCtx;
} XECRYPT_AES_STATE, *PXECRYPT_AES_STATE;

typedef struct {
	u32           cqw;                // Number of u64 digits in modulus
	u32           dwPubExp;           // Public exponent
	u64           qwReserved;         // Reserved (was qwMI)
} XECRYPT_RSA, *PXECRYPT_RSA;

typedef struct {
	XECRYPT_RSA     Rsa;                // Common header
	u64           aqwM[16];           // [BnQwNe] Modulus
} XECRYPT_RSAPUB_1024, *PXECRYPT_RSAPUB_1024;

typedef struct {
	XECRYPT_RSA     Rsa;                // Common header
	u64           aqwM[24];           // [BnQwNe] Modulus
} XECRYPT_RSAPUB_1536, *PXECRYPT_RSAPUB_1536;

typedef struct {
	XECRYPT_RSA     Rsa;                // Common header
	u64           aqwM[32];           // [BnQwNe] Modulus
} XECRYPT_RSAPUB_2048, *PXECRYPT_RSAPUB_2048;

typedef struct {
	XECRYPT_RSA     Rsa;                // Common header
	u64           aqwM[64];           // [BnQwNe] Modulus
} XECRYPT_RSAPUB_4096, *PXECRYPT_RSAPUB_4096;

typedef struct {
	XECRYPT_RSA     Rsa;                // Common header
	u64           aqwM[16];           // [BnQwNe] Modulus
	u64           aqwP[8];            // [BnQwNe] Private prime P
	u64           aqwQ[8];            // [BnQwNe] Private prime Q
	u64           aqwDP[8];           // [BnQwNe] Private exponent P
	u64           aqwDQ[8];           // [BnQwNe] Private exponent Q
	u64           aqwCR[8];           // [BnQwNe] Private coefficient
} XECRYPT_RSAPRV_1024, *PXECRYPT_RSAPRV_1024;

typedef struct {
	XECRYPT_RSA     Rsa;                // Common header
	u64           aqwM[24];           // [BnQwNe] Modulus
	u64           aqwP[12];           // [BnQwNe] Private prime P
	u64           aqwQ[12];           // [BnQwNe] Private prime Q
	u64           aqwDP[12];          // [BnQwNe] Private exponent P
	u64           aqwDQ[12];          // [BnQwNe] Private exponent Q
	u64           aqwCR[12];          // [BnQwNe] Private coefficient
} XECRYPT_RSAPRV_1536, *PXECRYPT_RSAPRV_1536;

typedef struct {
	XECRYPT_RSA     Rsa;                // Common header
	u64           aqwM[32];           // [BnQwNe] Modulus
	u64           aqwP[16];           // [BnQwNe] Private prime P
	u64           aqwQ[16];           // [BnQwNe] Private prime Q
	u64           aqwDP[16];          // [BnQwNe] Private exponent P
	u64           aqwDQ[16];          // [BnQwNe] Private exponent Q
	u64           aqwCR[16];          // [BnQwNe] Private coefficient
} XECRYPT_RSAPRV_2048, *PXECRYPT_RSAPRV_2048;

typedef struct {
	XECRYPT_RSA     Rsa;                // Common header
	u64           aqwM[64];           // [BnQwNe] Modulus
	u64           aqwP[32];           // [BnQwNe] Private prime P
	u64           aqwQ[32];           // [BnQwNe] Private prime Q
	u64           aqwDP[32];          // [BnQwNe] Private exponent P
	u64           aqwDQ[32];          // [BnQwNe] Private exponent Q
	u64           aqwCR[32];          // [BnQwNe] Private coefficient
} XECRYPT_RSAPRV_4096, *PXECRYPT_RSAPRV_4096;

typedef struct _XECRYPT_SIG { 
	u64 aqwPad[28]; // 0x0 sz:0xE0
	u8 bOne; // 0xE0 sz:0x1
	u8 abSalt[10]; // 0xE1 sz:0xA
	u8 abHash[20]; // 0xEB sz:0x14
	u8 bEnd; // 0xFF sz:0x1
} XECRYPT_SIG, *PXECRYPT_SIG; // size 256
//C_ASSERT(sizeof(XECRYPT_SIG) == 0x100);


#define XECRYPT_ROTSUM_DIGEST_SIZE	(32)

#ifdef __cplusplus
extern "C" {
#endif

void XeCryptUidEccEncode(u8* pbaCpuKey);
int XeCryptHammingWeight(u8* data, int len);

/* Streaming HMAC-SHA1, for callers hashing more inputs than the three
   XeCryptHmacSha() accepts. Computes the same value. */
typedef struct _XECRYPT_HMAC_SHA_STATE {
	XECRYPT_SHA_STATE Inner;
	XECRYPT_SHA_STATE Outer;
} XECRYPT_HMAC_SHA_STATE, *PXECRYPT_HMAC_SHA_STATE;

void XeCryptHmacShaInit(PXECRYPT_HMAC_SHA_STATE pHmacState, const unsigned char *pbKey, unsigned int cbKey);
void XeCryptHmacShaUpdate(PXECRYPT_HMAC_SHA_STATE pHmacState, const unsigned char *pbInp, unsigned int cbInp);
void XeCryptHmacShaFinal(PXECRYPT_HMAC_SHA_STATE pHmacState, unsigned char *pbOut, unsigned int cbOut);

void XeCryptHmacSha(const unsigned char *pbKey, unsigned int cbKey, const unsigned char *pbInp1, unsigned int cbInp1, const unsigned char *pbInp2, unsigned int cbInp2, const unsigned char *pbInp3, unsigned int cbInp3, unsigned char *pbOut, unsigned int cbOut);
void XeCryptRc4(unsigned char *pbKey, unsigned int cbKey, unsigned char *pbInpOut, unsigned int cbInpOut);
void XeCryptRc4Key(PXECRYPT_RC4_STATE pRc4State, unsigned char *pbKey, unsigned int cbKey);
void XeCryptRc4Ecb(PXECRYPT_RC4_STATE pRc4State, unsigned char *pbInpOut, unsigned int cbInpOut);
void XeCryptRandom(unsigned char *pb, unsigned int cb);
void XeCryptAesKey(PXECRYPT_AES_STATE pAesState, const unsigned char * pbKey);
void XeCryptAesEcb(PXECRYPT_AES_STATE pAesState, unsigned char * pbInp, unsigned char * pbOut, BOOL fEncrypt);
void XeCryptAesCbc(PXECRYPT_AES_STATE pAesState, unsigned char * pbInp, unsigned int cbInp, unsigned char * pbOut, unsigned char * pbFeed, BOOL fEncrypt);
void XeCryptShaInit(PXECRYPT_SHA_STATE pShaState);
void XeCryptShaUpdate(PXECRYPT_SHA_STATE pShaState, unsigned char* pbInp, unsigned int cbInp);
void XeCryptShaFinal(PXECRYPT_SHA_STATE pShaState, unsigned char* pbOut, unsigned int cbOut);
void XeCryptSha(unsigned char * pbInp1, unsigned int cbInp1, unsigned char * pbInp2, unsigned int cbInp2, unsigned char * pbInp3, unsigned int cbInp3, unsigned char* pbOut, unsigned int cbOut);
void XeCryptRotSum(unsigned char *pbOut, unsigned char *pbInp, unsigned int cqwInp);
void XeCryptRotSumSha(unsigned char * pbInp1, unsigned int cbInp1, unsigned char * pbInp2, unsigned int cbInp2, unsigned char * pbOut, unsigned int cbOut);
void XeCryptBnDwLePkcs1Format(const unsigned char *pbHash, unsigned int dwType, unsigned char *pbSig, unsigned int cbSig);

// this does not meet secure critera for a RNG, but will make up to 2048 bit keys (the largest size supported by the underlying bigint lib)
BOOL XeCryptBnQwNeRsaKeyGen(unsigned int cbits, unsigned int dwPubExp, XECRYPT_RSA * pRsaPub, XECRYPT_RSA * pRsaPrv);

void XeCryptBnQw_SwapDwQwLeBe(const unsigned char * pqwInp, unsigned char * pqwOut, unsigned int cqw);

void XeCryptBnQwBeSigFormat(PXECRYPT_SIG pSig, const u8* pbHash, const u8* pbSalt);
BOOL XeCryptBnQwBeSigCreate(PXECRYPT_SIG pSig, const u8* pbHash, const u8* pbSalt, const XECRYPT_RSA *pRsa);
BOOL XeCryptBnQwBeSigVerify(PXECRYPT_SIG pSig, const u8* pbHash, const u8* pbSalt, const PXECRYPT_RSA pRsa);
BOOL XeCryptBnQwNeModExpRoot(u64 *pqwOut, const u64 *pqwIn, const u64 *pqwPP, const u64 *pqwQQ, const u64 *pqwDP, const u64 *pqwDQ, const u64 *pqwCR, u32 cqw);
BOOL XeCryptBnQwNeRsaPrvCrypt(const u64 *pqwIn, u64 *pqwOut, const XECRYPT_RSA *pRsa);
BOOL XeCryptBnQwNeRsaPubCrypt(const u64 *pqwIn, u64 *pqwOut, const XECRYPT_RSA *pRsa);
BOOL XeKeysPkcs1Verify(const unsigned char * pbHash, const unsigned char * pbSig, const XECRYPT_RSA *pRsaPub);

// this doesn't actually exist on xbox, but shows how an PKCS1 signature can be created on xbox/this library
BOOL XeKeysPkcs1Create(const unsigned char* pbHash, unsigned char* pbSig, const XECRYPT_RSA* pRsaPrv);

void XeCryptDesKey(XECRYPT_DES_STATE * pDesState, const unsigned char* pbKey);
void XeCryptDesEcb(XECRYPT_DES_STATE * pDesState, const unsigned char* pbInp, unsigned char* pbOut, BOOL fEncrypt);
// WARNING: Cbc and DES3 stuff is not tested yet!
void XeCryptDesCbc(XECRYPT_DES_STATE * pDesState, const unsigned char* pbInp, unsigned int cbInp, unsigned char* pbOut, unsigned char* pbFeed, BOOL fEncrypt);
void XeCryptDes3Key(XECRYPT_DES3_STATE* pDes3State, const unsigned char* pbKey);
void XeCryptDes3Ecb(XECRYPT_DES3_STATE* pDes3State, const unsigned char* pbInp, unsigned char * pbOut, BOOL fEncrypt);
void XeCryptDes3Cbc(XECRYPT_DES3_STATE* pDes3State, const unsigned char* pbInp, unsigned int cbInp, unsigned char* pbOut, unsigned char* pbFeed, BOOL fEncrypt);

void XeCryptMd5Init(XECRYPT_MD5_STATE * pMd5State);
void XeCryptMd5Update(XECRYPT_MD5_STATE * pMd5State, const unsigned char* pbInp, unsigned int cbInp);
void XeCryptMd5Final(XECRYPT_MD5_STATE * pMd5State, unsigned char* pbOut, unsigned int cbOut);
void XeCryptMd5(const unsigned char* pbInp1, unsigned int cbInp1, const unsigned char* pbInp2, unsigned int cbInp2, const unsigned char* pbInp3, unsigned int cbInp3, unsigned char* pbOut, unsigned int cbOut);

#ifdef __cplusplus
}
#endif

#endif // _XECRYPT_H
