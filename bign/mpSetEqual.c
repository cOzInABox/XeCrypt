/* mpSetEqual.c */

#include "bigdigits.h"

void mpSetEqual(DIGIT_T a[], const DIGIT_T b[], UINT ndigits)
{	
	/* Sets a = b */
		
	while(ndigits--)
		a[ndigits] = b[ndigits];
}
