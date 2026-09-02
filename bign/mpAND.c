/* mpAND.c: multi-precision bitwise AND */

#include "bigdigits.h"

void mpAND(DIGIT_T a[], const DIGIT_T x[], const DIGIT_T y[], UINT ndigits)	
{
	while(ndigits--)
		a[ndigits] = x[ndigits] & y[ndigits];
}
