/* mpGcd.c */

#include "bigdigits.h"

int mpGcd(DIGIT_T g[], const DIGIT_T x[], const DIGIT_T y[], UINT ndigits)
{	
	/* Computes g = gcd(x, y) */
	/* Ref: Schneier  */

	/*	NB This function requires temp storage 
	*/
	DIGIT_T yy[MAX_DIG_LEN], xx[MAX_DIG_LEN];
	
	mpSetEqual(xx, x, ndigits);
	mpSetEqual(yy, y, ndigits);

	mpSetEqual(g, yy, ndigits);		/* g = y */
	
	while (!mpIsZero(xx, ndigits)){	
		mpSetEqual(g, xx, ndigits);	/* g = x */
		mpModulo(xx, yy, ndigits, xx, ndigits);	/* x = y mod x */
		mpSetEqual(yy, g, ndigits);	/* y = g; */
	}

	return 0;	/* gcd is in g */
}
