/* spModMult.c */

#include "bigdigits.h"

int spModMult(DIGIT_T *a, DIGIT_T x, DIGIT_T y, DIGIT_T m)
{	/*	Computes a = (x * y) mod m */
	
	/* Calc p[2] = x * y */
	DIGIT_T p[2];
	spMultiply(p, x, y);

	/* Then modulo */
	*a = mpShortMod(p, m, 2);
	
	return 0;
}

