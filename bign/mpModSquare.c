/* Modulo Squaring */

#include "bigdigits.h"

int mpModSquare(DIGIT_T w[], const DIGIT_T u[], const DIGIT_T p[], UINT ndigits)
{
	/* Computes w = u^2 mod p */
	
	DIGIT_T t[MAX_DIG_LEN * 2];

	/* Calc t[2n] = u^2 */
	mpSquare(t, u, ndigits);

	/* Then modulo */
	mpModulo(w, t, ndigits * 2, (DIGIT_T*)p, ndigits);

	return 0;
}
