/**********************************************************************/
/*																	  */
/*	PROGRA		   : Header 1 of BIOS jump table (Structure)		  */
/*	SYSTEM		   : VER1.00										  */
/*	FILE		   : bios1def.h 									  */
/*	VERSION 	   : 1.00											  */
/*	AUTHOR		   : CASIO COMPUTER CO.,LTD 						  */
/*	DATE OF GET    : 94/10/17										  */
/*	DATE OF UPDATE :												  */
/*	FUNCTION	   :												  */
/*	HISTORY 	   :												  */
/*																	  */
/* Copyright(C) 1995 CASIO COMPUTER CO.,LTD. All rights reserved.	  */
/**********************************************************************/
#include	<stdio.H>

/****************************************************************************/
/*																			*/
/*		 Structure															*/
/*																			*/
/****************************************************************************/

typedef struct	_BIOSJMPTBL1
{

/*************************************************/
/*		 Data control				#002		 */
/*************************************************/
	ER		(*_dat_system)( FN, ID, VP );			/* Read/Write system data */		
	UW		(*_dat_mem_size)( void );				/* Return free area size */
	UW		(*_dat_fsize)( void );					/* Return free area size */
	int 	(*_open)( char *, int );				/* Open */
	int 	(*_close)( int );						/* Close */
	int 	(*_read)( int, char *, unsigned int );	/* Read */
	int 	(*_write)( int, char *, unsigned int ); /* Write */
	int 	(*_lseek)( int, long, int );			/* Set read/write position */
	char *	(*_sbrk)( unsigned long );				/* Get memory */
	ER		(*_dat_fdel)( B * );					/* Delete named file */
	ER		(*_dat_fdir)( B, struct dir * );		/* Get file information */
	
/*************************************************/
/*		 Disply BIOS				#003		 */
/*************************************************/
	ER		(*_lcd_csr_set)( H );					/* Set cursor status (On/Off) */
	ER		(*_lcd_csr_put)( H, H );				/* Set cursor position */
	ER		(*_lcd_csr_get)( H *, H *, H * );		/* Get cursor position */
	ER		(*_lcd_char)( H, H, UH, H );			/* Display 1 character */
	ER		(*_lcd_string)( H, H, UB *, H );		/* Display string */
	ER		(*_lcd_line)( H, H, H, H, H );			/* Draw/Erase line */
	ER		(*_lcd_cls)( void );					/* Clear screen */
	ER		(*_lcd_led)( H, H );					/* Set LED status (On/Off) */
	ER		(*_lcd_gaiji)( H, B * );				/* Change extra font */
	ER		(*_lcd_string2)( H, H, UB *, H );		/* Display string 2 */
	ER		(*_lcd_usrfont)( H, B * );				/* Set user font file */
	ER		(*_lcd_romfont)( void );				/* Set ROM font */
	ER		(*_lcd_userstr)( H, H, UB *, H );		/* Display user string */

/*************************************************/
/*		 Key BIOS					#004		 */
/*************************************************/
	ER		(*_key_read)( KEY_INP * );				/* Input 1 character */
	ER		(*_key_string)( KEY_INPS *, UB * ); 	/* Input string */
	ER		(*_key_num)( KEY_INPS *, UB * );		/* Input numeric data */
	ER		(*_key_check)( void );					/* Check keyboard buffer status */
	ER		(*_key_clear)( void );					/* Clear keyboard buffer */
	ER		(*_key_fnc)( UB, UB, KEYFORM * );		/* Set/Get code of functional key */
	ER		(*_key_fnc_mode)( UB, UB, ID *, UW *);	/* Set/Cancel notice status of functional key */

/*************************************************/
/*		 Communication BIOS 		#005		 */
/*************************************************/
	ER		(*_c_open)( H, UW, B *, H, TIM_TBL *, DEL_TBL *, B, B );
													/* Open COM port */
	ER		(*_c_close)( H );						/* Close COM port */
	ER		(*_c_status)( H );						/* Read status of COM port */
	ER		(*_c_hold)( H, B ); 					/* Occupy COM port */
	ER		(*_c_chkopen)( void );					/* Check COM port ststus */
	ER		(*_c_dout)( H, B *, H );				/* Send string */
	ER		(*_c_din)( H, B * );					/* Receive 1 character */
	ER		(*_c_tmdin)( H, B *, H );				/* Receive with watching time out */
	ER		(*_c_mdout)( H, B *, H );				/* Send memory block */
	ER		(*_c_mdin)( H, B *, UH ,H );			/* Receive memory block */
	ER		(*_c_out)( H, UB ); 					/* Send 1 character */
	ER		(*_c_break)( H, B );					/* Enable/Disable break character send */
	ER		(*_c_txrx)( H, B ); 					/* Enable/Disable send & receive */
	ER		(*_c_iobox)( H, B );					/* Set send mode of I/O box */
	ER		(*_c_irout)( H, B *, H );				/* Send data to I/O box */
	ER		(*_c_timer)( H, H, H, H );				/* Set time out value (DR/CS/CD) */
	ER		(*_c_rs)( H, B );						/* Set signal (RS) */
	ER		(*_c_er)( H, B );						/* Set signal (ER) */
	ER		(*_c_errs)( H, B ); 					/* Set signal (ER/RS) */
	ER		(*_c_flush)( H );						/* Clear receive buffer */
	ER		(*_c_bfsts)(	H, COM_STS * ); 		/* Check receive buffer status */
	ER		(*_c_errbfring)( H, B, UB );			/* Set error code buffering control mode */
	ER		(*_c_rderrsts)( H, UW * );				/* Read standard error status */
	ER		(*_c_chghdr)( H, B );					/* Change receive handler */

/*************************************************/
/*		 Power BIOS 				#006		 */
/*************************************************/
	ER		(*_pwr_inhabit)( UH, ID, UW );			/* Set/Reset notice mode */
	ER		(*_pwr_hold_apo)( UH, UW ); 			/* Set/Reset APO mode */
	ER		(*_pwr_off)( UH );						/* Command of power off */
	ER		(*_pwr_inhabit_clr)( ID, UW );			/* Clear notice event */
	
/*************************************************/
/*		 OBR BIOS					#007		 */
/*************************************************/ 																	
	ER		(*_OBR_open)( UW ); 					/* Activity directive of OBR BIOS */
	ER		(*_OBR_close)( void );					/* Close directive of OBR BIOS */
	UB		(*_OBR_getc)( UW * );					/* Read 1 character from OBR */
	ER		(*_OBR_gets)( UB *, UW *,UB * );		/* Read string from OBR */
	ER		(*_OBR_moderd)( M_TBL * );				/* Get operation mode of OBR */
	ER		(*_OBR_modewt)( M_TBL * );				/* Set operation mode of OBR */
	ER		(*_OBR_stat)( W *, UB * );				/* Check OBR buffer status */
	ER		(*_OBR_flush)( void );					/* Clear OBR buffer */
	ER		(*_OBR_gain)( UB ); 					/* Change receive gain of laser scanner */
	ER		(*_OBR_chgbuf)( UB );					/* Change OBR buffer */

/*************************************************/
/*		 Timer BIOS 				#008		 */
/*************************************************/
	void	(*_s_beep)( void ); 					/* Ring a error beep */
	ER		(*_s_sound)( UW, UW );					/* Ring a beep (Variable frequency) */
	ER		(*_s_dateget)( struct day_tabl * ); 	/* Get date */
	ER		(*_s_dateset)( struct day_tabl * ); 	/* Set date */
	ER		(*_s_timeget)( struct tim_tabl * ); 	/* Get time */
	ER		(*_s_timeset)( struct tim_tabl * ); 	/* Set time */
	ER		(*_s_settimer)( ID, UW, UW );			/* Set timer */
	ER		(*_s_timerend)( ER );					/* Cancel timer */
	ER		(*_s_settimer2)( ID, UW, UW );			/* Set timer 2 */
	ER		(*_s_timerend2)( ER );					/* Cancel timer 2 */
	
/*************************************************/
/*		 Communication utility		#031		 */
/*************************************************/
	ER		(*_cu_open)( H, H, DAT_COM_STR * ); 	/* Initialize COM port */
	ER		(*_cu_stopKeySet)( UB );				/* Set/Reset stop key */
	ER		(*_cu_fileSend)( H, UB, UB, UH, struct cu_file_info_form *,UB, UB );
													/* multi files send */
	ER		(*_cu_fileSendSet)( H, UB, UB, UH, W ); /* Set file send mode */
	ER		(*_cu_fileSend1)( H, struct cu_file_info_form *, UB, UB );
													/* Send 1 file */
	ER		(*_cu_fileRecv)( H, UB, UB , UH *, struct cu_file_info_form *,UB, UB );
													/* Multi files receive */
	ER		(*_cu_msgSend)( H, UB * );				/* Send disply message */
	ER		(*_cu_end)( H );						/* Cancel communication */
	ER		(*_cu_close)( H );						/* Close communication port */
	ER		(*_cu_readErrStat)( H, UW *, UW * );	/* Get error information */
	ER		(*_cu_readDlRjInfo)( H, UB * ); 		/* Get veto information of data link */

/*************************************************/
/*		 Common function			#052		 */
/*************************************************/
	void	(*_abort)( void );						/* Abort */
	void	(*_exit)( int );						/* Exit */
	void	(*_wkup_cost)( void );					/* Execute environment menu */
/*************************************************/
/*		 Library								 */
/*************************************************/
	size_t	(*_fread)( void *, size_t, size_t, FILE * );
													/* fread   */
	size_t	(*_fwrite)( void *, size_t, size_t, FILE * );
													/* fwrite  */
	void *	(*_calloc)( size_t, size_t );			/* calloc  */
	void *	(*_malloc)( size_t );					/* malloc  */
	void *	(*_realloc)( void *, size_t );			/* realloc */
	void	(*_free)( void * ); 					/* free    */
	int 	(*_fclose)( FILE * );					/* fclose  */

} BIOSJMPTBL1;

