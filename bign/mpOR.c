/* mpOR : multi-precision bitwise OR */

#include "bigdigits.h"

void mpOR(DIGIT_T a[], const DIGIT_T x[], const DIGIT_T y[], UINT ndigits)	
{
	while(ndigits--)
		 a[ndigits] = x[ndigits] | y[ndigits];
}
