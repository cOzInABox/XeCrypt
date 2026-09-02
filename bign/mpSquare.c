/* Multi-precision squaring */

#include <assert.h>
#include "bigdigits.h"

int mpSquare(DIGIT_T w[], const DIGIT_T u[], UINT ndigits)
{
	/* Ref: Modified Squaring Algorithm by Jorge Guajardo and Christof Paar */
	UINT i, j;
	DIGIT_T t[2];
	DIGIT_T prod[2];
	DIGIT_T C1;
	DIGIT_T C2;
	
	assert(w != u);

	mpSetZero(w, 2*ndigits);
	
	for(i=0;i<ndigits;i++){
		/* t = w_(2i) + (x_i)^2 */
		spMultiply(t, u[i], u[i]);
		mpShortAdd(t, t, w[2*i], 2); 
		
		w[2*i] = t[0];
		C1 = t[1];
		C2 = 0;
		
		for(j=i+1;j<ndigits;j++){
			/* prod = x_i * x_j */
			spMultiply(prod, u[i], u[j]);
			
			/* t = w_(i+j) + prod + C1 */
			mpShortAdd(t, prod, C1, 2);
			mpShortAdd(t, t, w[i+j], 2);
			
			w[i+j] = t[0];
			C1 = t[1];
			
			/* t = w_(i+j) + prod + C2 */
			mpShortAdd(t, prod, C2, 2);
			mpShortAdd(t, t, w[i+j], 2);
			
			w[i+j] = t[0];
			C2 = t[1];
		}
		/* t = C1 + C2 */
		mpSetZero(t, 2);
		mpShortAdd(t, t, C1, 2);
		mpShortAdd(t, t, C2, 2);
		
		C1 = t[0];
		C2 = t[1];
		
		/* t = w_(i+ndigits) + C1 */
		mpSetZero(t, 2);
		mpShortAdd(t, t, w[i+ndigits], 2);
		mpShortAdd(t, t, C1, 2);
		
		w[i+ndigits] = t[0];
		
		w[i+ndigits+1] = C2 + t[1];
	}
	
	return 0;
}
