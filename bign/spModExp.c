/* spModExp.c */

#include "bigdigits.h"

//static int spModExpK(DIGIT_T *exp, DIGIT_T x, 
//			DIGIT_T n, DIGIT_T d);
static int spModExpB(DIGIT_T *exp, DIGIT_T x, 
			DIGIT_T n, DIGIT_T d);

int spModExp(DIGIT_T *exp, DIGIT_T x, 
			DIGIT_T n, DIGIT_T d)
{
	return spModExpB(exp, x, n, d);
}

//static int spModExpK(DIGIT_T *exp, DIGIT_T x, 
//			DIGIT_T n, DIGIT_T d)
//{	/*	Computes exp = x^n mod d */
//	/*	Ref: Knuth Vol 2 Ch 4.6.3 p 462 Algorithm A
//	*/
//	DIGIT_T y = 1;		/* Step A1. Initialise */
//
//	while (n > 0)
//	{							/* Step A2. Halve N */
//		if (n & 0x1)			/* If odd */
//			spModMult(&y, y, x, d);	/*   Step A3. Multiply Y by Z */	
//		
//		n >>= 1;					/* Halve N */
//		if (n > 0)				/* Step A4. N = 0? Y is answer */
//			spModMult(&x, x, x, d);	/* Step A5. Square Z */
//	}
//
//	*exp = y;
//	return 0;
//}

static int spModExpB(DIGIT_T *exp, DIGIT_T x, 
			DIGIT_T e, DIGIT_T m)
{	/*	Computes exp = x^e mod m */
	/*	Binary left-to-right method
	*/
	DIGIT_T mask;
	DIGIT_T y;	/* Temp variable */

	/* Find most significant bit in e */
	for (mask = HIBITMASK; mask > 0; mask >>= 1)
	{
		if (e & mask)
			break;
	}

	y = x;
	/* For j = k-2 downto 0 step -1 */
	for (mask >>= 1; mask > 0; mask >>= 1)
	{
		spModMult(&y, y, y, m);		/* Square */
		if (e & mask)
			spModMult(&y, y, x, m);	/* Multiply */
	}

	*exp = y;
	
	return 0;
}
