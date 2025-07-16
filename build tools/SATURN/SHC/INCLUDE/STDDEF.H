/*-------------------------------------------------------------------------*/
/* SH SERIES C Compiler Ver. 1.0                                           */
/* Copyright (c) 1992 Hitachi,Ltd.,Hitachi Software Engineering Co.,Ltd.   */
/* Licensed material of Hitachi,Ltd.,Hitachi Software Engineering Co.,Ltd. */
/*-------------------------------------------------------------------------*/
/***********************************************************************/
/* SPEC;                                                               */
/*  NAME = stddef :                                                    */
/*                                                                     */
/*  FUNC = this module do the following functions                      */
/*                                                                     */
/*  CLAS = UNIT;                                                       */
/*                                                                     */
/* END;                                                                */
/***********************************************************************/

                                                         /* D S01006SP */
#ifndef  ptrdiff_t
#define  ptrdiff_t      int
#endif

#ifndef  size_t
#define  size_t         unsigned long
#endif

#ifndef  NULL
#define  NULL           ((void *)0)
#endif

#ifndef  offsetof
#define  offsetof(type,id) ((char *)(&(((type *)NULL)->id))  -    \
                            (char *)((type *)NULL))
#endif

#ifndef  errno
#define  errno          _errno
#endif
