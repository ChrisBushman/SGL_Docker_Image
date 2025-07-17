/*********************************************************************/
/*																	 */
/* PROGRAM				: Common Data Definition					 */
/* SYSTEM				: VER 1.00									 */
/* FILE 				: CMNDEF.h									 */
/* VERSION				: 1.00										 */
/* AUTHOR				: CASIO COMPUTER CO.,LTD					 */
/* DATA OF GET			: yy/mm/dd									 */
/* DATA OF UPDATE		: 95/ 5/23									 */
/* FUNCTION 			:											 */
/* HISTORY				: 95/ 5/23	<< Communication utility >> 	 */
/*									Addional AP file error(CU_ERR_AP)*/
/*									at Error information & Error	 */
/*									status. 						 */
/*						  95/07/07	Remove find data for search 	 */
/*									function.						 */
/*																	 */
/* Copyright(C) 1995 CASIO COMPUTER CO.,LTD. All rights reserved.	 */
/*********************************************************************/


/*********************************************************************/
/*				Common data 							  #000		 */
/*				Data control							  #002		 */
/*				Display BIOS							  #003		 */
/*				Key BIOS								  #004		 */
/*				Communication BIOS						  #005		 */
/*				Power BIOS								  #006		 */
/*				OBR BIOS								  #007		 */
/*				Timer BIOS								  #008		 */
/*				Communication utility					  #031		 */
/*********************************************************************/

#define _SH7020_			1			/* 1 = SH7020 / 0 = SH7032	 */

/*********************************************************************/
/*				Common data 							  #000		 */
/*********************************************************************/
/*------------- Symbol Definition -----------------------------------*/
/*============= Task ID =============================================*/
#define TK_SYS_M			0x0001		/* System menu task 		 */
#define TK_ENV_M			0x0002		/* Environment menu task	 */
#define TK_MNTR 			0x0003		/* System superviser task	 */
#define TK_IDOL 			0x0004		/* Idoling task 			 */
#define TK_AP_MAIN			0x0005		/* Application task (main)	 */
#define TK_AP_MAX			0x0040		/* Application task (max)	 */
#define TK_AP_MIN			0x0015		/* Application task (min)	 */

/*============= Task priority =======================================*/
#define TK_SYS_M_PRI		0x000a		/* System memu task 		 */
#define TK_ENV_M_PRI		0x000c		/* Environment menu task	 */
#define TK_MNTR_PRI 		0x0008		/* System superviser task	 */
#define TK_IDOL_PRI 		0x0040		/* Idoling task 			 */
#define TK_AP_MAIN_PRI		0x0015		/* Application task (main)	 */

/*============= Event flag ID =======================================*/
#define FL_SYS_M			0x0001		/* Sytem menu event 	 */
#define FL_SYS_M_STR		0x00000001	/*	 Execution			 */
#define FL_ENV_M			0x0002		/* Environment menu 		 */
#define FL_ENV_M_SYS		0x00000001	/*	 Execution (From system menu)*/
#define FL_ENV_M_AP 		0x00000002	/*	 Execution (From application)*/
#define FL_MNTR 			0x0003		/* System supervise event	 */
#define FL_MNTR_STR 		0x00000001	/*	 Periodic start 		 */
#define FL_MNTR_APO 		0x00000002	/*	 Replace APO time		 */
#define FL_MNTR_ABO 		0x00000004	/*	 Replace ABO time		 */
#define FL_MNTR_LB1 		0x00000008	/*	 LB1 occur				 */
#define FL_MNTR_LB2 		0x00000010	/*	 LB2 occur				 */
#define FL_MNTR_PWR 		0x00010000	/*	 wait for completion power supply load */
#define FL_MNTR_STP 		0x80000000	/*	 Disable periodic start  */
#define FL_AP_MAIN			0x0004		/* Application event		 */
#define FL_AP_MAIN_STR		0x00000001	/*	 Execution				 */
#define FL_SYS_ERR			0x0005		/* Watching system abnormaly event */
#define FL_SYS_ERR_FIL		0x00000001	/*	 Unfair file			 */
#define FL_SYS_ERR_CLS		0x00000002	/*	 Unclose file			 */
#define FL_SYS_ERR_SUM		0x00000003	/*	 Sum check error		 */
#define FL_SYS_ERR_DAT		0x00000100	/*	 Out of system data range*/
#define FL_SYS_ERR_RCT		0x00010000	/*	 Abnormaly RTC			 */
#define FL_SYS_WRN			0x0006		/* Warning event			 */
#define FL_SYS_WRN_HW		0x00000001	/*	 Start up by H/W reset	 */
#define FL_SYS_WRN_CLR		0x00000002	/*	 Start up by broken memory*/
#define FL_SYS_WRN_LB		0x00000004	/*	 Start up by LB1 occur	 */
#define FL_SYS_WRN_MRY		0x00010000	/*	 Memory shortage		 */
#define FL_SYS_WRN_FIL		0x00020000	/*	 File volume shortage	 */
#define FL_INV_SLP			0x0007		/* Disable sleep event		 */
#define FL_INV_SLP_SC0		0x00000001	/*	 Using SCI0 			 */
#define FL_INV_SLP_SC1		0x00000002	/*	 Using SCI1 			 */
#define FL_INV_SLP_DM0		0x00000004	/*	 Using DMAC0			 */
#define FL_INV_SLP_DM1		0x00000008	/*	 Using DMAC1			 */
#define FL_INV_SLP_DM2		0x00000010	/*	 Using DMAC2			 */
#define FL_INV_SLP_DM3		0x00000020	/*	 Using DMAC3			 */
#define FL_INV_SLP_IT0		0x00000040	/*	 Using ITU0 			 */
#define FL_INV_SLP_IT1		0x00000080	/*	 Using ITU1 			 */
#define FL_INV_SLP_IT2		0x00000100	/*	 Using ITU2 			 */
#define FL_INV_SLP_IT3		0x00000200	/*	 Using ITU3 			 */
#define FL_INV_SLP_IT4		0x00000400	/*	 Using ITU4 			 */
#define FL_INV_SLP_ON		0x80000000	/*	 Sleeping				 */
#define FL_INV_APO			0x0008		/* Disable APO event		 */
#define FL_INV_APO_KEY		0x00000001	/*	 Not key wait			 */
#define FL_INV_APO_TRG		0x00000002	/*	 Not trigger wait		 */
#define FL_INV_APO_IR		0x00010000	/*	 Communicating (IR) 	 */
#define FL_INV_APO_14P		0x00020000	/*	 Communicating (serial/3pin)*/
#define FL_INV_APO_OBR		0x00040000	/*	 Scanning barcode		 */
#define FL_INV_APO_USR		0x80000000	/*	 Disable APO from application*/
#define FL_INV_CLK			0x0009		/* Disable change CPU mode event*/
#define FL_INV_CLK_IR		0x00010000	/*	 Communicating (IR) 	 */
#define FL_INV_CLK_14P		0x00020000	/*	 Communicating (serial/3pin)*/
#define FL_INV_CLK_OBR		0x00040000	/*	 Scanning barcode		 */
#define FL_INV_CLK_FIL		0x00080000	/*	 Accessing file 		 */
#define FL_INV_CLK_BEP		0x80000000	/*	 Ringing beep			 */
#define FL_WAT_KEY			0x000a		/* Key wait event			 */
#define FL_WAT_KEY_OBR9 	0x00020000	/*	 OBR notice (buffer#9)	 */
#define FL_WAT_KEY_OBR8 	0x00040000	/*	 OBR notice (buffer#8)	 */
#define FL_WAT_KEY_OBR7 	0x00080000	/*	 OBR notice (buffer#7)	 */
#define FL_WAT_KEY_OBR6 	0x00100000	/*	 OBR notice (buffer#6)	 */
#define FL_WAT_KEY_OBR5 	0x00200000	/*	 OBR notice (buffer#5)	 */
#define FL_WAT_KEY_OBR4 	0x00400000	/*	 OBR notice (buffer#4)	 */
#define FL_WAT_KEY_OBR3 	0x00800000	/*	 OBR notice (buffer#3)	 */
#define FL_WAT_KEY_OBR2 	0x01000000	/*	 OBR notice (buffer#2)	 */
#define FL_WAT_KEY_OBR1 	0x02000000	/*	 OBR notice (buffer#1)	 */
#define FL_WAT_KEY_LB		0x04000000	/*	 LB occur				 */
#define FL_WAT_KEY_FNC		0x08000000	/*	 Press event notice key  */
#define FL_WAT_KEY_IN		0x40000000	/*	 There is key enter 	 */
#define FL_WAT_KEY_MOD		0x80000000	/*	 Backlit on/off mode (0FON)*/
#define FL_WAT_BAR			0x000b		/* Wait for barcode scanning event*/
#define FL_WAT_BAR_TRG		0x00000001	/*	 There is trigger key enter*/
#define FL_WAT_BAR_SCN		0x00000002	/*	 Complete barcode scanning*/
#define FL_WAT_BAR_STM		0x00010000	/*	 Scanning time out		 */
#define FL_WAT_COM			0x000c		/* Wait for communication event*/
#define FL_WAT_IR_RCV		0x00000001	/*	 Receive data  (IR) 	 */
#define FL_WAT_IR_CER		0x00000004	/*	 Communication error(IR)  */
#define FL_WAT_IR_DR		0x00000010	/*	 Time out (DR) (IR) 	 */
#define FL_WAT_IR_CD		0x00000020	/*	 Time out (CD) (IR) 	 */
#define FL_WAT_IR_CS		0x00000040	/*	 Time out (CS) (IR) 	 */
#define FL_WAT_IR_RT		0x00000080	/*	 Receive time out (IR)	 */
#define FL_WAT_IR_PER		0x00000400	/*	 Protocol error (IR)	 */
#define FL_WAT_IR_KEY		0x00001000	/*	 Cancel communication (IR)*/
#define FL_WAT_IR_LB		0x00002000	/*	 LB occur	   (IR) 	 */
#define FL_WAT_14P_RCV		0x00010000	/*	 Receive data  (14pin)	 */
#define FL_WAT_14P_DMA		0x00020000	/*	 Complete DMA transfer (14pin)*/
#define FL_WAT_14P_CER		0x00040000	/*	 Communication error (14pin)*/
#define FL_WAT_14P_DR		0x00100000	/*	 Timer out (DR) (14pin)  */
#define FL_WAT_14P_CD		0x00200000	/*	 Timer out (CD) (14pin)  */
#define FL_WAT_14P_CS		0x00400000	/*	 Timer out (CS) (14pin)  */
#define FL_WAT_14P_RT		0x00800000	/*	 Receive time out (14pin)*/
#define FL_WAT_14P_PER		0x04000000	/*	 Protocol error (14pin)  */
#define FL_WAT_14P_KEY		0x10000000	/*	 Cancel communication (14pin)*/
#define FL_WAT_14P_LB		0x20000000	/*	 LB occur		(14pin)  */
#define FL_WAT_LB0			0x80000000	/*	 LB0 occur		( COM )  */
#define FL_5V_RDY			0x000d		/* Ready for VCC(5V) event	 */
#define FL_5V_RDY_OK		0x00000001	/*	 Ready for VCC			 */
#define FL_LB_INT			0x000e		/* Power down event 		 */
#define FL_LB_INT_LB0		0x00000001	/*	 LB0 (empty main battery)*/
#define FL_LB_INT_LB1		0x00000002	/*	 LB1 (main battery power down)*/
#define FL_LB_INT_LB2		0x00000004	/*	 LB2 (sub battery power down)*/
#define FL_LB_INT_LB3		0x00000008	/*	 LB3 (Unused)			 */
#define FL_LB_INT_LB4		0x00000010	/*	 LB4 (APO)				 */
#define FL_LB_INT_LB5		0x00000020	/*	 LB5 (Power OFF key)	 */
#define FL_LB_MODE			0x000f		/* Notice mode event		 */
#define FL_CUTY 			0x0010		/* Time out of communication utility event*/
#define FL_CUTY_IR			0x00000001	/*	 Receive time out (IR)	 */
#define FL_CUTY_14P 		0x00010000	/*	 Receive time out (14pin)*/
#define FL_CUTY_LB			0x0011		/* Power down of communication utility event*/

