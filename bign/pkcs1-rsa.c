/*--------------------------------------------------------*/
/* PKCS #1 - RSA Cryptosystem Simplified Implementation   */
/* Author : Dang Nguyen Duc, nguyenduc@icu.ac.kr          */
/* Date   : 2006/11/12                                    */
/* Note   : Bit length of modulus of ways divisible by bit*/
/*          length of a double word (i.e. 32 bits)        */
/* To do  :						  */
/*          1. Fast Decryption Using CRT                  */			  
/*--------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "mcrypto.h"
#include "pkcs1-rsa.h"
#include "hash.h"
#include "bigdigits.h"

/* Internal Functions - Forward Declaration */
static void memxor(BYTE *c, BYTE *a, BYTE *b, UINT len); 
	/* Perform c = a XOR b */

static int GenRsaPrime(DIGIT_T p[], DIGIT_T e, UINT ndigits);
	/* Generate a pseudoprime of length ndigits */ /* To do: put in BigInt lib */	

static int MGF1(UINT hid, BYTE *seed, UINT seedlen, BYTE  *mask, UINT masklen);
	/* Mask Generation Function Using Hash Function */

/* Internal Functions */
static void memxor(BYTE *c, BYTE *a, BYTE *b, UINT len)
{
	while(len--)
		c[len] = a[len] ^ b[len]; 
}

static int GenRsaPrime(DIGIT_T p[], DIGIT_T e, UINT ndigits)
{
	DIGIT_T overflow;
	int i, j;
	int done = 0;
	int maxloops = 5;
	int maxadd = (ndigits*(sizeof(DIGIT_T) * 8))*100;

	for (i = 0; (done == 0) && (i < maxloops); i++)
	{
		printf("making rand of 0x%x digits\n", ndigits);
		/* Generate a pseudorandom number */
		mpMakeRandom(p, ndigits);

		/*	Make sure two highest and the low bits are set.
			Having the two highest bits set means the product
			(pq) will always have its highest bit set. 
		*/
		p[ndigits - 1] |= HIBITMASK | (HIBITMASK >> 1);
		p[0] |= 0x1;
		done = 0;
		overflow = 0;
		for (j = 0; j < maxadd; j++, overflow = mpShortAdd(p, p, 2, ndigits))
		{
			if (overflow)
				break;
			printf(".");
			if (mpIsPrimeShort(p, ndigits))
			{
				if (mpShortMod(p, e, ndigits) == 1)
				{
					printf("x");
					continue;
				}
				printf("*");
				if (mpIsPrimeRabin(p, ndigits, N_TEST_PRIME))
				{
					// success
					done = 1;
					break;
				}
			}
		}
	}
	
	return 0;
}


/* Main Functions */

void PKCS1_RSA_FreeKey(PKCS1_RSA_PRIVATE_KEY *ssk)
{
	if (ssk == NULL)
		return;
	if (ssk->modulus != NULL)
	{
		free(ssk->modulus);
		ssk->modulus = NULL;
	}
	if (ssk->PublicExponent != NULL)
	{
		free(ssk->PublicExponent);
		ssk->PublicExponent = NULL;
	}
	if (ssk->exponent != NULL)
	{
		free(ssk->exponent);
		ssk->exponent = NULL;
	}
	if (ssk->p != NULL)
	{
		free(ssk->p);
		ssk->p = NULL;
	}
	if (ssk->q != NULL)
	{
		free(ssk->q);
		ssk->q = NULL;
	}
	if (ssk->dP != NULL)
	{
		free(ssk->dP);
		ssk->dP = NULL;
	}
	if (ssk->dQ != NULL)
	{
		free(ssk->dQ);
		ssk->dQ = NULL;
	}
	if (ssk->qInv != NULL)
	{
		free(ssk->qInv);
		ssk->qInv = NULL;
	}
}

