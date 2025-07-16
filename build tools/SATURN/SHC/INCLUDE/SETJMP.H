/*-------------------------------------------------------------------------*/
/* SH SERIES C Compiler Ver. 1.0                                           */
/* Copyright (c) 1992 Hitachi,Ltd.,Hitachi Software Engineering Co.,Ltd.   */
/* Licensed material of Hitachi,Ltd.,Hitachi Software Engineering Co.,Ltd. */
/*-------------------------------------------------------------------------*/
/***********************************************************************/
/* SPEC;                                                               */
/*  NAME = setjmp.h :                                ;                 */
/*                                                                     */
/*  FUNC = this module do the following functions    ;                 */
/*                                                                     */
/*  CLAS = UNIT;                                                       */
/*                                                                     */
/* END;                                                                */
/***********************************************************************/
                                                         /* D S01001SP */
typedef int jmp_buf[65];

extern int setjmp(jmp_buf);
extern void longjmp(jmp_buf, int);
extern volatile int _errno;                              /* C S01002SP */

#ifndef SEQERR
#define SEQERR     1108
#endif
