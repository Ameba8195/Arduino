/*
 * rtl_math_types.h
 *
 * Definitions for basic types / macro functions
 */
 
#ifndef _RTL_MATH_DIAG_H_
#define	_RTL_MATH_DIAG_H_

#include <basic_types.h>
#include <diag.h>



//
// other macro, will be modified /taken off in the future. 
//

#undef printk
#define printk DiagPrintf


extern _LONG_CALL_ void __rtl_memDump_v1_00(const u8 *start, u32 size, char * strHeader);

#undef memDump
#define memDump __rtl_memDump_v1_00


#undef dbg_printk
#define dbg_printk(fmt, args...) \
	     	printk("%s():%d : " fmt "\n", __FUNCTION__, __LINE__, ##args); 

#undef dbg_mem_dump
#define dbg_mem_dump(start, size, str_header) \
	  printk("%s():%d : memdump : address: %08x, size: %d\n", __FUNCTION__, __LINE__, start, size); \
	  memDump((const u8*)start, size, (char*)str_header); \



#endif /* _RTL_MATH_DIAG_H_ */
