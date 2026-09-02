/* spModInv.c */

#include "bigdigits.h"

int spModInv(DIGIT_T *inv, DIGIT_T u, DIGIT_T v)
{	/*	Computes inv = u^(-1) mod v */
	/*	Ref: Knuth Algorithm X Vol 2 p 342 
		ignoring u2, v2, t2
		and avoiding negative numbers
	*/
	DIGIT_T u1, u3, v1, v3, t1, t3, q, w;
	int bIterations = 1;
	
	/* Step X1. Initialise */
	u1 = 1;
	u3 = u;
	v1 = 0;
	v3 = v;

	while (v3 != 0)	/* Step X2. */
	{	/* Step X3. */
		q = u3 / v3;	/* Divide and */
		t3 = u3 % v3;
		w = q * v1;		/* "Subtract" */
		t1 = u1 + w;
		/* Swap */
		u1 = v1;
		v1 = t1;
		u3 = v3;
		v3 = t3;
		bIterations = -bIterations;
	}

	if (bIterations < 0)
		*inv = v - u1;
	else
		*inv = u1;

	return 0;
}

