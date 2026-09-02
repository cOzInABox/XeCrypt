/* mpCompare.c */

#include "bigdigits.h"

int mpCompare(const DIGIT_T a[], const DIGIT_T b[], UINT ndigits)
{
	/*	Returns sign of (a - b)
	*/

	if (ndigits == 0) return 0;

	while (ndigits--){
		if (a[ndigits] > b[ndigits])
			return 1;	/* GT */
		if (a[ndigits] < b[ndigits])
			return -1;	/* LT */
	}

	return 0;	/* EQ */
}

