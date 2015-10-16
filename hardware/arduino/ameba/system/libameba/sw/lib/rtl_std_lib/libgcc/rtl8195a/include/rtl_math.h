/*
 * rtl_math.h
 *
 * Definitions for mathematics functions.
 */

#ifndef _RTL_MATH_H_
#define	_RTL_MATH_H_

#include <basic_types.h>

extern double rtl_fabs(double);
extern float rtl_fabsf(float a);


extern float rtl_cos_f32(float a);
extern float rtl_sin_f32(float a);

extern void rtl_mult_f32_array(
		float * pSrcA, float * pSrcB,
		float * pDst, u32 blockSize);



#endif /* _RTL_MATH_H_ */
