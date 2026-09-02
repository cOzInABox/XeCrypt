/* spIsPrime.c */

#include <assert.h>
#include "bigdigits.h"

static DIGIT_T SMALL_PRIMES[] = { 2, 3, 5, 7, 11, 13, 17, 19 };
#define N_SMALL_PRIMES sizeof(SMALL_PRIMES)/sizeof(DIGIT_T)

int spIsPrime(DIGIT_T w, UINT t)
{	/*	Returns true if w is a probable prime 
		Carries out t iterations
		(Use t = 50 for DSS Standard) 
	*/
	/*	Uses Rabin-Miller Probabilistic Primality Test,
		Ref: FIPS-186-2 Appendix 2.
		Also Schneier 2nd ed p 260 & Knuth Vol 2, p 379.
	*/
	/*	Rabin-Miller Probabilistic Primality Test,
		from FIPS-186-2 Appendix 2.
		Also Schneier 2nd ed p 260 & Knuth Vol 2, p 379.
	*/

	UINT i, j;
	DIGIT_T m, a, b, z;
	int failed;

	/*	First check for small primes */
	for (i = 0; i < N_SMALL_PRIMES; i++)
	{
		if (w % SMALL_PRIMES[i] == 0)
			return 0;	/* Failed */
	}

	/*	Now do Rabin-Miller  */
	/*	Step 2. Find a and m where w = 1 + (2^a)m
		m is odd and 2^a is largest power of 2 dividing w - 1 */
	m = w - 1;
	for (a = 0; ISEVEN(m); a++)
		m >>= 1;	/* Divide by 2 until m is odd */

	/*
	assert((1 << a) * m + 1 == w);
	*/

	for (i = 0; i < t; i++)
	{
		failed = 1;	/* Assume fail unless passed in loop */
		/* Step 3. Generate a random integer 1 < b < w */
		b = spPseudoRand(2, w - 1);

		/*
		assert(1 < b && b < w);
		*/

		/* Step 4. Set j = 0 and z = b^m mod w */
		j = 0;
		spModExp(&z, b, m, w);
		do
		{
			/* Step 5. If j = 0 and z = 1, or if z = w - 1 */
			if ((j == 0 && z == 1) || (z == w - 1))
			{	/* Passes on this loop  - go to Step 9 */
				failed = 0;
				break;
			}

			/* Step 6. If j > 0 and z = 1 */
			if (j > 0 && z == 1)
			{	/* Fails - go to Step 8 */
				failed = 1;
				break;
			}

			/* Step 7. j = j + 1. If j < a set z = z^2 mod w */
			j++;
			if (j < a)
				spModMult(&z, z, z, w);
			/* Loop: if j < a go to Step 5 */
		} while (j < a);

		if (failed)
		{	/* Step 8. Not a prime - stop */
			return 0;
		}
	}	/* Step 9. Go to Step 3 until i >= n */
	/* If got here, probably prime => success */
	
	return 1;
}
