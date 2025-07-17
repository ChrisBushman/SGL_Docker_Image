/*-------------------------------------------------------------------------*/
/* SH SERIES C Compiler Ver. 1.0                                           */
/* Copyright (c) 1992 Hitachi,Ltd.,Hitachi Software Engineering Co.,Ltd.   */
/* Licensed material of Hitachi,Ltd.,Hitachi Software Engineering Co.,Ltd. */
/*-------------------------------------------------------------------------*/
/*****************************************************************/
/* SPEC ;                                                        */
/*   NAME = float :                                              */
/*   FUNC = limit about float type  and double type ;            */
/*                                                               */
/*                                                               */
/*   END ;                                                       */
/*****************************************************************/
                                                   /* C S01006SP */

extern const float       _FLT_MAX_VAL;
extern const float       _FLT_MIN_VAL;
extern const double      _DBL_MAX_VAL;
extern const double      _DBL_MIN_VAL;
extern const long double _LDBL_MAX_VAL;
extern const long double _LDBL_MIN_VAL;

#define FLT_RADIX             2
#define FLT_ROUNDS            1
#define FLT_GUARD             1
#define FLT_NORMALIZE         1
#define FLT_MAX _FLT_MAX_VAL
#define FLT_MIN _FLT_MIN_VAL
#define FLT_MAX_EXP         127
#define FLT_MIN_EXP       (-149)
#define FLT_MAX_10_EXP       38
#define FLT_MIN_10_EXP     (-44)
#define FLT_DIG               6
#define FLT_MANT_DIG         24
#define FLT_EXP_DIG           8
#define FLT_POS_EPS 5.9604648328104311e-8f
#define FLT_NEG_EPS 2.9802324164052156e-8f
#define FLT_POS_EPS_EXP    (-23)
#define FLT_NEG_EPS_EXP    (-24)

#define DBL_RADIX             2
#define DBL_ROUNDS            1
#define DBL_GUARD             1
#define DBL_NORMALIZE         1
#define DBL_MAX _DBL_MAX_VAL
#define DBL_MIN _DBL_MIN_VAL
#define DBL_MAX_EXP        1023
#define DBL_MIN_EXP      (-1074)
#define DBL_MAX_10_EXP      308
#define DBL_MIN_10_EXP    (-323)
#define DBL_DIG              15
#define DBL_MANT_DIG         53
#define DBL_EXP_DIG          11
#define DBL_POS_EPS 1.1102230246251567e-16
#define DBL_NEG_EPS 5.5511151231257834e-17
#define DBL_POS_EPS_EXP    (-52)
#define DBL_NEG_EPS_EXP    (-53)

#define LDBL_RADIX             2
#define LDBL_ROUNDS            1
#define LDBL_GUARD             1
#define LDBL_NORMALIZE         1
#define LDBL_MAX _LDBL_MAX_VAL
#define LDBL_MIN _LDBL_MIN_VAL
#define LDBL_MAX_EXP     DBL_MAX_EXP
#define LDBL_MIN_EXP     DBL_MIN_EXP
#define LDBL_MAX_10_EXP  DBL_MAX_10_EXP
#define LDBL_MIN_10_EXP  DBL_MIN_10_EXP
#define LDBL_DIG         DBL_DIG
#define LDBL_MANT_DIG    DBL_MANT_DIG
#define LDBL_EXP_DIG     DBL_EXP_DIG
#define LDBL_POS_EPS     DBL_POS_EPS
#define LDBL_NEG_EPS     DBL_NEG_EPS
#define LDBL_POS_EPS_EXP DBL_POS_EPS_EXP
#define LDBL_NEG_EPS_EXP DBL_NEG_EPS_EXP
