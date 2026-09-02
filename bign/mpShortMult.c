/* mpShortMult.c */

#include "bigdigits.h"

DIGIT_T mpShortMult(DIGIT_T w[], const DIGIT_T u[], DIGIT_T v, UINT ndigits)
{
	/*	Computes product w = u * v
		Returns overflow k
		where w, u are multiprecision integers of ndigits each
		and v, k are single precision digits

		Ref: Knuth Algorithm M.
	*/

	DIGIT_T k, t[2];
	UINT j;

	
	if (v == 0) {
		mpSetZero(w, ndigits);
		
		return 0;
	}

	k = 0;
	for (j = 0; j < ndigits; j++){
		/* t = x_i * v */
		spMultiply(t, u[j], v);
		/* w_i = LOHALF(t) + carry */
		w[j] = t[0] + k;
		/* Overflow? */
		if (w[j] < k)
			t[1]++;
		/* Carry forward HIHALF(t) */
		k = t[1];
	}

	return k;
}

