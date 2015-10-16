/*
 * rtl_lib.h
 *
 * Definitions for RTL library functions
 */
 
#ifndef _RTL_LIB_H_
#define	_RTL_LIB_H_


#ifdef __cplusplus
extern "C" {
#endif


#include <basic_types.h>
#include <diag.h>


extern int __rtl_errno;


extern void rtl_libc_init(void);


//
// RTL library functions for Libc::stdio
// 

extern int rtl_printf(IN const char* fmt, ...);


//
// RTL library functions for string
// 

extern void * rtl_memset(void * m , int c , size_t n);
extern void * rtl_memchr(const void * src_void , int c , size_t length);

extern void * rtl_memmove( void * dst_void , const void * src_void , size_t length);

extern int rtl_strcmp(const char *s1 ,	const char *s2);


//
// RTL library functions for math
// 


extern double rtl_fabs(double);
extern float rtl_fabsf(float a);
extern float rtl_cos_f32(float a);
extern float rtl_sin_f32(float a);

extern float rtl_fadd(float a, float b);
extern float rtl_fsub(float a, float b);
extern float rtl_fmul(float a, float b);
extern float rtl_fdiv(float a, float b);

extern int rtl_fcmplt(float a, float b);
extern int rtl_fcmpgt(float a, float b);





//
// RTL eabi functions 

extern double rtl_ftod(float f);

extern double rtl_ddiv(double a, double b);


//
// Macro Library Functions
//

typedef union
{
  float value;
  u32 	word;
} ieee_float_shape_type;

/* Get a 32 bit int from a float.  */

#define GET_FLOAT_WORD(i,d)					\
do {								\
  ieee_float_shape_type gf_u;					\
  gf_u.value = (d);						\
  (i) = gf_u.word;						\
} while (0)

/* Set a float from a 32 bit int.  */

#define SET_FLOAT_WORD(d,i)					\
do {								\
  ieee_float_shape_type sf_u;					\
  sf_u.word = (i);						\
  (d) = sf_u.value;						\
} while (0)

static inline
float rtl_nanf(void)
{
	float x;

	SET_FLOAT_WORD(x,0x7fc00000);
	return x;
}


//
// Library Test functions
//

extern int rtl_lib_test(IN u16 argc, IN u8 *argv[]);
extern int rtl_math_test(IN u16 argc, IN u8 *argv[]);
extern int rtl_string_test(IN u16 argc, IN u8 *argv[]);


//
// Macro functions
//

#ifdef __cplusplus
}
#endif

#endif /* _RTL_LIB_H_ */