/*============= Semaphore ID ==============================================*/
#define SM_MEMORY			0x0001		/* Semaphore of memory control */
#define SM_FILE 			0x0002		/* Semaphore of file control */
#define SM_SYSD 			0x0003		/* Semaphore of system data control */
#define SM_IO_PRTC			0x0004		/* Semaphore of write protection for external I/O */
#define SM_IO_CNT			0x0005		/* Semaphore of write protection count for external I/O */
#define SM_FL_PRTC			0x0006		/* Semaphore of write protection for file area */
#define SM_FL_CNT			0x0007		/* Semaphore of write protection count for file area */
#define SM_DSP				0x0008		/* Semaphore of display BIOS */
#define SM_KEY				0x0009		/* Semaphore of key BIOS	 */
#define SM_OBR				0x000a		/* Semaphore of OBR BIOS	 */
#define SM_IR				0x000b		/* Semaphore of communication BIOS (IR) */
#define SM_14P				0x000c		/* Semaphore of communication BIOS (serial/3pin) */

/*============= Function code of extension SVC ================================*/
#define SVC_SLEEP			1		/* Extension SVC  SLEEP Call	 */
#define SVC_DCOD			2		/* Extension SVC  DECODE Call	 */

/*============= Others ==============================================*/
#define S_RESET 			0x0000		/* Reset		*/
#define S_RESUME_OFF		0x0001		/* Resume off		*/

/*------------- Struct Definition -----------------------------------*/

/*------------- Error  Definition -----------------------------------*/
#define E_PRM				0xFFFFFFFE	/* Parameter error	*/
#define E_NG				0xFFFFFFFF	/* Error end			*/


/*********************************************************************/
/*				Data control							  #002		 */
/*********************************************************************/
/*				   Ver 1.00 (94.12.09)								 */
/*				   Ver 1.01 (95.07.07)								 */
/*------------- Symbol Definition -----------------------------------*/
/*============= System data control ========================================*/
										/* Function code			 */
#define SYSD_FNC_NONOP		0x00000000	/*	 Unrequest				 */
#define SYSD_FNC_READ		0x00000001	/*	 Request read			 */
#define SYSD_FNC_WRITE		0x00000002	/*	 Request write			 */
#define SYSD_FNC_INIT		0x00000003	/*	 Request initialization in error */
/*#define SYSD_FNC_NONCHG	0x00000004		 Request initialization in error */
#define SYSD_FNC1_KNL_ON	0x00000000	/*	 Execute request in kernel start up */
#define SYSD_FNC1_KNL_OFF	0x00000001	/*	 Execute request at not activating kernel


										/* Discrimination ID		 */
