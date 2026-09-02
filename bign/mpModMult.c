/* mpModMult.c */

#include "bigdigits.h"

int mpModMult(DIGIT_T a[], const DIGIT_T x[], const DIGIT_T y[], const DIGIT_T m[], UINT ndigits)
{	
	/*	Computes a = (x * y) mod m */
	
	/* Double-length temp variable */
	DIGIT_T p[MAX_DIG_LEN * 2];

	/* Calc p[2n] = x * y */
	mpMultiply(p, x, y, ndigits);

	/* Then modulo */
	mpModulo(a, p, ndigits * 2, (DIGIT_T*)m, ndigits);

	return 0;
}

