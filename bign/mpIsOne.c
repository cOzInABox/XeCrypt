/* mpIsOne.c */

#include "bigdigits.h"

int mpIsOne(const DIGIT_T a[], UINT ndigits)
{
	UINT i;
	if (ndigits == 0) 
		return -1;

	for (i = 1; i < ndigits; i++){
		if (a[i] != 0)
			return 0;	/* False */
	}

	return (a[0]==1);
}

