/* Hash wrapper functions */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mcrypto.h"
#include "bigdigits.h"
#include "hash.h"
#include "../sha/sha.h"

//#include "md5.h"
//#include "sha1.h"
//#include "sha2.h"

void GenSeed(BYTE *p, UINT len)
{
	prng(p, len);
}

UINT HashLenQuery(UINT hid)
{
	UINT hlen;	/* in byte */
	
	switch(hid) {
	case HASH_MD5:		hlen = 16; break;
	case HASH_SHA1:		hlen = 20; break;
	case HASH_SHA256:	hlen = 32; break;
	case HASH_SHA384:	hlen = 48; break;
	case HASH_SHA512:	hlen = 64; break;
	default: hlen = 0;
	}
 	
 	return hlen;
}

//int MD5Hash(BYTE *data, UINT len, BYTE *hash)
//{
//	/* To do: length of input checking */
//	MD5_CTX md5context;
//	
//	MD5Init(&md5context);
//	MD5Update(&md5context, data, len);
//	MD5Final(&md5context);
//	
//	memcpy(hash, md5context.digest, 16);
//	
//	return 0;
//}

int SHA1Hash(BYTE *data, UINT len, BYTE *hash)
{
	/* To do: length of input checking */
	SHA_CTX sha1context;
	
	SHA1_Init(&sha1context);
	SHA1_Update(&sha1context, data, len);
	SHA1_Final(hash, &sha1context);
	
	return 0;
}

//int SHA256Hash(BYTE *data, UINT len, BYTE *hash)
//{
//	/* To do: length of input checking */
//	sha256_ctx sha256context;
//	
//	sha256_begin(&sha256context);
//	sha256_hash(data, len, &sha256context);
//	sha256_end(hash, &sha256context);
//	
//	return 0;
//}
//
//int SHA384Hash(BYTE *data, UINT len, BYTE *hash)
//{
//	/* To do: length of input checking */
//	sha384_ctx sha384context;
//	
//	sha384_begin(&sha384context);
//	sha384_hash(data, len, &sha384context);
//	sha384_end(hash, &sha384context);
//	
//	return 0;
//}
//
//int SHA512Hash(BYTE *data, UINT len, BYTE *hash)
//{
//	/* To do: length of input checking */
//	sha512_ctx sha512context;
//	
//	sha512_begin(&sha512context);
//	sha512_hash(data, len, &sha512context);
//	sha512_end(hash, &sha512context);
//	
//	return 0;
//}

int Hash(UINT hid, BYTE *data, UINT len, BYTE *hash)
{
	/* Compute hash value of data */
	switch(hid) {
	//case HASH_MD5:	  return MD5Hash(data, len, hash); 
	case HASH_SHA1:	  return SHA1Hash(data, len, hash); 
	//case HASH_SHA256: return SHA256Hash(data, len, hash);
	//case HASH_SHA384: return SHA384Hash(data, len, hash);
	//case HASH_SHA512: return SHA512Hash(data, len, hash);
	default: return -1;	/* unknown hash algorithm */
	}
	
	return 0;
}

int Hash2BigInt(UINT hid, BYTE* data, UINT len, DIGIT_T *p)
{
	/* have to allocate memory for p yourself */
	return Hash(hid, data, len, (BYTE *)p);
}