#define SYSD_PWR			0x0000		/*	 Power BIOS 			 */
#define SYSD_KEY			0x0001		/*	 Key BIOS				 */
#define SYSD_OBR			0x0002		/*	 OBR BIOS				 */
#define SYSD_DSP			0x0003		/*	 Display BIOS			 */
#define SYSD_COM0			0x0004		/*	 IR 					 */
#define SYSD_COM1			0x0005		/*	 Serial 				 */
#define SYSD_COM2			0x0006		/*	 3pin					 */
#define SYSD_TIM			0x0007		/*	 Timer					 */
#define SYSD_SYS			0x0008		/*	 System 				 */
#define SYSD_PRO			0x0009		/*	 Protocol				 */


										/* Data definition			 */
										/* Power BIOS association	 */
#define APO_DEF_TIME		10			/*	 APO(10minutes) 		 */
#define ABO_DEF_TIME		15			/*	 APO(15minutes) 		 */
#define RESUME_ON			 1			/*	 Resume on				 */
#define RESUME_OFF			 0			/*	 Resume off 		 */
										/* Key BIOS association 	 */
#define CLICK_ON			 1			/*	 Key click on		 */
#define CLICK_OFF			 0			/*	 Key click off		 */
										/* Display BIOS association  */
#define FONT6_SET			 1			/*	 Appointment 6 font 	 */
#define FONT8_SET			 0			/*	 Appointment 8 font 	 */
#define ENG_SET 			 1			/*	 Appointment english	 */
#define JPN_SET 			 0			/*	 Appointment japanese	 */
										/* OBR association			 */
#define RD_DEF_CNT			 1			/*	 Read count (1 time)	 */
#define CMP_DEF_CNT 		 1			/*	 Verify count (1 time)  */
#define SCA_DEF_TIM 		 4			/*	 Scanning time (4 seconds)*/
										/* Timer association		 */
#define BUZZ_OFF			 0			/*	 Volume of beep (off)	 */
#define BUZZ_LOW			 1			/*	 Volume of beep (min)	 */
#define BUZZ_LOUD			 2			/*	 Volume of beep (max)	 */
										/* Protocol association 	 */
#define NRT_DEF_TIM 		 3			/*	 Usual receive time out  */
										/*				(3 seconds)  */
#define NRC_DEF_TIM 		 3			/*	 Retry count of usual receive */
										/*				(3 times)	 */
#define MRT_DEF_TIM 		30			/*	 Connect with multi drop */
										/*	 Data link receive time out */
										/*		(30 milli seconds)	 */
#define PST_DEF_TIM 		60			/*	 Bidirect connection	 */
										/*	 Data link receive time out */
										/*		Transmitting side	 */
										/*				(60 seconds) */
#define PRT_DEF_TIM 		10			/*	 Bidirect connection	 */
										/*	 Data link receive time out */
										/*		Receiving side		 */
										/*				(10 seconds) */
#define PRRC_DEF_TIM		10			/*	 Bidirect connection	 */
										/*	 Retry count of data link receive */
										/*		Receiving side		 */
										/*				(10 times)	 */
										/*							 */
/*============== File control =======================================*/
										/* Mode of file open		 */
#define O_RDONLY			 1			/*	 Read only				 */
#define O_WRONLY			 2			/*	 Write only 			 */
#define O_RDWR				 4			/*	 Both of read & write	 */
#define O_CREAT 			 8			/*	 Create new file		 */
#define O_TRUNC 			16			/*	 Delete of file data	 */
#define O_APPEND			32			/*	 Position of seek is the file end */

										/* Run time of file open	 */
#define SUM_SKIP_OFF		 0			/*	 Non skip of sum check	 */
#define SUM_SKIP_ON 		 1			/*	 Skip of sum check		 */

										/* Set position of FCT reading */
#define DAT_FILE_TOP		 0			/*	 File of begin			 */
#define DAT_FILE_NEXT		 1			/*	 Next file				 */

#define E_NON				 0xFFFF0200 /* There is not data for search function */



/*-------------- Struct Definition ----------------------------------*/
/*============== System data control =======================================*/
typedef struct		sys_pwr
{										/* Power BIOS association	 */
	W		apo;						/*	 APO time				 */
	W		abo;						/*	 ABO time				 */
	W		res_md; 					/*	 Resume mode (ON/OFF)	 */
}DAT_PWR_STR;

typedef struct		sys_key
{										/* Key BIOS association 	 */
	W		clk_md; 					/*	 Key click mode (ON/OFF) */
}DAT_KEY_STR;

typedef struct		sys_obr
{										/* OBR BIOS association 	 */
	W		rd_ct;						/*	 Read count 			 */
	W		cmp_ct; 					/*	 Verify count			 */
	W		scn_tm; 					/*	 Scanning time			 */
}DAT_OBR_STR;

typedef struct		sys_disp
{										/* Display BIOS association  */
	W		font_md;					/*	 Font mode (6font/8font) */
	W		lang_md;					/*	 Langage mode (Jap/Eng)  */
}DAT_DSP_STR;

typedef struct		sys_tty
{										/* Communication BIOS association */
	W		speed;						/*	 Baud rate				 */
	W		length; 					/*	 Length of data 		 */
	W		parity; 					/*	 Parity bit 			 */
	W		stop_bit;					/*	 Stop bit				 */
}DAT_COM_STR;

typedef struct		sys_time
{										/* Timer BIOS association	 */
	W		buzzer; 					/*	 Volume of beep 		 */
}DAT_TIM_STR;

typedef struct		sys_dat
{										/* System association		 */
	UB		sys_id[7];					/*	 System ID				 */
	UB		bios_ver[7];				/*	 BIOS Ver.				 */
	UW		mac_type;					/*	 Machine type			 */
}DAT_SYS_STR;

typedef struct		sys_pro
{										/* Protocol association 	 */
	W		non_rec_tmout;				/*	 Time out of usual reseive */
	W		non_retry_ct;				/*	 Retry count of usual receive */
	W		mal_rec_tmout;				/*	 Time out of receive with multi drop */
	W		ptp_snd_tmout;				/*	 Time out of bidirect connection receive */
										/*			(Transmitting side) */
	W		ptp_rec_tmout;				/*	 Time out of bidirect connection receive */
										/*			(Receiving side) */
	W		ptp_rec_retry_ct;			/*	 Retry count of bidirect connection receive */
										/*			(Receiving side) */
}DAT_PRO_STR;
/*============== File control =======================================*/
typedef struct		dir
{										/* File information table	 */
	B	filename[8];					/*	 File name (prefix) 	 */
	B	extension[3];					/*	 File name (suffix) 	 */
	W	top_adr;						/*	 Starting address		 */
	W	size;							/*	 Size of file			 */
	UW	date_tm;						/*	 Date/Time				 */
	W	attribute;						/*	 File attribute (not using) */
}DIR_TBL;

/*-------------- Error	Definition ----------------------------------*/
#define E_LOWERR	-1					/* End status of primitive function */


/*********************************************************************/
/*				Display BIOS							  #003		 */
/*********************************************************************/
/*------------- Symbol Definition -----------------------------------*/
#define LCD_OFF 			 0			/* Display off			 */
#define LCD_ON				 1			/* Display on			 */

#define LCD_CSR_OFF 		 0			/* Not indicate cursor	 */
#define LCD_CSR_UNDER		 1			/* Cursor type : under line */
#define LCD_CSR_BLOCK		 2			/* Cursor type : block	 */

