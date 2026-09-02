/* mpIsPrime.c */

#include "bigdigits.h"

static DIGIT_T SMALL_PRIMES[] = {
	3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 
	47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97, 101, 
	103, 107, 109, 113,
	127, 131, 137, 139, 149, 151, 157, 163, 167, 173,
	179, 181, 191, 193, 197, 199, 211, 223, 227, 229,
	233, 239, 241, 251, 257, 263, 269, 271, 277, 281,
	283, 293, 307, 311, 313, 317, 331, 337, 347, 349,
	353, 359, 367, 373, 379, 383, 389, 397, 401, 409,
	419, 421, 431, 433, 439, 443, 449, 457, 461, 463,
	467, 479, 487, 491, 499, 503, 509, 521, 523, 541,
	547, 557, 563, 569, 571, 577, 587, 593, 599, 601,
	607, 613, 617, 619, 631, 641, 643, 647, 653, 659,
	661, 673, 677, 683, 691, 701, 709, 719, 727, 733,
	739, 743, 751, 757, 761, 769, 773, 787, 797, 809,
	811, 821, 823, 827, 829, 839, 853, 857, 859, 863,
	877, 881, 883, 887, 907, 911, 919, 929, 937, 941,
	947, 953, 967, 971, 977, 983, 991, 997,
};

#define N_SMALL_PRIMES (sizeof(SMALL_PRIMES)/sizeof(DIGIT_T))

int mpIsPrime(const DIGIT_T w[], UINT ndigits, UINT t)
{	
	/*	Returns true if w is a probable prime 
		Carries out t iterations
		(Use t = 50 for DSS Standard) */
	/*	Uses Rabin-Miller Probabilistic Primality Test,
		Ref: FIPS-186-2 Appendix 2.
		Also Schneier 2nd ed p 260 & Knuth Vol 2, p 379.
	*/

	/* Temp big digits */
	DIGIT_T m[MAX_DIG_LEN], a[MAX_DIG_LEN], b[MAX_DIG_LEN];
	DIGIT_T z[MAX_DIG_LEN], w1[MAX_DIG_LEN];
	DIGIT_T j[MAX_DIG_LEN];
	UINT i;
	int failed, isprime;

	/* Check the obvious */
	if (mpISEVEN(w, ndigits))
		return 0;

	/* First check for small primes, unless we could be one ourself */
	if (mpShortCmp(w, SMALL_PRIMES[N_SMALL_PRIMES-1], ndigits) > 0) {
		for (i = 0; i < N_SMALL_PRIMES; i++) {
			if (mpShortMod(w, SMALL_PRIMES[i], ndigits) == 0)
			return 0; /* Failed */
		}
	}
	else {	
		/* w is a small number, so check directly */
		for (i = 0; i < N_SMALL_PRIMES; i++) {
			if (mpShortCmp(w, SMALL_PRIMES[i], ndigits) == 0)
				return 1;	/* w is a small prime */
		}
		return 0;	/* w is not a small prime */
	}

	/*	Now do Rabin-Miller  */
	/*	Step 2. Find a and m where w = 1 + (2^a)m
		m is odd and 2^a is largest power of 2 dividing w - 1 */
	mpShortSub(w1, w, 1, ndigits);		/* Store w1 = w - 1 */
	mpSetEqual(m, w1, ndigits);		/* Set m = w - 1 */
	
	/* for (a = 0; iseven(m); a++) */
	for (mpSetZero(a, ndigits); mpISEVEN(m, ndigits); mpShortAdd(a, a, 1, ndigits))
		/* Divide by 2 until m is odd */
		mpShiftRight(m, m, 1, ndigits);
	

	/* assert((1 << a) * m + 1 == w); */

	isprime = 1;
	for (i = 0; i < t; i++) {
		failed = 1;	/* Assume fail unless passed in loop */
		/* Step 3. Generate random integer 1 < b < w */
		mpSetZero(b, ndigits);
		do {
			b[0] = spPseudoRand(2, MAX_DIGIT);
		} while (mpCompare(b, w, ndigits) >= 0);

		/* assert(1 < b && b < w); */

		/* Step 4. Set j = 0 and z = b^m mod w */
		mpSetZero(j, ndigits);
		mpModExp(z, b, m, w, ndigits);
		do {
			/* Step 5. If j = 0 and z = 1, or if z = w - 1 */
			/* i.e. if ((j == 0 && z == 1) || (z == w - 1)) */
			if ((mpIsZero(j, ndigits) && mpShortCmp(z, 1, ndigits) == 0) || (mpCompare(z, w1, ndigits) == 0)) {	
				/* Passes on this loop  - go to Step 9 */
				failed = 0;
				break;
			}

			/* Step 6. If j > 0 and z = 1 */
			if (!mpIsZero(j, ndigits) && (mpShortCmp(z, 1, ndigits) == 0)) {	
				/* Fails - go to Step 8 */
				failed = 1;
				break;
			}

			/* Step 7. j = j + 1. If j < a set z = z^2 mod w */
			mpShortAdd(j, j, 1, ndigits);
			if (mpCompare(j, a, ndigits) < 0)
				mpModMult(z, z, z, w, ndigits);
			/* Loop: if j < a go to Step 5 */
		} while (mpCompare(j, a, ndigits) < 0);

		if (failed) {	
			/* Step 8. Not a prime - stop */
			isprime = 0;
			break;
		}
	}	/* Step 9. Go to Step 3 until i >= n */
	/* Else, if i = n, w is probably prime => success */

	return isprime;
}

