/* mpSetZero.c */

#include "bigdigits.h"

void mpSetZero(DIGIT_T a[], UINT ndigits)
{	
	/* Sets a = 0 */
	 while(ndigits--)	
		 a[ndigits] = 0;
	
}
