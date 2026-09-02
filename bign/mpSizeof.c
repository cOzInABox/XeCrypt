/* mpSizeof.c */

#include "bigdigits.h"

UINT mpSizeof(const DIGIT_T a[], UINT ndigits)
{	/* Returns size of significant digits in a */
	
	while(ndigits--){
		if (a[ndigits] != 0)
			return (++ndigits);
	}
	
	return 0;
}
