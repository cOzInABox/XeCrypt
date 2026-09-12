/*
 * Streaming HMAC-SHA1.
 *
 * XeCryptHmacSha() computes an HMAC over up to three input buffers in one
 * call. The Init/Update/Final form below places no limit on the number of
 * inputs, which callers hashing a longer sequence of buffers need.
 *
 * Both forms compute the same value: HMAC(K, m) = H((K ^ opad) || H((K ^ ipad) || m)).
 */

#include <string.h>
#include "xecrypt.h"

void XeCryptHmacShaInit(PXECRYPT_HMAC_SHA_STATE pHmacState,
                        const unsigned char *pbKey, unsigned int cbKey)
{
	unsigned int i;
	unsigned char K[XECRYPT_HMAC_SHA_MAX_KEY_SZ];
	unsigned char Pad[XECRYPT_HMAC_SHA_MAX_KEY_SZ];

	/* pad the key out to the block size, but do not exceed it */
	memset(K, 0, XECRYPT_HMAC_SHA_MAX_KEY_SZ);
	if (cbKey <= XECRYPT_HMAC_SHA_MAX_KEY_SZ)
		memcpy(K, pbKey, cbKey);
	else
		memcpy(K, pbKey, XECRYPT_HMAC_SHA_MAX_KEY_SZ);

	XeCryptShaInit(&pHmacState->Inner);
	XeCryptShaInit(&pHmacState->Outer);

	for (i = 0; i < XECRYPT_HMAC_SHA_MAX_KEY_SZ; i++)
		Pad[i] = K[i] ^ 0x36;
	XeCryptShaUpdate(&pHmacState->Inner, Pad, XECRYPT_HMAC_SHA_MAX_KEY_SZ);

	for (i = 0; i < XECRYPT_HMAC_SHA_MAX_KEY_SZ; i++)
		Pad[i] = K[i] ^ 0x5C;
	XeCryptShaUpdate(&pHmacState->Outer, Pad, XECRYPT_HMAC_SHA_MAX_KEY_SZ);
}

void XeCryptHmacShaUpdate(PXECRYPT_HMAC_SHA_STATE pHmacState,
                          const unsigned char *pbInp, unsigned int cbInp)
{
	XeCryptShaUpdate(&pHmacState->Inner, (unsigned char *)pbInp, cbInp);
}

void XeCryptHmacShaFinal(PXECRYPT_HMAC_SHA_STATE pHmacState,
                         unsigned char *pbOut, unsigned int cbOut)
{
	unsigned char Digest[XECRYPT_SHA_DIGEST_SIZE];

	XeCryptShaFinal(&pHmacState->Inner, Digest, XECRYPT_SHA_DIGEST_SIZE);
	XeCryptShaUpdate(&pHmacState->Outer, Digest, XECRYPT_SHA_DIGEST_SIZE);
	XeCryptShaFinal(&pHmacState->Outer, pbOut, cbOut);
}
