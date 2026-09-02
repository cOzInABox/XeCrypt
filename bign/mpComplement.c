/* mpComplement.c: multi-precicion bitwise complement */

#include "bigdigits.h"

void mpComplement(DIGIT_T a[], const DIGIT_T b[], UINT ndigits)
{
	while(ndigits--)
		 a[ndigits] = ~b[ndigits];
}
