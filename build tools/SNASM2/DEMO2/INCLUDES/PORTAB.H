/* portability macros for porting code to other systems */


#define	TRUE	1
#define	FALSE	0


typedef unsigned    char    Uint8;      /* 符号なし１バイト整数 */
typedef signed      char    Sint8;      /* 符号つき１バイト整数 */
typedef unsigned    short   Uint16;     /* 符号なし２バイト整数 */
typedef signed      short   Sint16;     /* 符号つき２バイト整数 */
typedef unsigned    long    Uint32;     /* 符号なし４バイト整数 */
typedef signed      long    Sint32;     /* 符号つき４バイト整数 */
typedef float               Float32;    /* ４バイト実数 */
typedef double              Float64;    /* ８バイト実数 */

typedef int                 Int;        /* INT型（ツール用）            */

typedef int                 Bool;       /* 論理型（論理定数を値にとる） */

/*****************************************************************************/
/********************************* 定数マクロ ********************************/
/*****************************************************************************/

#ifndef NULL
#define NULL    ((void *)0)             /* NULL */
#endif  /* NULL */