int PKCS1_RSA_GenKey(PKCS1_RSA_PRIVATE_KEY *ssk, UINT mod_len, UINT exponent)
{
	DIGIT_T *p, *q, *n, *e, *d, *dP, *dQ, *qInv;
	DIGIT_T Phi[MAX_DIG_LEN];
	DIGIT_T p1[MAX_DIG_LEN];
	DIGIT_T q1[MAX_DIG_LEN];
	UINT plen;
	UINT qlen;
	UINT prime_len;
	int ret;
		
	memset(ssk, 0, sizeof(PKCS1_RSA_PRIVATE_KEY));
	/* Limit checking */
	if(mod_len < MIN_RSA_MODULUS_LEN)
		return ERR_MOD_TOO_SMALL;
	if(mod_len > MAX_RSA_MODULUS_LEN)
		return ERR_MOD_TOO_LONG;
		
	/* Computing length of two primes */
	prime_len = plen = mod_len / 2;
	qlen = mod_len - plen;
	if(qlen > prime_len)
		prime_len = qlen;
		
	/* allocate memory */
	n = (DIGIT_T*)malloc(NBYTE(mod_len));
	mpSetZero(n, mod_len);
	
	e = (DIGIT_T*)malloc(NBYTE(mod_len));
	mpSetZero(e, mod_len);
	if (exponent != 0)
	{
		DIGIT_T val = exponent & 0xFFFFFFFF;
		mpSetDigit(e, val, mod_len);
	}
	
	d = (DIGIT_T*)malloc(NBYTE(mod_len));
	mpSetZero(d, mod_len);
	
	p = (DIGIT_T*)malloc(NBYTE(plen));
	mpSetZero(p, plen);
	
	q = (DIGIT_T*)malloc(NBYTE(qlen));
	mpSetZero(q, qlen);

	dP = (DIGIT_T*)malloc(NBYTE(prime_len));
	mpSetZero(dP, prime_len);
	
	dQ = (DIGIT_T*)malloc(NBYTE(prime_len));
	mpSetZero(dQ, prime_len);

	qInv = (DIGIT_T*)malloc(NBYTE(prime_len));
	mpSetZero(qInv, prime_len);

	/* Generate p and q */
	ret = GenRsaPrime(p, exponent, plen);
	if (ret==-1) 
		return ERR_PRIME_FAILED;
	printf("\nfirst prime found\n");
	
	ret = GenRsaPrime(q, exponent, qlen);
	if (ret==-1) 
		return ERR_PRIME_FAILED;
	printf("\nsecond prime found\n");

	// if q > p swap them
	if (mpCompare(p, q, plen) < 1)
	{
		DIGIT_T* temp;
		UINT tlen;
		// put p in temp
		temp = p;
		tlen = plen;
		// put q in p
		p = q;
		plen = qlen;
		// put temp in q
		q = temp;
		qlen = tlen;
	}

	mcrypto_dump("Key Gen: prime p",(BYTE *)p, NBYTE(plen));
	mcrypto_dump("Key Gen: prime q",(BYTE *)q, NBYTE(qlen));
	
	/* Comput p-1 and q-1 */
	mpShortSub(p1, p, 1, plen);
	mpShortSub(q1, q, 1, qlen);

	/* Check gcd(p-1, e) = 1 */
	/* Check gcd(q-1, e) = 1 */
	// skipping for now

	/* Compute n = pq */
	mpMultiply(Phi, p, q, prime_len);
	mpSetEqual(n, Phi, mod_len);
	mcrypto_dump("Key Gen Modulus",(BYTE *)n, NBYTE(mod_len));
	
	/* Phi(n) = (p-1)*(q-1) */
	mpMultiply(Phi, p1, q1, prime_len);
	mcrypto_dump("Key Gen Phi(n)=(p-1)(q-1)",(BYTE *)Phi, NBYTE(2*prime_len));
	
	/* Calculate private key, d = e^-1 Mod L */
	mpModInv(d, e, Phi, mod_len);
	
	mcrypto_dump("Key Gen Public Exponent e",(BYTE *)e, NBYTE(mod_len));
	mcrypto_dump("Key Gen Private Exponent d",(BYTE *)d, NBYTE(mod_len));
	
	/* Calculate CRT values */
	mpModInv(dP, e, p1, plen);
	mpModInv(dQ, e, q1, qlen);
	mpModInv(qInv, q, p, prime_len);

	mcrypto_dump("CRT value dP", (BYTE *)dP, NBYTE(plen));
	mcrypto_dump("CRT value dQ", (BYTE *)dQ, NBYTE(plen));
	mcrypto_dump("CRT value qInv", (BYTE *)qInv, NBYTE(prime_len));

	/* Collecting data */
	ssk->len = mod_len;
	ssk->modulus = n;
	ssk->PublicExponent = e;
	ssk->exponent = d;
	ssk->p = p;
	ssk->plen = plen;
	ssk->q = q;
	ssk->qlen = qlen;
	ssk->dP = dP;
	ssk->dQ = dQ;
	ssk->qInv = qInv;
	
	return ERR_OK;
}

