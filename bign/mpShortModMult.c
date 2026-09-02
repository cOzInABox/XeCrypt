/* mpShortModMult.c */

#include "bigdigits.h"

int mpShortModMult(DIGIT_T w[], const DIGIT_T u[], DIGIT_T v, DIGIT_T m[], UINT ndigits)
{
	/*	Computes product w = u * v mod m
		where w, u, m are multiprecision integers of ndigits each
		and v are single precision digits
	*/
	
	DIGIT_T w1[MAX_DIG_LEN+1];
	DIGIT_T u1[MAX_DIG_LEN+1];
	DIGIT_T m1[MAX_DIG_LEN+1];
	
	mpSetEqual(u1, u, ndigits);
	u1[ndigits] = 0;
	
	mpSetEqual(m1, m, ndigits);
	m1[ndigits] = 0;
	
	mpShortMult(w1, u1, v, ndigits+1);
		
	mpModulo(u1, w1, ndigits+1, m1, ndigits+1);
	
	mpSetEqual(w, u1, ndigits);
		
	return 0;
	
}