#define LCD_ANK_LIGHT		 0			/* ANK display mode : reduction */
#define LCD_ANK_STANDARD	 1			/* ANK display mode : standard */

#define LCD_ATTR_NORMAL 	0x0000		/* Normal display mode		 */
#define LCD_ATTR_REVERS 	0x0001		/* Reversal display mode	 */
#define LCD_ATTR_WIDTH		0x0002		/* Highlight display mode	 */

#define LCD_LINE_OFF		 0			/* Line delete				 */
#define LCD_LINE_ON 		 1			/* Line draw				 */

#define LCD_SYMBOL_KEY		 0			/* Symbol of key enter mode  */
#define LCD_SYMBOL_MASTER	 1			/* Power down symbol of main battery */
#define LCD_SYMBOL_SUB		 2			/* Power down symbol of sub battery */
#define LCD_SYMBOL_OFF		 0			/* Symbol lights out 		 */
#define LCD_SYMBOL_ON		 1			/* Symbol lighting			 */

#define LCD_LED_OFF 		 0			/* LED lights out			 */
#define LCD_LED_ON			 1			/* LED lighting 			 */

#define LCD_LED_GREEN		 0			/* Green LED lighting		 */
#define LCD_LED_RED 		 1			/* Red LED lighting 		 */

#define LCD_CONTRAS_UP		 0			/* Contrast up				 */
#define LCD_CONTRAS_DOWN	 1			/* Contrast down			 */

#define LCD_6DOT_FILE		 0			/* Exceptional character file (6 dots) */
#define LCD_8DOT_FILE		 1			/* Exceptional character file (8 dots) */

#define LCD_LF_OFF			 0			/* Not scrolling up at line edge */
#define LCD_LF_ON			 1			/* Scrolling up at line edge */

										/* Kind of user file		 */
#define LCD_K6_FILE 		 0			/* Kanji file (6 dots)		 */
#define LCD_K8_FILE 		 1			/* Kanji file (8 dots)		 */
#define LCD_AS6_FILE		 2			/* Standard ANK file (6 dots)*/
#define LCD_AS8_FILE		 3			/* Standard ANK file (8 dots)*/
#define LCD_AL6_FILE		 4			/* Reduction ANK file (6 dots)*/
#define LCD_AL8_FILE		 5			/* Reduction ANK file (8 dots)*/


/*------------- Struct Definition -----------------------------------*/

/*------------- Error  Definition -----------------------------------*/


/*********************************************************************/
/*				Key BIOS								  #004		 */
/*********************************************************************/
/*------------- Symbol Definition -----------------------------------*/
#define KEY_INT_EXT 		0x01		/* Event notice key 		 */
#define KEY_LB_EXT			0x02		/* End of LB				 */
#define KEY_OBR_EXT 		0x04		/* Notice of OBR			 */
#define KEY_CLR_EXT 		0x08		/* CLR key					 */
#define KEY_FULL_CHR		0x10		/* Input area full			 */
#define KEY_NON_EXT 		0x00		/* not return condition 	 */

#define ECHO_ON 			00			/* Enable echo back 		 */
#define ECHO_OFF			01			/* Disable echo back		 */

#define KEY_NUM_CLR_ON		0			/* Clear					 */
#define KEY_NUM_CLR_OFF 	1			/* Not clear				 */

#define FNC_SET 			00			/* Set function 			 */
#define FNC_GET 			01			/* Get function 			 */
#define FNC_1				00			/* Function 1				 */
#define FNC_2				01			/* Function 2				 */
#define FNC_3				02			/* Function 3				 */
#define FNC_4				03			/* Function 4				 */
#define FNC_5				04			/* Function 5				 */
#define FNC_6				05			/* Function 6				 */
#define FNC_7				06			/* Function 7				 */
#define FNC_8				07			/* Function 8				 */

#define FNC_MODE_SET		0x00000000	/* Set						 */
#define FNC_MODE_CLR		0x00000001	/* Clear					 */
#define FNC_MODE_RED		0x00000002	/* Read 					 */

#define KEY_STS 			0xFFFFFFFF	/* Get key mode 			 */
#define KEY_NUM_MODE		0x00000000	/* Numeric input mode		 */
#define KEY_CHR_MODE		0x00000001	/* Character input mode 	 */

/*------------- Struct Definition -----------------------------------*/
typedef struct		st_key_inp
{
	UB		ext;						/* Return condition 		 */
	UB		echo;						/* Echo back				 */
	H		font_size;					/* Size of font 			 */
	H		type;						/* Type 					 */
	UH		column_pos; 				/* Input position (column)	 */
	UH		line_pos;					/* Input position (line)	 */
} KEY_INP;

typedef struct		st_key_inps
{
	UB		ext;						/* Return condition 		 */
	UB		echo;						/* Echo back				 */
	H		font_size;					/* Size of font 			 */
	H		type;						/* Type 					 */
	UH		len;						/* Number of input character (ANK) */
	UH		column_pos; 				/* Input position (column)	 */
	UH		line_pos;					/* Input position (line)	 */
	UH		column_len; 				/* Position of input character */
	UH		clr_type;					/* Clearing after default data display */
} KEY_INPS;

typedef struct	stKeyCode				/* Key code 				 */
{
	UB		attr;						/* Attribute				 */
	UB		code;						/* Code 					 */
} KEYFORM;

/*------------- Error  Definition -----------------------------------*/
#define E_KEY_INT			0xFFFF0400	/* Event notice key 		 */
#define E_KEY_LB			0xFFFF0401	/* End of LB				 */
#define E_KEY_OBR			0xFFFF0402	/* End of OBR event 		 */
#define E_KEY_CLR			0xFFFF0403	/* CLR key					 */
#define E_KEY_FUL			0xFFFF0404	/* Buffer full end			 */
#define E_KEY_MD			0xFFFF0401	/* During key input 		 */

/****************************************************************/
/*					Communication BIOS					#005	*/
/****************************************************************/
/*-------------------- Symbol Definitions ----------------------*/
/*================== Communication Port No. ====================*/
#define COM0			0		/* COM 0						*/
#define COM1			1		/* COM 1						*/
#define COM2			2		/* COM 2						*/

/*======================= Baud Rate ============================*/
#define B_153600		0x00090000	/* 153600 bps				*/
#define B_76800 		0x00080000	/*	76800 bps				*/
#define B_38400 		0x00070000	/*	38400 bps				*/
#define B_19200 		0x00060000	/*	19200 bps				*/
#define B_9600			0x00050000	/*	 9600 bps				*/
#define B_4800			0x00040000	/*	 4800 bps				*/
#define B_2400			0x00030000	/*	 2400 bps				*/
#define B_1200			0x00020000	/*	 1200 bps				*/

/*======================== Parity Bit ==========================*/
#define PARI_NON		0x00000000	/* Non Parity				*/
#define PARI_ODD		0x00001000	/* Odd Parity				*/
#define PARI_EVN		0x00003000	/* Even Parity				*/

/*==================== Chara. Bit Length =======================*/
#define CHAR_8			0x00000C00	/* 8 Bit					*/
#define CHAR_7			0x00000800	/* 7 Bit					*/

/*========================== Stop Bit ==========================*/
#define STOP_1			0x00004000	/* 1 Bit					*/
#define STOP_2			0x0000C000	/* 2 Bit					*/

/*==================== RTS Initial Value =======================*/
#define RTS_ON			0x00000010	/* RTS On					*/
#define RTS_OFF 		0x00000000	/* RTS Off					*/