#define MAX_SIZE_SET 0x8000
static int MGF1(UINT hid, BYTE *seed, UINT seedlen, BYTE  *mask, UINT masklen)
{
	/* Mask Generation Function Using Hash Function */
	UINT hlen;
	DWORD i;
	BYTE *hash;
	BYTE *data;
	DWORD n;
	DWORD MAX_SIZE = MAX_SIZE_SET;
	BYTE T[MAX_SIZE_SET];
	int ret;

	/* Init Output */
	memset(T, 0x00, MAX_SIZE);

	/* masklen should be less than MAX_SIZE */
	if ((hlen = HashLenQuery(hid)) == 0)	/* Unkown Hash Algorithm */
		return ERR_UNKNOWN_HASH;

	if (masklen % hlen)
		n = masklen / hlen + 1;
	else
		n = masklen / hlen;

	/* Preparing Hash Input/Ouput */
	data = (BYTE *)malloc(seedlen + 4);
	memcpy(data, seed, seedlen);

	hash = (BYTE *)malloc(hlen);

	for (i = 0; i < n; i++) {
		/* Constructing Hash Input */
		memcpy(data + seedlen, &i, 4);

		/* Computing Hash */
		if ((ret = Hash(hid, data, seedlen + 4, hash)) != 0) {
			free(data);
			free(hash);
			return ERR_HASH;
		}

		/* Appending Hash to T */
		memcpy(T + i*hlen, hash, hlen);
	}

	free(data);
	free(hash);

	memcpy(mask, T, masklen);

	return ERR_OK;
}

int PKCS1_RSAEP(PKCS1_RSA_PUBLIC_KEY *spk, DIGIT_T *m, DIGIT_T *c)
{
	/* Do RSA Encryption */
	mpModExp(c, m, spk->exponent, spk->modulus, spk->len);
	
	return ERR_OK;
}

int PKCS1_RSADP(PKCS1_RSA_PRIVATE_KEY *ssk, DIGIT_T *c, DIGIT_T *m)
{
	/* Do RSA Decryption */
	mpModExp(m, c, ssk->exponent, ssk->modulus, ssk->len);
	
	return ERR_OK;
}

int PKCS1_RSASP1(PKCS1_RSA_PRIVATE_KEY *ssk, DIGIT_T *m, DIGIT_T *s)
{
	/* Do RSA Signing */
	return PKCS1_RSADP(ssk, m, s);
}


int PKCS1_RSAVP1(PKCS1_RSA_PUBLIC_KEY *spk, DIGIT_T *s, DIGIT_T *m)
{
	/* Extract Encoded Message */
	return PKCS1_RSAEP(spk, s, m);

}