int mpIsPrimeShort(const DIGIT_T w[], UINT ndigits)
{
	/*	Returns true if w is a probable prime */

	UINT i;
	/* Check the obvious */
	if (mpISEVEN(w, ndigits))
		return 0;

	/* First check for small primes, unless we could be one ourself */
	if (mpShortCmp(w, SMALL_PRIMES[N_SMALL_PRIMES - 1], ndigits) > 0) {
		for (i = 0; i < N_SMALL_PRIMES; i++) {
			if (mpShortMod(w, SMALL_PRIMES[i], ndigits) == 0)
				return 0; /* Failed */
		}
	}
	else {
		/* w is a small number, so check directly */
		for (i = 0; i < N_SMALL_PRIMES; i++) {
			if (mpShortCmp(w, SMALL_PRIMES[i], ndigits) == 0)
				return 1;	/* w is a small prime */
		}
		return 0;	/* w is not a small prime */
	}
	return 1;
}

int mpIsPrimeRabin(const DIGIT_T w[], UINT ndigits, UINT t)
{
	/*	Returns true if w is a probable prime
	Carries out t iterations
	(Use t = 50 for DSS Standard) */
	/*	Uses Rabin-Miller Probabilistic Primality Test,
	Ref: FIPS-186-2 Appendix 2.
	Also Schneier 2nd ed p 260 & Knuth Vol 2, p 379.
	*/

	/* Temp big digits */
	DIGIT_T m[MAX_DIG_LEN], a[MAX_DIG_LEN], b[MAX_DIG_LEN];
	DIGIT_T z[MAX_DIG_LEN], w1[MAX_DIG_LEN];
	DIGIT_T j[MAX_DIG_LEN];
	UINT i;
	int failed, isprime;

	/*	Now do Rabin-Miller  */
	/*	Step 2. Find a and m where w = 1 + (2^a)m
	m is odd and 2^a is largest power of 2 dividing w - 1 */
	mpShortSub(w1, w, 1, ndigits);		/* Store w1 = w - 1 */
	mpSetEqual(m, w1, ndigits);		/* Set m = w - 1 */

	/* for (a = 0; iseven(m); a++) */
	for (mpSetZero(a, ndigits); mpISEVEN(m, ndigits); mpShortAdd(a, a, 1, ndigits))
		/* Divide by 2 until m is odd */
		mpShiftRight(m, m, 1, ndigits);


	/* assert((1 << a) * m + 1 == w); */

	isprime = 1;
	for (i = 0; i < t; i++) {
		failed = 1;	/* Assume fail unless passed in loop */
					/* Step 3. Generate random integer 1 < b < w */
		mpSetZero(b, ndigits);
		do {
			b[0] = spPseudoRand(2, MAX_DIGIT);
		} while (mpCompare(b, w, ndigits) >= 0);

		/* assert(1 < b && b < w); */

		/* Step 4. Set j = 0 and z = b^m mod w */
		mpSetZero(j, ndigits);
		mpModExp(z, b, m, w, ndigits);
		do {
			/* Step 5. If j = 0 and z = 1, or if z = w - 1 */
			/* i.e. if ((j == 0 && z == 1) || (z == w - 1)) */
			if ((mpIsZero(j, ndigits) && mpShortCmp(z, 1, ndigits) == 0) || (mpCompare(z, w1, ndigits) == 0)) {
				/* Passes on this loop  - go to Step 9 */
				failed = 0;
				break;
			}

			/* Step 6. If j > 0 and z = 1 */
			if (!mpIsZero(j, ndigits) && (mpShortCmp(z, 1, ndigits) == 0)) {
				/* Fails - go to Step 8 */
				failed = 1;
				break;
			}

			/* Step 7. j = j + 1. If j < a set z = z^2 mod w */
			mpShortAdd(j, j, 1, ndigits);
			if (mpCompare(j, a, ndigits) < 0)
				mpModMult(z, z, z, w, ndigits);
			/* Loop: if j < a go to Step 5 */
		} while (mpCompare(j, a, ndigits) < 0);

		if (failed) {
			/* Step 8. Not a prime - stop */
			isprime = 0;
			break;
		}
	}	/* Step 9. Go to Step 3 until i >= n */
		/* Else, if i = n, w is probably prime => success */

	return isprime;
}
