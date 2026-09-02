/* mpModInv.c */

#include "bigdigits.h"

int mpModInv(DIGIT_T inv[], const DIGIT_T u[], const DIGIT_T v[], UINT ndigits)
{	/*	Computes inv = u^(-1) mod v */
	/*	Ref: Knuth Algorithm X Vol 2 p 342
		ignoring u2, v2, t2
		and avoiding negative numbers.
	*/
	/* Allocate temp variables */
	DIGIT_T u1[MAX_DIG_LEN], u3[MAX_DIG_LEN], v1[MAX_DIG_LEN], v3[MAX_DIG_LEN];
	DIGIT_T t1[MAX_DIG_LEN], t3[MAX_DIG_LEN], q[MAX_DIG_LEN];
	DIGIT_T w[2*MAX_DIG_LEN];
	/* TODO: CHECK LENGTHS HERE */
	int bIterations;

	/* Step X1. Initialise */
	mpSetDigit(u1, 1, ndigits);		/* u1 = 1 */
	mpSetEqual(u3, u, ndigits);		/* u3 = u */
	mpSetZero(v1, ndigits);			/* v1 = 0 */
	mpSetEqual(v3, v, ndigits);		/* v3 = v */

	bIterations = 1;	/* Remember odd/even iterations */
	while (!mpIsZero(v3, ndigits))		/* Step X2. Loop while v3 != 0 */
	{					/* Step X3. Divide and "Subtract" */
		mpDivide(q, t3, u3, ndigits, v3, ndigits);
						/* q = u3 / v3, t3 = u3 % v3 */
		mpMultiply(w, q, v1, ndigits);	/* w = q * v1 */
		mpAdd(t1, u1, w, ndigits);		/* t1 = u1 + w */

		/* Swap u1 = v1; v1 = t1; u3 = v3; v3 = t3 */
		mpSetEqual(u1, v1, ndigits);
		mpSetEqual(v1, t1, ndigits);
		mpSetEqual(u3, v3, ndigits);
		mpSetEqual(v3, t3, ndigits);

		bIterations = -bIterations;
	}

	if (bIterations < 0)
		mpSubtract(inv, v, u1, ndigits);	/* inv = v - u1 */
	else
		mpSetEqual(inv, u1, ndigits);		/* inv = u1 */

	
	return 0;
}

