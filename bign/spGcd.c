/* spGcd.c */

#include "bigdigits.h"

DIGIT_T spGcd(DIGIT_T x, DIGIT_T y)
{	/*	Returns gcd(x, y) */

	/* Ref: Schneier 2nd ed, p245 */
	
	DIGIT_T g;

	if (x + y == 0)
		return 0;	/* Error */

	g = y;
	while (x > 0)
	{
		g = x;
		x = y % x;
		y = g;
	}
	
	return g;
}
