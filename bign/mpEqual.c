/* mpEqual.c */

#include "bigdigits.h"

int mpEqual(const DIGIT_T a[], const DIGIT_T b[], UINT ndigits)
{
	/*	Returns true if a == b, else false
	*/

	if (ndigits == 0) return -1;

	while (ndigits--){
		if (a[ndigits] != b[ndigits])
			return 0;	/* False */
	}

	return (!0);	/* True */
}

