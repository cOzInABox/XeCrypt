/* common functions for libmcrypto */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#ifdef __linux__
	/* linux-specific include files */
	#include <unistd.h>
	#include <sys/types.h>
	#include <sys/stat.h>
#elif defined(window)
	/* window-specific include files */
#endif 

#include "mcrypto.h"

int prng(BYTE* buf, int buf_size)
{
#if LINUX_URANDOM	    	
	int fd;
	
		if((fd = open("/dev/urandom", O_RDONLY)) < 0) {
			perror("/dev/urandom");
			return -1;
		}
		read(fd, buf, buf_size);
		close(fd);
#else
	/* other platforms support here - for now fill buf with zeros :) */
	memset(buf, 0x00, buf_size);
#endif		
	return 0;
}

/* Useful Function for Debug */
void mcrypto_msg(const char *s)
{
#ifdef MCRYPTO_DEBUG
	fprintf(stderr, "debugging message - file %s line %d : %s", __FILE__, __LINE__, s);
#endif
}

void mcrypto_dump(char *desc, BYTE *p, UINT len)
{
 #ifdef MCRYPTO_DEBUG
	UINT i = 0;
	
	printf("\n[%s]\n", desc);
	while (len--) {
		if ((i % 16) == 0 && i)
			printf("\n");
		fprintf(stderr, "%02x ", p[len]);
		i++;
	}
	fprintf(stderr, "\n");
#endif
}
