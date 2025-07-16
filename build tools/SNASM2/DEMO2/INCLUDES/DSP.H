/*----------------------------------------------------------------------------
 *----------------------------------------------------------------------------
 */

/*
 * GLOBAL DEFINES/MACROS DEFINES
 */

#define  DSP_REG_BASE        0x25FE0080 /* DSP Register base address        */

#define  DSP_RW_CTRL         0x00000000 /* DSP Control Port                 */
#define  DSP_W_PDAT          0x00000004 /* DSP Program RAM Data Port        */
#define  DSP_W_DADR          0x00000008 /* DSP Data RAM Address Port        */
#define  DSP_RW_DDAT         0x0000000c /* DSP Data RAM Data Port           */

#define  DSP_RAM_0           0x00       /* DSP RAM 0 Address                */
#define  DSP_RAM_1           0x40       /* DSP RAM 1 Address                */
#define  DSP_RAM_2           0x80       /* DSP RAM 2 Address                */
#define  DSP_RAM_3           0xc0       /* DSP RAM 3 Address                */

#define  DSP_END             1          /* DSP Process End Code             */
#define  DSP_NOT_END         0          /* DSP Process Not End Code         */

#define DSP_WRITE_REG(reg, val) \
      *(Uint32*)(DSP_REG_BASE+(reg)) = (val)
#define DSP_READ_REG(reg) \
      (*(Uint32*)(DSP_REG_BASE+(reg)))

/*
 * EXTERNAL FUNCTION PROTOTYPE DECLARATIONS
 */
void  DSP_LoadProgram(Uint8 dst, Uint32 *src, Uint16 count);
void  DSP_WriteData(Uint8 dst, Uint32 *src, Uint16 count);
void  DSP_ReadData(Uint32 *dst, Uint8 src, Uint16 count);
void  DSP_Start(Uint8 pc);
void  DSP_Stop(void);
Uint8 DSP_CheckEnd(void);


/*
 * データタイプ定義
 */

typedef Sint32           Fixed32;    /* Fixed-point representation           */
				     /* Radix point is between bit 15 & 16   */

typedef struct  MthXy {              /* 2 Dimension Point Data               */
	 Fixed32         x;
	 Fixed32         y;
} MthXy;

typedef struct  MthXyz {             /* 3 Dimension Point Data               */
	 Fixed32         x;
	 Fixed32         y;
	 Fixed32         z;
} MthXyz;

typedef struct  MthMatrix {          /* 3 Dimension Matrix Data              */
	 Fixed32         val[3][4];
} MthMatrix;

typedef struct  MthMatrixTbl {       /* Matrix Stack Management Table        */
	 Uint16          stackSize;  /* Number of Matrix Stack Entry         */
	 MthMatrix       *current;   /* Current Matrix Pointer               */
	 MthMatrix       *stack;     /* Matrix Stack Area                    */
} MthMatrixTbl;

typedef struct  MthViewLight {       /* Coord Tran Matrix and Light Parm     */
	 MthMatrix     viewMatrix;   /* View Coord Transfer Matrix           */
	 MthXyz        lightVector;  /* Light Vector in View Coord           */
	 MthMatrix     worldMatrix;  /* World Coord Transfer Matrix          */
} MthViewLight;

typedef struct  MthPolyTransParm {   	/* Polygon Data Coord Transfer Parm    */
	 MthViewLight  *viewLight;   			/* Coord Tran Matrix and Light Parm    */
	 Uint32        surfCount;    			/* Number of Polygon Surface           */
	 MthXyz        *surfPoint;   			/* Surface Point for Compute Bright    */
	 MthXyz        *surfNormal;  			/* Surface Normal Vector               */
	 Sint32        *surfBright;  			/* Result of Surface Compute Bright    */
	 Uint32        transViewVertCount;  /* Number of Vert for View Tran  		*/
	 MthXyz        *transViewVertSrc;   /* Source Vert for View Tran     		*/
	 MthXyz        *transViewVertAns;   /* Answer Vert for View Tran     		*/
	 Uint32        gourVertCount;			/* Number of Vert for Compute Bright   */
	 MthXyz        *vertNormal;  			/* Vertices Bright Normal Vector       */
	 Sint32        *vertBright;  			/* Result of Vertices Compute Bright   */
	 Uint32        transWorldVertCount; /* Number of Vert for World Tran 		*/
	 MthXyz        *transWorldVertSrc;  /* Source Vert for World Tran    		*/
	 MthXyz        *transWorldVertAns;  /* Answer Vert for World Tran    		*/
} MthPolyTransParm;