int PKCS1_EME_OAEP_ENC(PKCS1_RSA_PUBLIC_KEY *spk, UINT hid, BYTE *m, UINT mlen, BYTE *L, UINT llen, BYTE *em)
{
	/* Encoding message m of length mlen to em using OAEP */
	UINT hlen;	/* Hash Output Length in Byte */
	UINT k;		/* Encoded Message Length */
	BYTE *lHash;	/* Hash of L */
	BYTE *DB;
	BYTE *seed;
	BYTE *dbMask;
	BYTE *seedMask;
	int ret;
	
	k = NBYTE(spk->len);
	if((hlen = HashLenQuery(hid))==0)
		return ERR_UNKNOWN_HASH;
		
	/* Length checking */
	if(mlen > (k - 2*hlen - 2))
		return ERR_MSG_TOO_LONG;
	
	/* Compute Hash of L */
	mcrypto_dump("OAEP Encoding: L", L, llen);
	lHash = (BYTE *)malloc(hlen);
	if(Hash(hid, L, llen, lHash)!=0){
		return ERR_HASH;
	}
	mcrypto_dump("OAEP Encoding: Hash of L", lHash, hlen);
	
	/* Forming DB */
	DB = (BYTE *)malloc(k-hlen-1);
	memset(DB, 0x00, k-hlen-1);
	
	memcpy(DB, lHash, hlen);
	DB[k-hlen-mlen-2] = 0x01;
	memcpy(DB+k-hlen-mlen-1, m, mlen);
	mcrypto_dump("OAEP Encoding: DB", DB, k-hlen-1);
	
	/* Make a random seed */
	seed = (BYTE *)malloc(hlen);
	GenSeed(seed, hlen);
	mcrypto_dump("OAEP Encoding: seed", seed, hlen);
	
	/* Forming maskedDB and maskedSeed */
	dbMask = (BYTE *)malloc(k-hlen-1);
	if((ret=MGF1(hid, seed, hlen, dbMask, k-hlen-1))!=ERR_OK) {
		free(lHash);
		free(DB);
		free(seed);
		
		return ret;
	}
	mcrypto_dump("OAEP Encoding: dbMask", dbMask, k-hlen-1); 
	
	memxor(DB, DB, dbMask, k-hlen-1);
	mcrypto_dump("OAEP Encoding: maskedDB", DB, k-hlen-1);
	
	seedMask = (BYTE *)malloc(hlen);
	if((ret=MGF1(hid, DB, k-hlen-1, seedMask, hlen))!=ERR_OK) {
		free(lHash);
		free(DB);
		free(seed);
		free(seedMask);
		
		return ret;
	}

	mcrypto_dump("OAEP Encoding: seedMask", seedMask, hlen);
	
	memxor(seed, seed, seedMask, hlen);
	mcrypto_dump("OAEP Encoding: maskedSeed", seed, hlen);
	
	/* forming OAEP-encoded message */
	memset(em, 0x00, k);
	memcpy(em+1, seed, hlen);
	memcpy(em+1+hlen, DB, k-hlen-1);
	mcrypto_dump("OAEP Encoding: Encoded Message em", em, k);
	
	/* free used memory */
	free(lHash);
	free(DB);
	free(seed);
	free(dbMask);
	free(seedMask);
	
	return ERR_OK;
}

int PKCS1_RSA_OAEP_ENCRYPT(PKCS1_RSA_PUBLIC_KEY *spk, UINT hid, BYTE *m, UINT mlen, BYTE *L, UINT llen, BYTE *c)
{
	/* Encryption using RSA-OAEP */
	BYTE *em;
	int ret;
	
	mcrypto_dump("RSAOAEP Encrypt: Plaintext m", m, mlen);
	
	/* Encoding message */
	em = (BYTE *)malloc(NBYTE(spk->len));
	
	if((ret = PKCS1_EME_OAEP_ENC(spk, hid, m, mlen, L, llen, em))!=ERR_OK) {
		free(em);
		return ret;
	}
	
	/* Do Encryption */
	ret = PKCS1_RSAEP(spk, (DIGIT_T *)em, (DIGIT_T *)c);
	mcrypto_dump("RSAOAEP Encrypt: Ciphertext c", c, NBYTE(spk->len));
	
	free(em);
	
	return ret;
}
	

