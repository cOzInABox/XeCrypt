/* mpXOR : multi-precision bitwise XOR */

#include "bigdigits.h"

void mpXOR(DIGIT_T a[], const DIGIT_T x[], const DIGIT_T y[], UINT ndigits)	
{
	while(ndigits--)
		a[ndigits] = x[ndigits] ^ y[ndigits];
}
