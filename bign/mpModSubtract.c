/* mpModSubtract.c */

#include "bigdigits.h"

int mpModSubtract(DIGIT_T w[], const DIGIT_T u[], const DIGIT_T v[], const DIGIT_T m[], UINT ndigits)
{
	/*	Computes product w = u - v mod m
		where w, u, v, m are multiprecision integers of ndigits each
	*/
	
	DIGIT_T w1[MAX_DIG_LEN+1];
	DIGIT_T u1[MAX_DIG_LEN+1];
	DIGIT_T v1[MAX_DIG_LEN+1];
	DIGIT_T m1[MAX_DIG_LEN+1];
	
	mpSetEqual(u1, u, ndigits);
	u1[ndigits] = 0;
	
	mpSetEqual(v1, v, ndigits);
	v1[ndigits] = 0;
	
	mpSetEqual(m1, m, ndigits);
	m1[ndigits] = 0;
	
	while(mpCompare(u1, v1, ndigits+1) < 0){ /* u1 < v1 */
		/* u1 = u1 + m1 */
		mpAdd(u1, u1, m1, ndigits+1);
	}
	
	mpSubtract(w1, u1, v1, ndigits+1);
	
	mpModulo(u1, w1, ndigits+1, m1, ndigits+1);
	
	mpSetEqual(w, u1, ndigits);
	
	return 0;
	
}
