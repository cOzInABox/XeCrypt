/* mpHalfMod.c */


#include "bigdigits.h"

DIGIT_T mpHalfMod(const DIGIT_T a[], HALF_DIGIT_T d, UINT ndigits)
{
	/*	Calculates r = a mod d
		where a is a multiprecision integer of ndigits
		and r, d are single precision digits
		using bit-by-bit method from left to right.

		Ref: Derived from principles in PGP by Phil Zimmermann
		Note: This method will only work until r <<= 1 overflows.
		i.e. for d < HIBITMASK, but we keep HALF_DIGIT
		limit for safety 
		(and also because we don't have a 31/32nds_digit).
	*/

	DIGIT_T mask = HIBITMASK;
	DIGIT_T r = 0;

	if (ndigits == 0) return 0;

	while (ndigits){	
		/* Work from left to right */
		r <<= 1;	/* Multiply remainder by 2 */

		/* Look at current bit */
		if (a[ndigits-1] & mask)
			r++;
		if (r >= d)
			r -= d;

		/* Move to next bit */
		if (mask == 1){
			mask = HIBITMASK;
			ndigits--;
		}
		else
			mask >>= 1;
	}
	
	return r;
}

