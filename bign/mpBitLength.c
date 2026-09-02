/* mpBitLength - return bit length of a big integer */
#include "bigdigits.h"

UINT mpBitLength(const DIGIT_T d[], UINT ndigits)
{
	UINT n, i, bits;
	DIGIT_T mask;

	if (!d || ndigits == 0)
		return 0;

	n = mpSizeof(d, ndigits);
	if (0 == n) return 0;

	for (i = 0, mask = HIBITMASK; mask > 0; mask >>= 1, i++)
	{
		if (d[n-1] & mask)
			break;
	}

	bits = n * BITS_PER_DIGIT - i;

	return bits;
}
