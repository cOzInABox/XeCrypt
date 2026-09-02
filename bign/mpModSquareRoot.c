/* Compute Modulo Square Root */

#include "bigdigits.h"

int mpModSquareRootPre(UINT *S, DIGIT_T Q[], DIGIT_T V[], const DIGIT_T p[], UINT len)
{
	/* Precomputation for computing square root */
	DIGIT_T W[MAX_DIG_LEN];
	int val = 0;
	
	/* find S and Q such that P - 1 = Q*2^S, Q is odd */
	mpShortSub(Q, p, 1, len);
	*S = 0;
	while(mpISEVEN(Q, len)){
		mpShiftRight(Q, Q, 1, len);
		(*S)++;
	}
	
	/* find a quadratic nonresidue W mod p and compute V = W^Q mod p */
	mpMakeRandom(W, len);
	while(val!=-1){
		mpShortAdd(W, W, 1, len);
		
		/* Test Legendre Symbol */
		if(mpLegendre(&val, W, p, len)!=0)
			return -1;	/* error */	
	}
	
	mpModExp(V, W, Q, p, len);
	
	return 0;
}


int mpModSquareRoot(DIGIT_T x[], const DIGIT_T a[], const DIGIT_T p[], UINT S, const DIGIT_T Q[], const DIGIT_T V[], const DIGIT_T a1[], UINT len)
{
	/* Shanks-Tonelli Algorithm */
	int i;
	UINT T;
	DIGIT_T e[MAX_DIG_LEN];
	DIGIT_T w[MAX_DIG_LEN];

	/* Check Legendre symbol (a/p) first */
	if(mpLegendre(&i, a, p, len) != 0)
		return -2; /* error */
	
	if(i != 1)	/* a has no square root */
		return -1;
	
	/* Compute square root modulo p of a using Shanks-Tonelli algorithm */
	
	/* x = a^[(Q+1)/2] mod p - first approximation */
	mpShortAdd(e, Q, 1, len);
	mpShiftRight(e, e, 1, len);
		
	mpModExp(x, a, e, p, len);
	
	if(S==1)
		/* Done, a^[(Q+1)/2] mod p is square root modulo p of a */	
		return 0;
	
	while(1)
	{
		/* w = x^2*a1 mod p */
		mpModSquare(e, x, p, len);
		mpModMult(w, e, a1, p, len);
		
		if(mpShortCmp(w, 1 , len) == 0)
			/* Done since x^2*a^-1 = 1 mod p */
			return 0;
				
		/* Looking for better approximation of x */
		T = 0;
		while(mpShortCmp(w, 1 , len))
		{
			/* w = w^2 mod p */
			mpSetEqual(e, w, len);
			mpModSquare(w, e, p, len);
			
			T++;
		}
		
		/* Compute new approximation x' = x*V^[2^(S-T-1)] */
		mpSetEqual(e, V, len);
		for(i=0;i < (int)(S-T-1);i++)
		{
			mpModSquare(w, e, p, len);
			mpSetEqual(e, w, len);
		}
		
		mpModMult(e, w, x, p, len);
		mpSetEqual(x, e, len);
	}
		
	return 0;
}
