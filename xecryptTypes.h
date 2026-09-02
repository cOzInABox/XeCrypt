#ifndef _XECRYPTTYPES_H
#define _XECRYPTTYPES_H

#ifdef _MSC_VER // lets use swap intrinsics on visual studio
// #pragma message("using visual studio intrinsics")


	#ifndef bswap16
		#define bswap16	_byteswap_ushort
	#endif

	#ifndef bswap32
		#define bswap32 _byteswap_ulong
	#endif

	#ifndef bswap64
		#define bswap64 _byteswap_uint64
	#endif

#else // go old fashioned for everything else
	#define _fseeki64 fseeko64
	#define _ftelli64 ftello64
	// #define u16Rev(x) (((x&0xFF)<<8)+(((x&0xFF00)>>8)))
	#ifndef bswap16
		#define bswap16(x) (((x&0xFF)<<8)+(((x&0xFF00)>>8)))
	#endif

	// #define u32Rev(x) ((((x&0xFF)<<24))+(((x&0xFF00)<<8))+(((x&0xFF0000)>>8))+(((x&0xFF000000)>>24)))
	#ifndef bswap32
		#define bswap32(x) ((((x&0xFF)<<24))+(((x&0xFF00)<<8))+(((x&0xFF0000)>>8))+(((x&0xFF000000)>>24)))
	#endif

	// #define u64Rev(x) (((x&0xFF)<<56)+((x&0xFF00)<<40)+((x&0xFF0000)<<24)+((x&0xFF000000)<<8)+((x>>8)&0xFF000000)+((x>>24)&0xFF0000)+((x>>40)&0xFF00)+((x>>56)&0xFF))
	#ifndef bswap64
		#define bswap64(x) (((x&0xFF)<<56)+((x&0xFF00)<<40)+((x&0xFF0000)<<24)+((x&0xFF000000)<<8)+((x>>8)&0xFF000000)+((x>>24)&0xFF0000)+((x>>40)&0xFF00)+((x>>56)&0xFF))
	#endif

#endif // _MSC_VER

typedef unsigned char		u8, BYTE, *PBYTE;
typedef unsigned short		u16, WORD, *PWORD;
typedef unsigned int		u32;
typedef unsigned long long	u64, QWORD, *PQWORD;
typedef char				s8;
typedef short				s16;
typedef int					s32;
typedef long long			s64;
typedef int                 BOOL;

#ifndef TRUE
	#define TRUE 1
#endif

#ifndef FALSE
	#define FALSE 0
#endif

#ifndef MAXPATHLEN
#define MAXPATHLEN 1024
#endif

#endif // _XECRYPTTYPES_H
