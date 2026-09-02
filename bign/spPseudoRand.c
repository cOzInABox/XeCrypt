/* spPseudoRand */

#include <stdlib.h>
#include <time.h>
#include "bigdigits.h"

DIGIT_T spPseudoRand(DIGIT_T lower, DIGIT_T upper)
{	
	/*	Returns a pseudo-random digit.
		Handles own seeding using time
		NOT for cryptographically-secure random numbers.
	*/
	static unsigned seeded = 0;
	UINT i;
	double f;

	if (!seeded)
	{
		/* seed with system time */
		srand((unsigned)time(NULL));
		/* Throw away a random few to avoid similar starts */
		i = rand() & 0xFF;
		while (i--)
			rand();
		seeded = 1;
	}
	f = (double)rand() / RAND_MAX * upper;
	
	return (lower + (DIGIT_T)f);
}
