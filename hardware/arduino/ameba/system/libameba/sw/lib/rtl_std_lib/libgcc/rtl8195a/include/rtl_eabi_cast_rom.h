/*
 * rtl_string.h
 *
 * Definitions for memory and string functions.
 */
#ifndef _RTL_EABI_CAST_ROM_H_
#define	_RTL_EABI_CAST_ROM_H_


#include "rtl_math_types.h"

#define __rtl_itod 		__rtl_itod_v1_00
#define __rtl_itof		__rtl_itof_v1_00
#define __rtl_dtoi 		__rtl_dtoi_v1_00
#define __rtl_dtoui 	__rtl_dtoui_v1_00
#define __rtl_dtoi64 	__rtl_dtoi64_v1_00

#define __rtl_uitof 	__rtl_uitof_v1_00
#define __rtl_uitod 	__rtl_uitod_v1_00

#define __rtl_ftol 		__rtl_ftol_v1_00
#define __rtl_ftod 		__rtl_ftod_v1_00
#define __rtl_dtof 		__rtl_dtof_v1_00

//#define __rtl_dtoa 		__rtl_dtoa_v1_00
#define __rtl_ltoa 		__rtl_ltoa_v1_00
#define __rtl_ultoa 	__rtl_ultoa_v1_00

#define __rtl_fadd 		__rtl_fadd_v1_00
#define __rtl_fsub 		__rtl_fsub_v1_00
#define __rtl_fmul 		__rtl_fmul_v1_00
#define __rtl_fdiv 		__rtl_fdiv_v1_00
#define __rtl_fsqrt 	__rtl_fsqrt_v1_00

#define __rtl_dadd 		__rtl_dadd_v1_00
#define __rtl_dsub 		__rtl_dsub_v1_00
#define __rtl_dmul 		__rtl_dmul_v1_00
#define __rtl_ddiv 		__rtl_ddiv_v1_00

#define __rtl_dcmpeq 	__rtl_dcmpeq_v1_00
#define __rtl_dcmplt 	__rtl_dcmplt_v1_00
#define __rtl_dcmpgt 	__rtl_dcmpgt_v1_00
#define __rtl_dcmple 	__rtl_dcmple_v1_00

#define __rtl_fcmplt 	__rtl_fcmplt_v1_00
#define __rtl_fcmpgt 	__rtl_fcmpgt_v1_00


extern _LONG_CALL_ char* __rtl_ltoa_v1_00( long value, char *string, int radix );
extern _LONG_CALL_ char* __rtl_ultoa_v1_00( unsigned long value, char *string, int radix );

//extern _LONG_CALL_ char *__rtl_dtoa_v1_00(char *a, double d, int digits);


extern _LONG_CALL_ int 	__rtl_dtoi_v1_00(double d);
extern _LONG_CALL_ int64 __rtl_dtoi64_v1_00(double d);
extern _LONG_CALL_ unsigned int __rtl_dtoui_v1_00(double d);



extern _LONG_CALL_ long __rtl_ftol_v1_00(float f);


extern _LONG_CALL_ float __rtl_itof_v1_00(int32 lval);
extern _LONG_CALL_ double __rtl_itod_v1_00(int lval);
extern _LONG_CALL_ double __rtl_i64tod_v1_00(int64 lval);

extern _LONG_CALL_ float __rtl_uitof_v1_00(unsigned int lval);
extern _LONG_CALL_ double __rtl_uitod_v1_00(unsigned int lval);

extern _LONG_CALL_ double __rtl_ftod_v1_00(float f);

extern _LONG_CALL_ float __rtl_dtof_v1_00(double d);

extern _LONG_CALL_ float __rtl_fadd_v1_00(float a, float b);
extern _LONG_CALL_ float __rtl_fsub_v1_00(float a, float b);
extern _LONG_CALL_ float __rtl_fmul_v1_00(float a, float b);
extern _LONG_CALL_ float __rtl_fdiv_v1_00(float a, float b);
extern _LONG_CALL_ float __rtl_fsqrt_v1_00(float a);



extern _LONG_CALL_ double __rtl_dadd_v1_00(double a, double b);
extern _LONG_CALL_ double __rtl_dsub_v1_00(double a, double b);
extern _LONG_CALL_ double __rtl_dmul_v1_00(double a, double b);
extern _LONG_CALL_ double __rtl_ddiv_v1_00(double a, double b);

extern _LONG_CALL_ int 	  __rtl_dcmpeq_v1_00(double a, double b);
extern _LONG_CALL_ int 	  __rtl_dcmpge_v1_00(double a, double b);
extern _LONG_CALL_ int 	  __rtl_dcmplt_v1_00(double a, double b);
extern _LONG_CALL_ int 	  __rtl_dcmpgt_v1_00(double a, double b);
extern _LONG_CALL_ int 	  __rtl_dcmple_v1_00(double a, double b);

extern _LONG_CALL_ int 	  __rtl_fcmplt_v1_00(float a, float b);
extern _LONG_CALL_ int 	  __rtl_fcmpgt_v1_00(float a, float b);


#endif /* _RTL_EABI_CAST_ROM_H_ */
