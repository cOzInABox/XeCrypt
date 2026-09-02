/* mpModulo.c */

#include "bigdigits.h"

int mpModulo(DIGIT_T r[], const DIGIT_T u[], UINT udigits, DIGIT_T v[], UINT vdigits)
{
	/*	Calculates r = u mod v
		where r, v are multiprecision integers of length vdigits
		and u is a multiprecision integer of length udigits.
		r may overlap v.

		Note that r here is only vdigits long, 
		whereas in mpDivide it is udigits long.

		Use remainder from mpDivide function.
	*/

	/* Double-length temp variable for divide fn */
	DIGIT_T qq[MAX_DIG_LEN * 2];
	/* Use a double-length temp for r to allow overlap of r and v */
	DIGIT_T rr[MAX_DIG_LEN * 2];

	/* rr[2n] = u[2n] mod v[n] */
	mpDivide(qq, rr, u, udigits, v, vdigits);

	mpSetEqual(r, rr, vdigits);
	return 0;
}