int PKCS1_EME_OAEP_DEC(PKCS1_RSA_PRIVATE_KEY *ssk, UINT hid, BYTE *em, BYTE *L, UINT llen, BYTE *m, UINT *mlen)
{
	/* OAEP Decoding */
	UINT hlen;	/* Hash Output Length in Byte */
	UINT k;		/* Encoded Message Length */
	BYTE *lHash;	/* Hash of L */
	BYTE *DB;
	BYTE *seed;
	BYTE *maskedDB;
	BYTE *maskedSeed;
	UINT i;
	int ret;
	
	k = NBYTE(ssk->len);
	mcrypto_dump("OAEP Decoding: Encoded Message em", em, k);
	
	if(*(em)) /* fist byte of encoded message not 0x00 */
		return ERR_DECRYPTION;
	
	if((hlen = HashLenQuery(hid))==0)
		return ERR_UNKNOWN_HASH;
		
	/* Compute Hash of L */
	mcrypto_dump("OAEP Decoding: L", L, llen);
	lHash = (BYTE *)malloc(hlen);
	if((ret=Hash(hid, L, llen, lHash))!=0) {
		free(lHash);
		
		return ret;
	}
	
	mcrypto_dump("OAEP Decoding: Hash of L", lHash, hlen);
	
	/* Extracting maskedDB and maskedSeed from encoded message em */
	maskedSeed = (BYTE *)malloc(hlen);
	memcpy(maskedSeed, em+1, hlen);
	mcrypto_dump("OAEP Decoding: maskedSeed", maskedSeed, hlen);
	
	maskedDB = (BYTE *)malloc(k-hlen-1);
	memcpy(maskedDB, em+1+hlen, k-hlen-1);
	mcrypto_dump("OAEP Decoding: maskedDB", maskedDB, k-hlen-1);
	
	/* Finding seed and DB */
	seed = (BYTE *)malloc(hlen);
	if((ret=MGF1(hid, maskedDB, k-hlen-1, seed, hlen))!=ERR_OK)
	{
		free(lHash);
		free(maskedSeed);
		free(maskedDB);
		free(seed);
		
		return ret;
	}
	mcrypto_dump("OAEP Decoding: seedMask", seed, hlen);
	
	memxor(seed, seed, maskedSeed, hlen);
	mcrypto_dump("OAEP Decoding: seed", seed, hlen);
	
	DB = (BYTE *)malloc(k-hlen-1);
	if((ret=MGF1(hid, seed, hlen, DB, k-hlen-1))!=ERR_OK) {
		free(lHash);
		free(maskedSeed);
		free(maskedDB);
		free(seed);
		free(DB);
		
		return ret;
	}

	mcrypto_dump("OAEP Decoding: dbMask", DB, k-hlen-1);
	memxor(DB, DB, maskedDB, k-hlen-1);
	mcrypto_dump("OAEP Decoding: DB", DB, k-hlen-1);
	
	/* Checking whether first hlen bits of DB equals to lHash */
	if(memcmp(lHash, DB, hlen)!=0) {
		free(lHash);
		free(DB);
		free(seed);
		free(maskedDB);
		free(maskedSeed);
		
		return ERR_DECRYPTION;
	}
	
	/* Try to Extract M from DB */
	i = hlen;
	while((DB[i] == 0x00) && (DB[i] != 0x01) && (i < (k-hlen-1-1))) i++;
	
	if(i == (k-hlen-1-1)) {
		free(lHash);
		free(DB);
		free(seed);
		free(maskedDB);
		free(maskedSeed);
		
		return ERR_DECRYPTION;
	}
	
	if(DB[i] != 0x01) {
		free(lHash);
		free(DB);
		free(seed);
		free(maskedDB);
		free(maskedSeed);
		
		return ERR_DECRYPTION;
	}	
	
	*mlen = k-hlen-1-1 - (i+1) + 1; /* starting after 0x01 byte to the end of DB */
	memcpy(m, DB+i+1, *mlen);
	mcrypto_dump("OAEP Decoding: Plaintext m", m, *mlen);
	
	free(lHash);
	free(DB);
	free(seed);
	free(maskedDB);
	free(maskedSeed);
		
	return ERR_OK;
}

int PKCS1_RSA_OAEP_DECRYPT(PKCS1_RSA_PRIVATE_KEY *ssk, UINT hid, BYTE *c, BYTE *L, UINT llen, BYTE *m, UINT *mlen)
{
	/* RSA-OAEP Decryption */
	BYTE *em;
	int ret;
	UINT hlen;
	
	if((hlen = HashLenQuery(hid)) == 0)
		return ERR_UNKNOWN_HASH;
	
	/* Length checking */
	if(NBYTE(ssk->len)<(2*hlen+2))
		return ERR_DECRYPTION;
	
	mcrypto_dump("RSAOAEP Decrypt: Ciphertext", c, NBYTE(ssk->len));
	
	/* Do RSA Decryption */
	em = (BYTE *)malloc(NBYTE(ssk->len));
	ret = PKCS1_RSADP(ssk, (DIGIT_T *)c, (DIGIT_T *)em);
	
	mcrypto_dump("RSAOAEP Decrypt: OAEP-Encoded Message (After Decryption)",(BYTE *)em, NBYTE(ssk->len));
	
	/* Decoding Message */
	ret = PKCS1_EME_OAEP_DEC(ssk, hid, em, L, llen, m, mlen);
	
	free(em); 
	
	return ret;
}

