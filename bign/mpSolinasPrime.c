/* mpSolinasPrime.c - generate Solinas' prime of the form p = 2^a + 2^b + 1, 0 < b < a */
/* useful for generating type-1 elliptic curve for pairing implementation */
/* todo: try p = 2^a - 2^b -1 too */

#include <stdlib.h>
#include <time.h>
#include "bigdigits.h"

UINT mpSolinasPrime(DIGIT_T p[], UINT ndigits, UINT bit_len)
{
	UINT a, b;	
	UINT i, j;
	UINT n = 1000;	/* going to try at most n times */
	
	srand((unsigned)time(NULL));	
	while(n--){
		/* first, make p = 1 */		
		mpSetDigit(p, 1, ndigits);
		
		/* let a = bit_len - 1 s.t. p's bit length is exactly bit_len */
		a = bit_len - 1;
		i = a / BITS_PER_DIGIT;
		j = a % BITS_PER_DIGIT;
		p[i] |= ((DIGIT_T)0x01) << j;
		
		/* choose b randomly from 1 to a - 1 */
		do {
			b = rand() % a;
		} while(b==0);
		i = b / BITS_PER_DIGIT;
		j = b % BITS_PER_DIGIT;
		p[i] |= ((DIGIT_T)0x01) << j;

		/* test if p is a prime number */
		if(mpIsPrime(p, ndigits, N_TEST_PRIME))
			return b;
	}	
	
	/* failed */
	return 0;
}