/*===================== ER Initial Value =======================*/
#define ER_ON			0x00000008	/* ER On					*/
#define ER_OFF			0x00000000	/* ER Off					*/

/*======================= SI/SO Conrol =========================*/
#define SI_ON			0x00000080	/* On						*/
#define SI_OFF			0x00000000	/* Off						*/

/*=================== Buffer Busy Control ======================*/
#define BUSY_OFF		0x00000000	/* Non Control				*/
#define XON_XOFF		0x00000020	/* X Control by DC1,DC3 	*/
#define BUSY_CHAR		0x00000040	/* X Control by Appo.Control Code*/
#define RS_CS			0x00000060	/* RS/CS Flow Control		*/

/*=================== Hold ON/OFF Control ======================*/
#define HOLD_ON 		1			/* Hold ON					*/
#define HOLD_OFF		0			/* Hold OFF 				*/

/*=================== Break ON/OFF Control =====================*/
#define BRK_ON			1			/* Break ON 				*/
#define BRK_OFF 		0			/* Break OFF				*/

/*================= RS Signal ON/OFF Control ===================*/
#define RS_ON			1			/* RS ON					*/
#define RS_OFF			0			/* RS OFF					*/

/*================= ER Signal ON/OFF Control ===================*/
#define ERS_ON			1			/* ER ON					*/
#define ERS_OFF 		0			/* ER OFF					*/

/*================ ER/RS Signal ON/OFF Control =================*/
#define ERRS_ON 		1			/* ER/RS ON 				*/
#define ERRS_OFF		0			/* ER/RS OFF				*/

/*=============== Error Code Buffering Control =================*/
#define ERRCD_ON		1			/* Buffering ON 			*/
#define ERRCD_OFF		0			/* Buffering OFF			*/

/*==============================================================*/
#define COM0_OPEN		0x000000001 /* COM 0 Open				*/
#define COM1_OPEN		0x000000002 /* COM 1 Open				*/
#define COM2_OPEN		0x000000004 /* COM 2 Open				*/

/*================= Receive Handler Control ====================*/
#define STAND_HDR		1			/* Standard Handler 		*/
#define HIGH_HDR		0			/* High-Speed Handler		*/

/*======================= Tx/Rx Control ========================*/
#define C_RXTXENB		0x00		/* Tx/Rx Valid				*/
#define C_RXTXDSB		0x01		/* Tx/Rx Invalid			*/
#define C_RXENB 		0x02		/* Rx Valid 				*/
#define C_TXENB 		0x03		/* Tx Valid 				*/
#define C_RXDSB 		0x04		/* Rx Invalid				*/
#define C_TXDSB 		0x05		/* Tx Invalid				*/

/*====================== I/O Box Control =======================*/
#define C_IOBOXENB		1			/* Tx ON					*/
#define C_IOBOXDSB		0			/* Tx OFF					*/

/*====================== COM Status mask =======================*/
#define STS_RD			0x00000001	/* Rx Data					*/
#define STS_CI			0x00000002	/* CI OFF					*/
#define STS_CD			0x00000004	/* CD OFF					*/
#define STS_CS			0x00000008	/* CS OFF					*/
#define STS_DR			0x00000010	/* DR OFF					*/
#define STS_PRI 		0x00000100	/* Parity Error 			*/
#define STS_OVR 		0x00000200	/* Over-run Error			*/
#define STS_FRE 		0x00000400	/* Framing Error			*/
#define STS_FUL 		0x00000800	/* Buffer full Error		*/
#define STS_BRK 		0x00001000	/* Break Detect 			*/

/*-------------------- Struct Definition -----------------------*/
typedef struct	tim_tbl
{									/* Time-out Initial Value	*/
	H		cs; 					/* CTS Time-out 			*/
	H		dr; 					/* DSR Time-out 			*/
	H		cd; 					/* CD Time-out				*/
} TIM_TBL;

typedef struct	del_tbl
{									/* Rx Delete Code Table 	*/
	B		del_n;					/* Rx Delete Code Number	*/
	UB		del_c[4];				/* Rx Delete Code			*/
} DEL_TBL;

typedef struct	com_sts
{									/* Rx Buffer Status 		*/
	H		char_no;				/* Rx Character Number		*/
	H		rest_no;				/* Buffer Remain			*/
	UB		char_cod;				/* Code of Buffer Head		*/
} COM_STS;

/*--------------------- Error Definition -----------------------*/
#define CERR_PARITY 	0x00000001	/* Parity error 			*/
#define CERR_OVERRUN	0x00000002	/* Over-run error			*/
#define CERR_FLAMING	0x00000004	/* Framing error			*/
#define CERR_BUFFULL	0x00000008	/* Buffer full				*/
#define CERR_CDTIMEOUT	0x00000010	/* CD time out				*/
#define CERR_CSTIMEOUT	0x00000020	/* CS time out				*/
#define CERR_DRTIMEOUT	0x00000040	/* DR time out				*/
#define CERR_BREAK		0x00000080	/* Break error				*/
#define CERR_NORECOVER	0x00000100	/* Not recover error		*/
#define CERR_DEMESNE	0x00000200	/* Demesne error			*/
#define CERR_NOTOPEN	0x00000400	/* Not open 				*/
#define CERR_LB0		0x00000800	/* LB0 Main battery dead	*/
#define CERR_LB4		0x00001000	/* LB4 Auto Power Off		*/
#define CERR_PARAMETER	0x00002000	/* Parameter error			*/
#define CERR_LB5		0x00004000	/* LB5 Off key				*/
#define CERR_LB1		0x00008000	/* LB1 Main battery low 	*/
#define CERR_RCVTOUT	0x00010000	/* Recieve time out error	*/
#define CERR_PARITY2	0x00020000	/* Parity error2			*/
#define CERR_OVERRUN2	0x00040000	/* Over run error2			*/
#define CERR_FLAMING2	0x00080000	/* Framing error2			*/


/****************************************************************/
/*							Power BIOS					#006	*/
/****************************************************************/
/*-------------------- Symbol Definitions ----------------------*/
#define PWR_ON			0x01	/* Power ON  Setting Appoint	*/
#define PWR_OFF 		0x00	/* Power OFF Setting Appoint	*/
#define PWR_RESUME_ON	0x01	/* Resume ON  Setting Appoint	*/
#define PWR_RESUME_OFF	0x00	/* Resume OFF Setting Appoint	*/
#define PWR_SC3I		0x01	/* Serial-port Power			*/
#define PWR_IR			0x02	/* Infrared Power				*/
#define PWR_BL			0x14	/* Backlit Power				*/
#define PWR_LZRON		0x08	/* Laser Power					*/
#define PWR_V50ON		0x00	/* Buzzer Power 				*/
#define PWR_BTWE		0x40	/* BOOT-block writting Power	*/
#define PWR_12VON		0x80	/* F-ROM writting Power 		*/
#define PWR_HIGH		0x02	/* High Speed Mode				*/
#define PWR_STD 		0x01	/* Standard Speed Mode			*/
#define PWR_LOW 		0x00	/* Low Speed Mode				*/
#define PWR_READ		0x03	/* Read CPU Mode				*/
#define PWR_CPUHIGH 	0x02	/* Read of High Speed Mode		*/
#define PWR_CPUSTD		0x01	/* Read of Standard Speed Mode	*/
#define PWR_CPULOW		0x00	/* Read of Low Speed Mode		*/
#define PWR_APOKEY		0x00000001	/* Key of Disable APO		*/
#define PWR_APOTRG		0x00000002	/* APO Not Disable Triggar	*/
#define PWR_APOIR		0x00010000	/* APO In Disable(Infrared) */
#define PWR_APO14		0x00020000	/* APO In Disable(Serial/3Pin)*/
#define PWR_APOOBR		0x00040000	/* APO In Disable(Barcode Scan)*/

