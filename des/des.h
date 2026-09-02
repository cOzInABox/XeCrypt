#ifndef _DES_H
#define _DES_H

// #define LTC_TEST	1

enum {
	CRYPT_OK=0,             /* Result OK */
	CRYPT_ERROR,            /* Generic Error */
	CRYPT_NOP,              /* Not a failure but no operation was performed */
	CRYPT_INVALID_KEYSIZE,  /* Invalid key size given */
	CRYPT_INVALID_ROUNDS,   /* Invalid number of rounds */
	CRYPT_FAIL_TESTVECTOR,  /* Algorithm failed test vectors */
};

typedef struct des_key {
	unsigned int ek[32], dk[32];
} XECRYPT_DES_STATE;

typedef struct des3_key {
	unsigned int ek[3][32], dk[3][32];
} XECRYPT_DES3_STATE;

#ifdef __cplusplus
extern "C" {
#endif

int des_setup(const unsigned char *key, int keylen, int num_rounds, XECRYPT_DES_STATE *skey);
int des_ecb_encrypt(const unsigned char *pIn, unsigned char *pOut, XECRYPT_DES_STATE *skey);
int des_ecb_decrypt(const unsigned char *pIn, unsigned char *pOut, XECRYPT_DES_STATE *skey);
int des_test(void);
void des_done(XECRYPT_DES_STATE *skey);
int des_keysize(int *keysize);

int des3_setup(const unsigned char *key, int keylen, int num_rounds, XECRYPT_DES3_STATE *skey);
int des3_ecb_encrypt(const unsigned char *pIn, unsigned char *pOut, XECRYPT_DES3_STATE *skey);
int des3_ecb_decrypt(const unsigned char *pIn, unsigned char *pOut, XECRYPT_DES3_STATE *skey);
int des3_test(void);
void des3_done(XECRYPT_DES3_STATE *skey);
int des3_keysize(int *keysize);


#ifdef __cplusplus
}
#endif

#endif // _DES_H
