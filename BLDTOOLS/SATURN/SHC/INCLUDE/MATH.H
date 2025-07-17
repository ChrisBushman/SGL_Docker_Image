/*-------------------------------------------------------------------------*/
/* SH SERIES C Compiler Ver. 1.0                                           */
/* Copyright (c) 1992 Hitachi,Ltd.,Hitachi Software Engineering Co.,Ltd.   */
/* Licensed material of Hitachi,Ltd.,Hitachi Software Engineering Co.,Ltd. */
/*-------------------------------------------------------------------------*/
/*****************************************************************/
/* SPEC ;                                                        */
/*   NAME = math :                                               */
/*   FUNC =                                                      */
/*          ;                                                    */
/*                                                               */
/*                                                               */
/*                                                               */
/*   CLAS = UNIT ;                                               */
/*   END ;                                                       */
/*****************************************************************/
                                                   /* D S01006SP */
#ifndef ERANGE
#define ERANGE  1100
#endif

#ifndef EDOM
#define EDOM    1101
#endif

#ifndef ENUM
#define ENUM    1208
#endif

extern volatile int _errno;                        /* C S01002SP */
extern const double _HUGE_VAL;                     /* C S01006SP */
#define HUGE_VAL  _HUGE_VAL                        /* C S01006SP */

extern double frexp(double, int *);
extern double ldexp(double, int );
extern double modf(double, double *);
extern double ceil(double);
extern double fabs(double);
extern double floor(double);
extern double fmod(double, double);

extern double acos(double);
extern double asin(double);
extern double atan(double);
extern double atan2(double,double);
extern double cos(double);
extern double sin(double);
extern double tan(double);
extern double cosh(double);
extern double tanh(double);
extern double sinh(double);
extern double exp(double);
extern double log(double);
extern double log10(double);
extern double pow(double,double);
extern double sqrt(double);

                                                   /* D S01006SP */
