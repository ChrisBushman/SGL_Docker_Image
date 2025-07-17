/*-------------------------------------------------------------------------*/
/* SH SERIES C Compiler Ver. 1.0                                           */
/* Copyright (c) 1992 Hitachi,Ltd.,Hitachi Software Engineering Co.,Ltd.   */
/* Licensed material of Hitachi,Ltd.,Hitachi Software Engineering Co.,Ltd. */
/*-------------------------------------------------------------------------*/
/*****************************************************************/
/* SPEC ;                                                        */
/*   NAME = stdarg :                                             */
/*   FUNC = this module do the following finctions ;             */
/*                                                               */
/*         (1) initialize parameter pointer                      */
/*         (2) set parameter pointer to next parameter           */
/*                                                               */
/*                                                               */
/*   END ;                                                       */
/*****************************************************************/

typedef char *va_list ;

#define va_start(ap,param) (void)((ap)=(int)\
((char *) &(param)+sizeof(param))%4u?\
(char *) &(param)+sizeof(param)+(4u-(sizeof(param)%4u)):\
(char *) &(param)+sizeof(param))
					       /* C T20066V	      */
#define va_arg(ap,type) (*((ap)=((int)((ap)+sizeof(type))%4u?   /*C T20029S */\
(ap)+sizeof(type)+(4u-(sizeof(type)%4u)):\
(ap)+sizeof(type)),\
(type *)((int)((ap)-sizeof(type))%4u?\
(ap)-sizeof(type)-(4u-(sizeof(type)%4u)):\
(ap)-sizeof(type))))						/* C T20029S */
					       /* C T20066V	      */
#define va_end(ap)

