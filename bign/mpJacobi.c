/* Jacobi Symbol Computation */ 
#include "bigdigits.h"

int mpJacobi(int *val, const DIGIT_T a[], const DIGIT_T m[], UINT len)
{
	/* Compute Jacobi symbol val = (a/m) */
	
	int j = 1;
	DIGIT_T temp_a[MAX_DIG_LEN], temp_m[MAX_DIG_LEN];
	
	/* return error if n is not odd */
	if(mpISEVEN(m, len))
		return -1;
	
	mpSetEqual(temp_a, a, len);
	mpSetEqual(temp_m, m, len);
	
	while(!mpIsZero(temp_a, len)){
		while(mpISEVEN(temp_a, len)){
			/* a = a / 2 */
			mpShiftRight(temp_a, temp_a, 1, len); 
			
			/* if (m mod 8 = 3) or (m mod 8 = 5) */
			if((mpShortMod(temp_m, 8, len) == 3) || (mpShortMod(temp_m, 8, len) == 5))
				j = -j;
		}
		/* a <--> m */
		mpSwap(temp_a, temp_m, len);
		
		/* if (a mod 4 = 3) and (m mod 4 = 3) */
		if((mpShortMod(temp_a, 4, len) == 3) && (mpShortMod(temp_m, 4, len) == 3))
			j = -j;
		
		/* a = a mod m */
		mpModulo(temp_a, temp_a, len, temp_m, len);
	}
	/* if m = 1 */
	if(mpShortCmp(temp_m, 1, len)==0)
		*val = j;
	else
		*val = 0;
	
	return 0;
}
