/* Hash wrapper header file */
#ifndef _HASH_H_
#define _HASH_H_

#include "bigdigits.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Message-digest algorithms. */
#define HASH_MD5	1
#define HASH_SHA1	2
#define HASH_SHA256	3 
#define HASH_SHA384	4
#define HASH_SHA512	5

void GenSeed(BYTE *p, UINT len);
	/* Generate a len-byte pseudorandom number for hash seed */

int MD5Hash(BYTE *data, UINT len, BYTE *hash);
	/* MD5 Hash Function Call Interface */
	
int SHA1Hash(BYTE *data, UINT len, BYTE *hash);
	/* SHA1 Hash Function Call Interface */

int SHA256Hash(BYTE *data, UINT len, BYTE *hash);
	/* SHA256 Hash Function Call Interface */
	
int SHA384Hash(BYTE *data, UINT len, BYTE *hash);
	/* SHA384 Hash Function Call Interface */

int SHA512Hash(BYTE *data, UINT len, BYTE *hash);
	/* SHA512 Hash Function Call Interface */

int Hash(UINT hid, BYTE *data, UINT len, BYTE *hash);
	/* Generic Hash Function Call Interface */

UINT HashLenQuery(UINT hid);
	/* Query Output Length of a Hash Function Specified by hid */

int Hash2BigInt(UINT hid, BYTE* data, UINT len, DIGIT_T *p);
	/* Hash directly to big integer */

#ifdef __cplusplus
}
#endif

#endif	/* _HASH_H_ */
