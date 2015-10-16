/*
 * rtl_math.h
 *
 * Definitions for mathematics functions.
 */

#ifndef _RTL_EABI_CAST_RAM_H_
#define	_RTL_EABI_CAST_RAM_H_






//
// aeabi.h
//
// ftp://ftp.netbsd.org/pub/NetBSD/NetBSD-current/src/sys/arch/arm/include/aeabi.h
//

extern float __aeabi_i2f(int val);
extern double __aeabi_i2d(int val);
extern double __aeabi_ui2d(unsigned int val);


extern int __aeabi_d2iz(double d);
extern unsigned int __aeabi_d2uiz(double d);

extern int __aeabi_f2iz(float f);

extern double __aeabi_f2d(float f);

extern float __aeabi_d2f(double);

extern double __aeabi_dadd(double, double);
extern double __aeabi_dsub(double, double);
extern double __aeabi_dmul(double, double);

extern int __aeabi_dcmpeq(double, double);
extern int __aeabi_dcmplt(double, double);
extern int __aeabi_dcmpgt(double, double);

extern int __aeabi_fcmplt(float, float);




//
// rtl math functions
//

extern int rtl_dtoi(double d);
extern unsigned int rtl_dtoui(double d);

extern float rtl_i2f(int val);
extern double rtl_i2d(int val);
extern double rtl_ui2d(unsigned int val);


extern char* rtl_itoa( int value, char *string, int radix );
extern char* rtl_ltoa( long value, char *string, int radix );
extern char* rtl_utoa( unsigned long value, char *string, int radix );
extern char* rtl_ultoa( unsigned long value, char *string, int radix );
extern char* rtl_ftoa(char *a, float f, int digits);
//extern char* rtl_dtoa(char *a, double d, int digits);
extern double rtl_ftod(float f);
extern float rtl_dtof(double d);

extern float rtl_fadd(float a, float b);
extern float rtl_fsub(float a, float b);
extern float rtl_fmul(float a, float b);
extern float rtl_fdiv(float a, float b);
extern float rtl_fsqrt(float a);



extern double rtl_dadd(double a, double b);
extern double rtl_dsub(double a, double b);
extern double rtl_dmul(double a, double b);


extern int rtl_dcmpeq(double a, double b);
extern int rtl_dcmplt(double a, double b);
extern int rtl_dcmpgt(double a, double b);

extern int rtl_fcmplt(float a, float b);


#endif /* _RTL_EABI_CAST_RAM_H_ */