/*-------------------- Struct Definition -----------------------*/

/*-------------------- Error  Definition -----------------------*/


/****************************************************************/
/*			OBR BIOS									#007	*/
/****************************************************************/
/*-------------------- Symbol Definitions ----------------------*/
/*========================= Read Code ==========================*/
#define OBR_CD39		0x00000001	/* CODE-39					*/
#define OBR_NW_7		0x00000002	/* NW-7 					*/
#define OBR_WPCA		0x00000004	/* WPC(UPC-E except)addon	*/
#define OBR_WPC 		0x00000008	/* WPC(UPC-E except)		*/
#define OBR_UPEA		0x00000010	/* UPC-E addon				*/
#define OBR_UPE 		0x00000020	/* UPC-E					*/
#define OBR_IDF 		0x00000040	/* Industrial 2of5			*/
#define OBR_ITF 		0x00000080	/* Interleved 2of5			*/
#define OBR_CD93		0x00000100	/* CODE-93					*/
#define OBR_CD128		0x00000200	/* CODE-128 				*/
#define OBR_MSI 		0x00000400	/* MSI						*/
#define OBR_NONDT		0xFFFFFFFF	/* Data Nothing(Only OBR_getc/s)*/
#define OBR_ALL 		0x000007FF	/* Select All Codes 		*/

/*====================== Output Format =========================*/
#define OBR_39SON		0x00	/* Exist CODE39 Start/Stop		*/
#define OBR_39SOFF		0x01	/* Non Exist CODE39 Start/Stop	*/
#define OBR_39ASON		0x02	/* Exist CODE39 Full ASC Start/Stop*/
#define OBR_39ASOFF 	0x03	/* Non Exist CODE39 Full ASC Start/Stop*/
#define OBR_NWSON		0x00	/* Exist NW-7 Start/Stop		*/
#define OBR_NWSOFF		0x01	/* Non Exist NW-7 Start/Stop	*/
#define OBR_UPAON		0x00	/* UPC-E Reconstruct Code
										Exist (UPC-A) Output	*/
#define OBR_UPAOFF		0x01	/* UPC-E Reconstruct Code
										Non Exist (UPC-A) Output*/
#define OBR_128AON		0x00	/* CODE128 Convert After		*/
#define OBR_128AOFF 	0x01	/* CODE128 Convert Before		*/
#define OBR_OUTOFF		0x00	/*								*/

/*=============== Check-digit/Check character ==================*/
#define OBR_CHK_ON	0x80000000	/* Execute a Check-digit(OBR_open)*/
#define OBR_OUT_ON	0x40000000	/* Output Check character(OBR_open)*/
#define OBR_CHKDON	0x01		/* Execute a Check-digit(mode R/W)*/
#define OBR_CHKDOFF 0x00		/* Non Execute a Check-digit(  " )*/
#define OBR_CDMSEV	0x02		/* MSI Execute a Check-digit
										1st. mod11(mode R/W)	*/
#define OBR_CDMSTN	0x03		/* MSI Execute a Check-digit
										1st mod10(mode R/W) 	*/
#define OBR_CHKKON	0x01		/* Exist Check character Output
													(mode R/W)	*/
#define OBR_CHKKOFF 0x00		/* Non Exist Check character Output
													(mode R/W)	*/

/*===================== Reading Method =========================*/
#define OBR_TYPE0	0x00		/* Single Read					*/
#define OBR_TYPE1	0x01		/* Continuous Read(with Triggar)*/
#define OBR_TYPE2	0x02		/*		"	"   (with'n Trig.)*/
#define OBR_TYPE3	0x03		/*		"	"   (Change Trig.)*/

/*===================== Gain Control ===========================*/
#define OBR_GAINL	0x00		/* Gain Large					*/
#define OBR_GAINS	0x01		/* Gain Small					*/
#define OBR_GAINAND 0x02		/* Gain AND 					*/
#define OBR_GAINOR	0x03		/* Gain OR						*/

/*===================== Buzzer Control =========================*/
#define OBR_BUZOFF	0x00		/* Non Exist Buzzer Control 	*/
#define OBR_BUZON	0x01		/* Exist Buzzer Control 		*/

/*======================= LED Control ==========================*/
#define OBR_LEDOFF	0x00		/* Non Exist LED Control		*/
#define OBR_LEDON	0x01		/* Exist LED Control			*/

/*=============== Appointment of Output Buffer =================*/
#define OBR_BUFOBR	0x00		/* Select OBR Data Buffer		*/
#define OBR_STON	0x01		/* Select Key Buffer(with START Code)*/
#define OBR_STOFF	0x02		/*		"		"   (without    "	)*/

/*============== Appointment of Termination Code ===============*/
#define OBR_ENDCR	0x00		/* CR Code(0x0d)				*/
#define OBR_ENDLF	0x01		/* LF Code(0x0a)				*/
#define OBR_ENDCL	0x02		/* CR+LF Code(0x0d0a)			*/

/*================= Reading Method ============================*/
#define OBR_NORM	0x00		/* Standard Read				*/
#define OBR_DANN	0x01		/* Step Read					*/

/*----------------- Structure Definition -----------------------*/
typedef struct	m_tbl
{								/* Action Mode Table			*/
	UW		Code;				/* Read Possible Code			*/
	UB		Cd39[6];			/* Code39						*/
	UB		Nw7[6]; 			/* Nw-7 						*/
	UB		Wpcea[6];			/* Wpc(except Upc-e)addon		*/
	UB		Wpce[6];			/* Wpc(except Upc-e)			*/
	UB		Upcea[6];			/* Upc-e addon					*/
	UB		Upce[6];			/* Upc-e						*/
	UB		Idsf[6];			/* Industrial 2of5				*/
	UB		Itrf[6];			/* Interleaved 2of5 			*/
	UB		Cd93[6];			/* Code93						*/
	UB		Cd128[6];			/* Code128						*/
	UB		Msi[6]; 			/* Msi							*/
	UB		Resv[21][6];		/* for Expansion				*/
								/* [0] Reserve					*/
								/* [1] Read digit(Min)			*/
								/* [2] Read digit(Max)			*/
								/* [3] Output Format			*/
								/* [4] Check-digit Execute Command*/
								/* [5] Check chara. Output Command*/
	UB		Type;				/* Setting Read Method			*/
	UB		Gain;				/* Setting Gain Control 		*/
	UB		Buzc;				/* Setting Buzzer Control		*/
	UB		Ledc;				/* Setting LED Control			*/
	UB		Bufc;				/* Setting Output Buffer		*/
	UB		Endc;				/* Setting Termination Code 	*/
	UB		Mode;				/* Appoint. Standard/Step Read	*/
	UB		Dumy;				/* dummy						*/
} M_TBL;

/*--------------------- Error Definition -----------------------*/
#define E_OBR_PON	0xFFFF0700	/* Open Condition				*/
#define E_OBR_POF	0xFFFF0701	/* Close Condition				*/


/****************************************************************/
/*						Timer BIOS						#008	*/
/****************************************************************/
/*-------------------- Symbol Definitions ----------------------*/
/*------------------- Structure Definition ---------------------*/
typedef struct	day_tabl
{
	UH		year;				/* Year(1980~2079)				*/
	UB		month;				/* Month(1~12)					*/
	UB		day;				/* Day(1~31)					*/
} DAY_DAT;

