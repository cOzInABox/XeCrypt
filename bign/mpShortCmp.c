/* mpShortCmp.c */

#include "bigdigits.h"

int mpShortCmp(const DIGIT_T a[], DIGIT_T b, UINT ndigits)
{
	/* Returns sign of (a - b) where b is a single digit */

	UINT i;

	if (ndigits == 0) return 0;

	for (i = 1; i < ndigits; i++){
		if (a[i] != 0)
			return 1;	/* GT */
	}

	if (a[0] < b)
		return -1;		/* LT */
	else if (a[0] > b)
		return 1;		/* GT */

	return 0;	/* EQ */
}