int PKCS1_RSASSA_PSS_SIGN(PKCS1_RSA_PRIVATE_KEY *ssk, UINT hid, BYTE *m, UINT mlen, UINT slen, BYTE *s)
{
	/* PKCS1 #1 RSA Signature Generation Using PSS Encoding */
	BYTE *em;
	int ret;
	
	/* Preparing encoded message */
	em = (BYTE *)malloc(NBYTE(ssk->len));
	
	/* PSS Encoding */
	if((ret=PKCS1_EMSA_PSS_ENCODE(hid, m, mlen, slen, em, NBYTE(ssk->len)))!=ERR_OK) {
		free(em);
		return ret;
	}
	
	/* Signing */
	ret = PKCS1_RSASP1(ssk, (DIGIT_T*)em, (DIGIT_T*)s);
	mcrypto_dump("Signature",(BYTE *)s, NBYTE(ssk->len));
	
	free(em);
	
	return ret;
}

int PKCS1_RSASSA_PSS_VERIFY(PKCS1_RSA_PUBLIC_KEY *spk, UINT hid, BYTE *m, UINT mlen, UINT slen, BYTE *s)
{
	/* PKCS #1 RSA Signature Verification Using PSS Encoding */
	BYTE *em;
	int ret;
	
	mcrypto_dump("Signature", s, NBYTE(spk->len));

	/* Extracting encoded message */
	em = (BYTE *)malloc(NBYTE(spk->len));
	
	if((ret = PKCS1_RSAVP1(spk, (DIGIT_T *)s, (DIGIT_T *)em))!=ERR_OK) {
		free(em);
		return ret;
	}
	mcrypto_dump("PSS-Encoded Message (Before Verificaton)",(BYTE *)em, NBYTE(spk->len));
	
	/* Verify encoded message */
	ret = PKCS1_EMSA_PSS_VERIFY(hid, m, mlen, slen, em, NBYTE(spk->len));
	
	free(em);
	
	if(ret == ERR_PSS_CONSISTENT)
		return ERR_VALID_SIGNATURE;
		
	return ERR_INVALID_SIGNATURE;
}

int PKCS1_EMSA_PSS_ENCODE(UINT hid, BYTE *m, UINT mlen, UINT slen, BYTE *em, UINT emlen)
{
	/* PSS Encoding */
	UINT hlen;	/* Length of Hash Output */
	BYTE *H;	/* Hash of m */
	BYTE *salt;
	BYTE *M;
	BYTE *DB;
	BYTE *maskedDB;
	int ret;
	
	if((hlen = HashLenQuery(hid)) == 0)
		return ERR_UNKNOWN_HASH;
	
	/* Computing Hash of m */
	mcrypto_dump("PSS Encoding: Message", m, mlen);
	H = (BYTE *)malloc(hlen);
	if((ret = Hash(hid, m, mlen, H))!=0) {
		free(H);
		
		return ret;
	}

	mcrypto_dump("PSS Encoding: Hashed Message", H, hlen);
	
	/* Length checking */
	if(emlen<(hlen+slen+2)) {
		free(H);
		return ERR_PSS_ENCODING;
	}
	
	/* Generating salt and constructing M */
	salt = (BYTE *)malloc(slen);
	GenSeed(salt, slen);
	mcrypto_dump("PSS Encoding: Salt", salt, slen);
	
	M = (BYTE *)malloc(8+hlen+slen);
	memset(M, 0x00, 8+hlen+slen);
	memcpy(M+8, H, hlen);
	memcpy(M+8+hlen, salt, slen);
	mcrypto_dump("PSS Encoding: Message to be encoded", M, 8+hlen+slen);
	
	/* Constructing DB */
	if((ret = Hash(hid, M, 8+hlen+slen, H))!=0) {
		free(H);
		free(M);
		free(salt);
		
		return ret;
	}
	mcrypto_dump("PSS Encoding: Hash of Message to be encoded", H, hlen);
	
	DB = (BYTE *)malloc(emlen-hlen-1);
	memset(DB, 0x00, emlen-hlen-1);
	DB[emlen-slen-hlen-2] = 0x01;
	memcpy(DB+emlen-slen-hlen-1, salt, slen);
	mcrypto_dump("PSS Encoding: DB", DB, emlen-hlen-1);
	
	/* Computing maskedDB */
	maskedDB = (BYTE *)malloc(emlen-hlen-1);
	if((ret=MGF1(hid, H, hlen, maskedDB, emlen-hlen-1))!=ERR_OK) {
		free(H);
		free(M);
		free(salt);
		free(maskedDB);
		free(DB);
		
		return ret;
	}

	mcrypto_dump("PSS Encoding: dbMask", maskedDB, emlen-hlen-1);
	
	memxor(maskedDB, maskedDB, DB, emlen-hlen-1);
	mcrypto_dump("PSS Encoding: maskedDB", maskedDB, emlen-hlen-1);
	
	/* Constructing encoded message, em */
	memcpy(em, maskedDB, emlen-hlen-1);
	memcpy(em+emlen-hlen-1, H, hlen);
	em[emlen-1] = 0xbc;
	mcrypto_dump("PSS Encoding: Encoded Message", em, emlen);
	
	return ERR_OK;
}

