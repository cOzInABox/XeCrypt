/* pkcs1-rsa.h */

#ifndef _PKCS1RSA_H_
#define _PKCS1RSA_H_

#include "bigdigits.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PKCS1_MAX_LINE_LEN	346	/* for reading parameter file */

#define PKCS1_VERSION_MAJOR 2
#define PKCS1_VERSION_MINOR 1

/* RSA key lengths. */
#define MIN_RSA_MODULUS_LEN 	5		/* 160 bits */
#define MAX_RSA_MODULUS_LEN 	MAX_DIG_LEN	/* 2048 bits */
#define MAX_MSG_LEN		256		/* bytes */

/* Error codes - todo: move into mcrypto.h and add MCRYPTO prefix */
#define ERR_DEFAULT		0x00
#define ERR_OK			0x01
#define ERR_MOD_TOO_SMALL	0x02
#define ERR_MOD_TOO_LONG	0x03
#define ERR_PRIME_FAILED	0x04
#define ERR_MSG_TOO_LONG        0x05
#define ERR_LABEL_TOO_LONG      0x06
#define ERR_DECRYPTION		0x07
#define ERR_UNKNOWN_HASH	0x08
#define ERR_VALID_SIGNATURE     0x09
#define ERR_INVALID_SIGNATURE   0x0a
#define ERR_PSS_CONSISTENT	0x0b
#define ERR_PSS_INCONSISTENT	0x0c
#define ERR_PSS_ENCODING	0x0d
#define ERR_HASH		0x0e


/* PKCS1-RSA public and private key. */
typedef struct {
  UINT    len;			/* length in digits of modulus */
  DIGIT_T *modulus;  		/* modulus */
  DIGIT_T *exponent; 		/* public exponent */
} PKCS1_RSA_PUBLIC_KEY;

typedef struct {
  UINT len;       		/* length in digits of modulus */
  DIGIT_T *modulus;		/* modulus */
  DIGIT_T *PublicExponent; 	/* public exponent */
  DIGIT_T *exponent; 		/* private exponent aka d */
  
  /* for fast decryption using CRT */
  DIGIT_T *p;   		/* prime factor 1 */
  UINT    plen;			/* length in digits of p */
  DIGIT_T *q;   		/* prime factor 2 */
  UINT    qlen;			/* length in digits of q = len - plen */
  DIGIT_T *dP;     		/* e*dP = 1 mod p-1 */
  DIGIT_T *dQ;     		/* e*dQ = 1 mod q-1 */
  DIGIT_T *qInv;          	/* q*qInv = 1 mod p */
} PKCS1_RSA_PRIVATE_KEY;


/* function prototype */
void errmsg(int err);
	/* Print out error message */

void PKCS1_RSA_FreeKey(PKCS1_RSA_PRIVATE_KEY *ssk);
	/* frees elements of a key once done using it */

int PKCS1_RSA_GenKey(PKCS1_RSA_PRIVATE_KEY *ssk, UINT mod_len, UINT exponent);
	/* PKCS #1 Key Generation according to modulus bit length, len*BITS_PER_DIGIT */

int PKCS1_RSAEP(PKCS1_RSA_PUBLIC_KEY *spk, DIGIT_T *m, DIGIT_T *c);
	/* PKCS #1 Encryption Primitive */

int PKCS1_RSADP(PKCS1_RSA_PRIVATE_KEY *ssk, DIGIT_T *c, DIGIT_T *m);
	/* PKCS #1 Decryption Primitive */

int PKCS1_RSASP1(PKCS1_RSA_PRIVATE_KEY *ssk, DIGIT_T *m, DIGIT_T *s);
	/* PKCS #1 Signature Generation Primitive */

int PKCS1_RSAVP1(PKCS1_RSA_PUBLIC_KEY *spk, DIGIT_T *s, DIGIT_T *m);
	/* PKCS #1 Signature Verification Primitive */

int PKCS1_EME_OAEP_ENC(PKCS1_RSA_PUBLIC_KEY *spk, UINT hid, BYTE *m, UINT mlen, BYTE *L, UINT llen, BYTE *em);
	/* PKCS #1 OAEP encoding function */

int PKCS1_RSA_OAEP_ENCRYPT(PKCS1_RSA_PUBLIC_KEY *spk, UINT hid, BYTE *m, UINT mlen, BYTE *L, UINT llen, BYTE *c); 
	/* PKCS #1 OAEP Encryption */

int PKCS1_EME_OAEP_DEC(PKCS1_RSA_PRIVATE_KEY *ssk, UINT hid, BYTE *em, BYTE *L, UINT llen, BYTE *m, UINT *mlen);
	/* PKCS #1 OAEP decoding function */

int PKCS1_RSA_OAEP_DECRYPT(PKCS1_RSA_PRIVATE_KEY *ssk, UINT hid, BYTE *c, BYTE *L, UINT llen, BYTE *m, UINT *mlen); 
	/* PKCS #1 OAEP Decryption */

int PKCS1_RSASSA_PSS_SIGN(PKCS1_RSA_PRIVATE_KEY *ssk, UINT hid, BYTE *m, UINT mlen, UINT slen, BYTE *s);
	/* PKCS #1 Signature Generation Using PSS encoding method */

int PKCS1_RSASSA_PSS_VERIFY(PKCS1_RSA_PUBLIC_KEY *spk, UINT hid, BYTE *m, UINT mlen, UINT slen, BYTE *s);
	/* PKCS #1 Signature Verification */

int PKCS1_EMSA_PSS_ENCODE(UINT hid, BYTE *m, UINT mlen, UINT slen, BYTE *em, UINT emlen);
	/* PKCS #1 PSS Encoding */

int PKCS1_EMSA_PSS_VERIFY(UINT hid, BYTE *m, UINT mlen, UINT slen, BYTE *em, UINT emlen);	
	/* PKCS #1 PSS Verification */

/* Untility Functions */
int LoadPublicKey(char *fname, PKCS1_RSA_PUBLIC_KEY *spk);
	/* Public Key Loader */

int LoadPrivateKey(char *fname, PKCS1_RSA_PRIVATE_KEY *ssk);
	/* Secret Key Loader */
			
#ifdef __cplusplus
}
#endif

#endif	/* _PKCS1RSA_H_ */
