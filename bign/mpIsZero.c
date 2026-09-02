/* mpIsZero.c */

#include "bigdigits.h"

int mpIsZero(const DIGIT_T a[], UINT ndigits)
{
	UINT i;
	if (ndigits == 0) 
		return -1;

	/* Start at lsb */	
	for (i = 0; i < ndigits; i++){
		if (a[i] != 0)
			return 0;	/* False */
	}

	return (!0);	/* True */
}