int PKCS1_EMSA_PSS_VERIFY(UINT hid, BYTE *m, UINT mlen, UINT slen, BYTE *em, UINT emlen)
{
	/* PSS Verification */
	UINT hlen;	/* Length of Hash Output */
	BYTE *H;	/* Hash of m */
	BYTE *M;
	BYTE *mHash;
	BYTE *DB;
	BYTE *maskedDB;
	int ret;
	
	if((hlen = HashLenQuery(hid)) == 0)
		return ERR_UNKNOWN_HASH;
	
	/* Computing Hash of m */
	mcrypto_dump("PSS Verify: Message", m, mlen);
	mHash = (BYTE *)malloc(hlen);
	if((ret = Hash(hid, m, mlen, mHash))!=0) {
		free(mHash);
		return ret;
	}
	
	mcrypto_dump("PSS Verify: Hash of Message", mHash, hlen);
	
	/* Length checking */
	mcrypto_dump("PSS Verify: Encoded Message", em, emlen);
	
	if(emlen<(hlen+slen+2)) {
		free(mHash);
		return ERR_PSS_INCONSISTENT;
	}
	
	/* Verification */
	if(em[emlen-1]!=0xbc) {
		free(mHash);
		return ERR_PSS_INCONSISTENT;
	}
	
	/* Extracting maskedDB and H */
	maskedDB = (BYTE *)malloc(emlen-hlen-1);
	memcpy(maskedDB, em, emlen-hlen-1);
	mcrypto_dump("PSS Verify: maskedDB", maskedDB, emlen-hlen-1);
	
	H = (BYTE *)malloc(hlen);
	memcpy(H, em+emlen-hlen-1, hlen);
	mcrypto_dump("PSS Verify: H", H, hlen);
	
	/* Computing DB */ 
	DB = (BYTE *)malloc(emlen-hlen-1);
	if((ret=MGF1(hid, H, hlen, DB, emlen-hlen-1))!=ERR_OK) {
		free(H);
		free(mHash);
		free(maskedDB);
		free(DB);
		
		return ret;
	}
	mcrypto_dump("PSS Verify: dbMask", DB, emlen-hlen-1);
	
	memxor(DB, DB, maskedDB, emlen-hlen-1);
	mcrypto_dump("PSS Verify: DB", DB, emlen-hlen-1);
	
	if(DB[emlen-slen-hlen-2]!=0x01) {
		free(H);
		free(mHash);
		free(maskedDB);
		free(DB);
		
		return ERR_PSS_INCONSISTENT;
	}
	
	M = (BYTE *)malloc(8+hlen+slen);
	memset(M, 0x00, 8+hlen+slen);
	memcpy(M+8, mHash, hlen);
	memcpy(M+8+hlen, DB+emlen-slen-hlen-1, slen);
	mcrypto_dump("PSS Verify: Message to encoded", M, 8+hlen+slen);
	
	if((ret = Hash(hid, M, 8+hlen+slen, mHash))!=0) {
		free(H);
		free(mHash);
		free(maskedDB);
		free(DB);
		free(M);
		
		return ret;
	}
	
	mcrypto_dump("PSS Verify: Hash of Message to encoded", mHash, hlen);
	
	if(memcmp(H, mHash, hlen)!=0) 
		return ERR_PSS_INCONSISTENT;
		
	return ERR_PSS_CONSISTENT;
}

