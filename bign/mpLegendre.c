/* Legendre Symbol Computation */ 

#include "bigdigits.h"

int mpLegendre(int *val, const DIGIT_T a[], const DIGIT_T p[], UINT len)
{
	return mpJacobi(val, a, p, len);
}
