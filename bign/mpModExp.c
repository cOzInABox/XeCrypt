/* mpModExp.c */

#include "bigdigits.h"
#include <stdio.h>

int mpModExp(DIGIT_T y[], const DIGIT_T x[], const DIGIT_T e[], const DIGIT_T m[], UINT ndigits)
{	/*	Computes y = x^e mod m */
	/*	Binary left-to-right method
	*/
	DIGIT_T mask;
	UINT n;
	
	if (ndigits == 0) return -1;

	/* Find second-most significant bit in e */
	n = mpSizeof(e, ndigits);
	for (mask = HIBITMASK; mask > 0; mask >>= 1){
		if (e[n-1] & mask)
			break;
	}
	mpNEXTBITMASK(mask, n);

	/* Set y = x */
	mpSetEqual(y, x, ndigits);

// 	printf("n = %x\n", n);
	/* For bit j = k-2 downto 0 step -1 */
	while (n){
		//printf("modmult Square\n");
		mpModMult(y, y, y, m, ndigits);		/* Square */
		if (e[n-1] & mask)
		{
			//printf("modmult Multiply\n");

			mpModMult(y, y, x, m, ndigits);	/* Multiply */
		}
		
		/* Move to next bit */
		mpNEXTBITMASK(mask, n);
	}

	return 0;
}