void errmsg(int err)
{
	switch(err) {
	case ERR_OK:			printf("Job Done Successfully!!!\n"); break;
	case ERR_MOD_TOO_SMALL:		printf("Wow!!! Modulus Length Too Short.\n"); break;
	case ERR_MOD_TOO_LONG:		printf("Sorry!!! Modulus Length Too Long.\n"); break;
	case ERR_PRIME_FAILED:		printf("Failed To Generate A Prime. Try It Again.\n"); break;
	case ERR_MSG_TOO_LONG:		printf("Sorry!!! Message Too Long.\n"); break;
	case ERR_LABEL_TOO_LONG:	printf("Sorry!!! OAEP Label Too Long\n"); break;
	case ERR_DECRYPTION:		printf("Sorry!!! Decryption Error.\n"); break;
	case ERR_UNKNOWN_HASH:		printf("Sorry!!! Hash Function Not Available.\n"); break;
	case ERR_VALID_SIGNATURE:	printf("Good!!! Signature Is Valid.\n"); break;	
	case ERR_INVALID_SIGNATURE:	printf("Sorry!!! Signature Is Invalid.\n"); break;
	case ERR_PSS_CONSISTENT:	printf("Good!!! PSS-Encoded Message Is Consistent.\n"); break;
	case ERR_PSS_INCONSISTENT:	printf("Sorry!!! PSS-Encoded Message Is Inconsistent.\n"); break;
	case ERR_PSS_ENCODING:		printf("Sorry!!! PSS Encoding Error\n"); break;
	case ERR_HASH:			printf("Sorry!!! Hash Function Error. Maybe Hash Input Limit Exceeded\n"); break;
	default: 			printf("Unkown Error!!!\n"); break;
	}
}

int LoadPublicKey(char *fname, PKCS1_RSA_PUBLIC_KEY *spk)
{
	/* Load keys from files */
	char s[5][PKCS1_MAX_LINE_LEN];	
	FILE *f;
	UINT i;
	UINT len;
	
	f = fopen(fname, "r");
	if(f == NULL)
		return -1;
	
	memset(s, 0x00, PKCS1_MAX_LINE_LEN*5);	
	/* reading data */
	for(i=0;i<5;i++)
	{
		if(feof(f))
		{
			fclose(f);
			return -1;
		}
		fgets(s[i], PKCS1_MAX_LINE_LEN, f);
		
		/* ignore newline charater */		
		s[i][strlen(s[i])-1] = '\0';
	}
	fclose(f);
	
	/* Decoding data */
	spk->len = (UINT)atoi(s[1]);
	
	if((spk->modulus = mpBase64Decode(&len, s[2]))==NULL)
		return -1; 
	if((spk->exponent = mpBase64Decode(&len, s[3]))==NULL)
		return -1; 
	return 0;
}

int LoadPrivateKey(char *fname, PKCS1_RSA_PRIVATE_KEY *ssk)
{
	/* Load keys from files */
	char s[6][PKCS1_MAX_LINE_LEN];
	FILE *f;
	UINT i;
	UINT len;
	
	f = fopen(fname, "r");
	if(f == NULL)
		return -1;
		
	memset(s, 0x00, PKCS1_MAX_LINE_LEN*6);	
	
	/* reading data */
	for(i=0;i<5;i++)
	{
		if(feof(f))
		{
			fclose(f);
			return -1;
		}
		fgets(s[i], PKCS1_MAX_LINE_LEN, f);
		s[i][strlen(s[i])-1] = '\0';
	}
	fclose(f);
	
	ssk->len = (UINT)atoi(s[1]);
	
	if((ssk->modulus = mpBase64Decode(&len, s[2]))==NULL)
		return -1; 
	if((ssk->PublicExponent = mpBase64Decode(&len, s[3]))==NULL)
		return -1; 
	if((ssk->exponent = mpBase64Decode(&len, s[4]))==NULL)
		return -1; 	
	
	return 0;
}


