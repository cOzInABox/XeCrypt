/* mpShortMod.c */

#include "bigdigits.h"

DIGIT_T mpShortMod(const DIGIT_T a[], DIGIT_T d, UINT ndigits)
{
	/*	Calculates r = a mod d
		where a is a multiprecision integer of ndigits
		and r, d are single precision digits
		using bit-by-bit method from left to right.

		Use remainder from divide function.
	*/

	DIGIT_T q[MAX_DIG_LEN * 2];
	DIGIT_T r = 0;

	r= mpShortDiv(q, a, d, ndigits);

	return r;
}