typedef struct	tim_tabl
{
	UB		hour;				/* Hour(0~23)					*/
	UB		mint;				/* Minute(0~59) 				*/
	UB		sec;				/* Second(0~59) 				*/
} TIM_DAT;

/*--------------------- Error Definition -----------------------*/
#define E_TID_OVER	0xFFFFF8FF	/* Timer Entry Max Over 		*/
#define E_TID_NON	0xFFFFF8FE	/* Timer Non Entry				*/

/****************************************************************/
/*					Communication Utility				#031	*/
/****************************************************************/
/*-------------------- Symbol Definitions ----------------------*/
/*====================== Connect Mode ==========================*/
#define CU_CNCT_MULT	0x00	/* Connect with multi drop		*/
#define CU_CNCT_PTOP	0x01	/* Bidirect Connection			*/

/*================ Appointment of Interrupt Key ================*/
#define CU_FNC_NON	0x00		/* Non Appoint					*/
#define CU_FNC_1	0x01		/* F1 Key						*/
#define CU_FNC_2	(CU_FNC_1+1)/* F2 Key						*/
#define CU_FNC_3	(CU_FNC_2+1)/* F3 Key						*/
#define CU_FNC_4	(CU_FNC_3+1)/* F4 Key						*/
#define CU_FNC_5	(CU_FNC_4+1)/* F5 Key						*/
#define CU_FNC_6	(CU_FNC_5+1)/* F6 Key						*/
#define CU_FNC_7	(CU_FNC_6+1)/* F7 Key						*/
#define CU_FNC_8	(CU_FNC_7+1)/* F8 Key						*/

/*=============== Appointment of Graph Display =================*/
#define CU_GRAPH_OFF	0x00	/* No Display					*/
#define CU_GRAPH_ON_1	0x01	/* In end Transmission			*/
#define CU_GRAPH_ON_2	0x02	/* In end 1 File Transmission	*/

/*========== Kind of Transmit File(for System Menu) ============*/
#define CU_KIND_SPECIAL 0x01	/* Special Appo. File			*/
#define CU_KIND_OTHER	0x02	/* Except Special Appo. File	*/
#define CU_KIND_ALL 	0xff	/* All of Files 				*/

/*============= Transmit File Info./STAT Value =================*/
#define CU_STAT_TRANS		0	/* Transmit Normal Terminate	*/
#define CU_STAT_OPEN_ERR	1	/* File Open Error				*/
#define CU_STAT_READ_ERR	2	/* File Read Error				*/
#define CU_STAT_WRITE_ERR	3	/* File Write Error 			*/
#define CU_STAT_SEND_ERR	4	/* File Transmit Error			*/
#define CU_STAT_PRE_TRANS	255 /* Other Error					*/

/*==================== Error Info./Phase =======================*/
#define CU_PHASE_DATALINK	0	/* Before Data Linking			*/
#define CU_PHASE_INFO_TRANS 1	/* After Data Transmit			*/

/*================== Error Info./Act. Mode =====================*/
#define CU_MODE_RES 	0		/* Response Receive 			*/
#define CU_MODE_CMD 	1		/* Command Receive				*/

/*================= Error Info./Command Kinds ==================*/
#define CU_CMD_NON		0x00	/* Not falling Command			*/
#define CU_CMD_SYN		0x01	/* Synchronous Code 			*/
#define CU_CMD_POL		0x02	/* Polling Command				*/
#define CU_CMD_EOT		0x03	/* EOT Code 					*/
#define CU_CMD_FS_REQ	0x21	/* File Transmit Request Command*/
#define CU_CMD_FR_REQ	0x22	/* File Receive Request Command */
#define CU_CMD_FS_START_REQ 0x23 /* File Transmit Start Req. Command*/
#define CU_CMD_FI_NOTICE 0x24	/* File Transmit Notice Command */
#define CU_CMD_FC_TRANS 0x25	/* File Contents Transmit Command*/
#define CU_CMD_DS_REQ	0x31	/* Data Transmit Command		*/
#define CU_CMD_DR_REQ	0x32	/* Data Receive Command 		*/
#define CU_CMD_DS_START_REQ 0x33 /* Data Transmit Start Command*/
#define CU_CMD_DC_TRANS 0x35	/* Data Contents Transmit Command*/
#define CU_CMD_MT		0xf1	/* Disp. Message Transmit Command*/
#define CU_CMD_DL_OFF	0xf2	/* Data Link Disconnection Command*/

/*=============== Error Info./Error Condition ==================*/
#define CU_ERR_NON		0x00	/* No Error 					*/
#define CU_ERR_PHASE	0x01	/* Func. use Phase Error		*/
#define CU_ERR_FILE 	0x02	/* File I/O Error				*/
#define CU_ERR_CLOCK	0x03	/* CPU Clock Change Error		*/
#define CU_ERR_DL		0x04	/* Data Link Error				*/
#define CU_ERR_DL_RJ	0x05	/* Data Link Interrupt Error	*/
#define CU_ERR_TIMER	0x06	/* Timer use Error				*/
#define CU_ERR_NAK		0x07	/* NAK Receive Retry Over		*/
#define CU_ERR_BCC		0x08	/* BCC error Retry Over 		*/
#define CU_ERR_SEQ		0x09	/* Sequence # Error 			*/
#define CU_ERR_CONT 	0x0a	/* Command Error				*/
#define CU_ERR_EOT		0x0b	/* EOT Receive ->Interrupt		*/
#define CU_ERR_RCVTO	0x0c	/* Receive Time Out 			*/
#define CU_ERR_CRP		0x0d	/* Command/Response Phase Error */
#define CU_ERR_CMD		0x0e	/* Unexpectation Command Receive*/
#define CU_ERR_FILE_NO	0x0f	/* Receive File Number Error	*/
#define CU_ERR_SEND 	0x10	/* Transmit Error				*/
#define CU_ERR_STOP 	0x11	/* Interrupt Key press			*/
#define CU_ERR_NODATA	0x12	/* Receive Data Nothing 		*/
#define CU_ERR_DL_OVER	0x13	/* Multi Drop Re-Datalink Over	*/
#define CU_ERR_UNKNOWN_CMD	0x14 /* Unknown Command Receive 	*/
#define CU_ERR_PRM		0x15	/* Utility Func. Parameter Error*/
#define CU_ERR_RECV 	0x16	/* Receive Error				*/
#define CU_ERR_AP		0x17	/* AP File Error				*/
#define CU_ERR_NOT_SUPPORT	0x40 /* Non Support Error			*/
#define CU_ERR_ABNORMAL 0xfe	/* Comm. UT Logic Error 		*/
#define CU_ERR_OTHER	0xff	/* Other Error					*/

/*------------------- Structure Definition ---------------------*/
/*================= Transmit File Information ==================*/
typedef struct	cu_file_info_form
{
	UB	fileName[11];			/* File Name					*/
	UB	stat;					/* Result Code					*/
} CU_FILE_INFO_FORM;

/*--------------------- Error Definition -----------------------*/
#define E_AP_NO_INST	0xFFFF3101	/* Not Installed Application*/
#define E_AP_CSUM		0xFFFF3102	/* Application Check-sum Error*/

