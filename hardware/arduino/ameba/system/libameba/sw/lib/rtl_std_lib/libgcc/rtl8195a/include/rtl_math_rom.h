/*
 * rtl_math_rom.h
 *
 * Definitions for math functions in ROM.
 */
#ifndef _RTL_MATH_ROM_H_
#define	_RTL_MATH_ROM_H_

#include "rtl_math_types.h"


#define __rtl_fabs				__rtl_fabs_v1_00
#define __rtl_fabsf				__rtl_fabsf_v1_00
#define __rtl_sin_f32			__rtl_sin_f32_v1_00
#define __rtl_cos_f32			__rtl_cos_f32_v1_00
#define __rtl_mult_f32_array  	__rtl_mult_f32_array_v1_00


extern _LONG_CALL_ float __rtl_fabsf_v1_00(float);
extern _LONG_CALL_ double __rtl_fabs_v1_00(double);

extern _LONG_CALL_ float __rtl_cos_f32_v1_00(float x);
extern _LONG_CALL_ float __rtl_sin_f32_v1_00(float x);

extern _LONG_CALL_ void __rtl_mult_f32_array_v1_00(
							float * pSrcA, float * pSrcB,
							float * pDst, u32 blockSize);



#endif /* _RTL_MATH_ROM_H_ */
