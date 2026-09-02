/* mpSetDigit.c */

#include "bigdigits.h"

void mpSetDigit(DIGIT_T a[], DIGIT_T d, UINT ndigits)
{	
	/* Sets a = d where d is a single digit */
	UINT i;
	
	for (i = 1; i < ndigits; i++)
	{
		a[i] = 0;
	}
	a[0] = d;
}
