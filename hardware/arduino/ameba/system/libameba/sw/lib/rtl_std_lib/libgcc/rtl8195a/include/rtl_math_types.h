/*
 * rtl_math_types.h
 *
 * Definitions for basic types / macro functions
 */
 
#ifndef _RTL_MATH_TYPES_H_
#define	_RTL_MATH_TYPES_H_


#include <basic_types.h>
#include <diag.h>


typedef unsigned long long  	uint64;
typedef long long       		int64;
typedef unsigned int    		uint32;
typedef int         			int32;
typedef unsigned short  		uint16;
typedef short           		int16;
typedef unsigned char   		uint8;
typedef char            		int8;


typedef char flag;
typedef uint32 float32;
typedef uint64 double64;

typedef unsigned char bits8;
typedef signed char sbits8;
typedef unsigned short int bits16;
typedef signed short int sbits16;
typedef unsigned int bits32;
typedef signed int sbits32;
typedef uint64 	bits64;
typedef int64   sbits64;


// 
// float
//

#undef LIT32
#define LIT32( a ) a##L


#undef float32_default_nan
#define float32_default_nan LIT32( 0x7FC00000 )

#define float32_zero 		LIT32(0)
#define float32_one 		LIT32(0x3f800000)
#define float32_ln2 		LIT32(0x3f317218)
#define float32_pi 			LIT32(0x40490fdb)
#define float32_half 		LIT32(0x3f000000)
#define float32_infinity 	LIT32(0x7f800000)



#undef LIT64
#define LIT64( a ) a##LL


#undef Double64_default_NAN
#define Double64_default_NAN LIT64( 0x7FF8000000000000 )


#define double64_zero 		LIT64(0)
#define double64_one 		LIT64(0x3ff0000000000000)
#define double64_ln2 		LIT64(0x3fe62e42fefa39ef)
#define double64_pi 		LIT64(0x400921fb54442d18)
#define double64_half 		LIT64(0x3fe0000000000000)
#define double64_infinity 	LIT64(0x7ff0000000000000)



//
// other macro, will be modified /taken off in the future. 
//

#undef printk
#define printk DiagPrintf


#undef memDump
#define memDump rtl_memDump


#undef dbg_printk
#define dbg_printk(fmt, args...) \
	     	printk("%s():%d : " fmt "\n", __FUNCTION__, __LINE__, ##args); 

#undef dbg_mem_dump
#define dbg_mem_dump(start, size, str_header) \
	  printk("%s():%d : memdump : address: %08x, size: %d\n", __FUNCTION__, __LINE__, start, size); \
	  memDump((const u8*)start, size, (char*)str_header); \



#endif /* _RTL_MATH_TYPES_H_ */
