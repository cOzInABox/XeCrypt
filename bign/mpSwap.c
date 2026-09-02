/* Swap two integers */

#include "bigdigits.h"

int mpSwap(DIGIT_T a[], DIGIT_T b[], UINT len)
{
	DIGIT_T temp[MAX_DIG_LEN];
	
	while(len--) {
		temp[len] = a[len];
		a[len] = b[len];
		b[len] = temp[len];
	}
	
	return 0;
}