/****************************************************************/
/*				Event Notice							#		*/
/****************************************************************/
#define FL_LB_INT_ID		0x0020		/* LB Notice Mode		*/
#define FL_FK_INT_ID		0x0040		/* Function Notice Mode */
#define FL_TM1_INT_ID		0x0030		/* Timer1 Entry ID		*/
#define FL_TM2_INT_ID		0x0031		/* Timer2 Entry ID		*/

#define FL_FK_INT_FNC1		0x0001		/* F-1 Notice Mode		*/
#define FL_FK_INT_FNC2		0x0002		/* F-2 Notice Mode		*/
#define FL_FK_INT_FNC3		0x0004		/* F-3 Notice Mode		*/
#define FL_FK_INT_FNC4		0x0008		/* F-4 Notice Mode		*/
#define FL_FK_INT_FNC5		0x0010		/* F-5 Notice Mode		*/
#define FL_FK_INT_FNC6		0x0020		/* F-6 Notice Mode		*/
#define FL_FK_INT_FNC7		0x0040		/* F-7 Notice Mode		*/
#define FL_FK_INT_FNC8		0x0080		/* F-8 Notice Mode		*/

#define FL_TM1_INT_RTC0 	0x00000001	/* RTC Timer Entry0 	*/
#define FL_TM1_INT_RTC1 	0x00000002	/* RTC Timer Entry1 	*/
#define FL_TM1_INT_RTC2 	0x00000004	/* RTC Timer Entry2 	*/
#define FL_TM1_INT_RTC3 	0x00000008	/* RTC Timer Entry3 	*/
#define FL_TM1_INT_RTC4 	0x00000010	/* RTC Timer Entry4 	*/
#define FL_TM1_INT_RTC5 	0x00000020	/* RTC Timer Entry5 	*/
#define FL_TM1_INT_RTC6 	0x00000040	/* RTC Timer Entry6 	*/
#define FL_TM1_INT_RTC7 	0x00000080	/* RTC Timer Entry7 	*/
#define FL_TM1_INT_RTC8 	0x00000100	/* RTC Timer Entry8 	*/
#define FL_TM1_INT_RTC9 	0x00000200	/* RTC Timer Entry9 	*/
#define FL_TM1_INT_RTC10	0x00000400	/* RTC Timer Entry10	*/
#define FL_TM1_INT_RTC11	0x00000800	/* RTC Timer Entry11	*/
#define FL_TM1_INT_RTC12	0x00001000	/* RTC Timer Entry12	*/
#define FL_TM1_INT_RTC13	0x00002000	/* RTC Timer Entry13	*/
#define FL_TM1_INT_RTC14	0x00004000	/* RTC Timer Entry14	*/
#define FL_TM1_INT_RTC15	0x00008000	/* RTC Timer Entry15	*/
#define FL_TM1_INT_RTC16	0x00010000	/* RTC Timer Entry16	*/
#define FL_TM1_INT_RTC17	0x00020000	/* RTC Timer Entry17	*/
#define FL_TM1_INT_RTC18	0x00040000	/* RTC Timer Entry18	*/
#define FL_TM1_INT_RTC19	0x00080000	/* RTC Timer Entry19	*/
#define FL_TM1_INT_RTC20	0x00100000	/* RTC Timer Entry20	*/
#define FL_TM1_INT_RTC21	0x00200000	/* RTC Timer Entry21	*/
#define FL_TM1_INT_RTC22	0x00400000	/* RTC Timer Entry22	*/
#define FL_TM1_INT_RTC23	0x00800000	/* RTC Timer Entry23	*/
#define FL_TM1_INT_RTC24	0x01000000	/* RTC Timer Entry24	*/
#define FL_TM1_INT_RTC25	0x02000000	/* RTC Timer Entry25	*/
#define FL_TM1_INT_RTC26	0x04000000	/* RTC Timer Entry26	*/
#define FL_TM1_INT_RTC27	0x08000000	/* RTC Timer Entry27	*/
#define FL_TM1_INT_RTC28	0x10000000	/* RTC Timer Entry28	*/
#define FL_TM1_INT_RTC29	0x20000000	/* RTC Timer Entry29	*/
#define FL_TM1_INT_RTC30	0x40000000	/* RTC Timer Entry30	*/
#define FL_TM1_INT_RTC31	0x80000000	/* RTC Timer Entry31	*/

#define FL_TM2_INT_ITU0 	0x00000001	/* ITU Timer Entry0 	*/
#define FL_TM2_INT_ITU1 	0x00000002	/* ITU Timer Entry1 	*/
#define FL_TM2_INT_ITU2 	0x00000004	/* ITU Timer Entry2 	*/
#define FL_TM2_INT_ITU3 	0x00000008	/* ITU Timer Entry3 	*/
#define FL_TM2_INT_ITU4 	0x00000010	/* ITU Timer Entry4 	*/
#define FL_TM2_INT_ITU5 	0x00000020	/* ITU Timer Entry5 	*/
#define FL_TM2_INT_ITU6 	0x00000040	/* ITU Timer Entry6 	*/
#define FL_TM2_INT_ITU7 	0x00000080	/* ITU Timer Entry7 	*/
#define FL_TM2_INT_ITU8 	0x00000100	/* ITU Timer Entry8 	*/
#define FL_TM2_INT_ITU9 	0x00000200	/* ITU Timer Entry9 	*/
#define FL_TM2_INT_ITU10	0x00000400	/* ITU Timer Entry10	*/
#define FL_TM2_INT_ITU11	0x00000800	/* ITU Timer Entry11	*/
#define FL_TM2_INT_ITU12	0x00001000	/* ITU Timer Entry12	*/
#define FL_TM2_INT_ITU13	0x00002000	/* ITU Timer Entry13	*/
#define FL_TM2_INT_ITU14	0x00004000	/* ITU Timer Entry14	*/
#define FL_TM2_INT_ITU15	0x00008000	/* ITU Timer Entry15	*/
#define FL_TM2_INT_ITU16	0x00010000	/* ITU Timer Entry16	*/
#define FL_TM2_INT_ITU17	0x00020000	/* ITU Timer Entry17	*/
#define FL_TM2_INT_ITU18	0x00040000	/* ITU Timer Entry18	*/
#define FL_TM2_INT_ITU19	0x00080000	/* ITU Timer Entry19	*/
#define FL_TM2_INT_ITU20	0x00100000	/* ITU Timer Entry20	*/
#define FL_TM2_INT_ITU21	0x00200000	/* ITU Timer Entry21	*/
#define FL_TM2_INT_ITU22	0x00400000	/* ITU Timer Entry22	*/
#define FL_TM2_INT_ITU23	0x00800000	/* ITU Timer Entry23	*/
#define FL_TM2_INT_ITU24	0x01000000	/* ITU Timer Entry24	*/
#define FL_TM2_INT_ITU25	0x02000000	/* ITU Timer Entry25	*/
#define FL_TM2_INT_ITU26	0x04000000	/* ITU Timer Entry26	*/
#define FL_TM2_INT_ITU27	0x08000000	/* ITU Timer Entry27	*/
#define FL_TM2_INT_ITU28	0x10000000	/* ITU Timer Entry28	*/
#define FL_TM2_INT_ITU29	0x20000000	/* ITU Timer Entry29	*/
#define FL_TM2_INT_ITU30	0x40000000	/* ITU Timer Entry30	*/
#define FL_TM2_INT_ITU31	0x80000000	/* ITU Timer Entry31	*/

#define VCT_TBL_ADR 		0x0101BA00	/* Vector Table addresss*/

/****************************************************************/
/*					End Of Common Data Definition				*/
/****************************************************************/

