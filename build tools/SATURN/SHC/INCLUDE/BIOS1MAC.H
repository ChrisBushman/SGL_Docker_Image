/**********************************************************************/
/*																	  */
/*	PROGRA		   : Macro difine header 1 (function call)			  */
/*	SYSTEM		   : VER1.00										  */
/*	FILE		   : bios1mac.h 									  */
/*	VERSION 	   : 1.00											  */
/*	AUTHOR		   : CASIO COMPUTER CO.,LTD 						  */
/*	DATE OF GET    : 94/10/17										  */
/*	DATE OF UPDATE :												  */
/*	FUNCTION	   :												  */
/*	HISTORY 	   : 94/10/21 Communication utility addition		  */
/*																	  */
/* Copyright(C) 1995 CASIO COMPUTER CO.,LTD. All rights reserved.	  */
/**********************************************************************/

/****************************************************************************/
/*																			*/
/*		 Function call macro												*/
/*																			*/
/****************************************************************************/

#include	"BIOS1DEF.H"


extern	BIOSJMPTBL1 	*fnctbl1;

/*************************************************/
/*		 Data control				#002		 */
/*************************************************/
#define 	dat_system		fnctbl1->_dat_system	/* Read/Write system data */
#define 	dat_mem_size	fnctbl1->_dat_mem_size	/* Return free area size */
#define 	dat_fsize		fnctbl1->_dat_fsize 	/* Return free area size */
#define 	open			fnctbl1->_open			/* Open */
#define 	close			fnctbl1->_close 		/* Close */
#define 	read			fnctbl1->_read			/* Read */
#define 	write			fnctbl1->_write 		/* Write */
#define 	lseek			fnctbl1->_lseek 		/* Set read/write position */
#define 	sbrk			fnctbl1->_sbrk			/* Get memory */
#define 	dat_fdel		fnctbl1->_dat_fdel		/* Delete named file */
#define 	dat_fdir		fnctbl1->_dat_fdir		/* Get file information */

/*************************************************/
/*		 Display BIOS				#003		 */
/*************************************************/
#define 	lcd_csr_set 	fnctbl1->_lcd_csr_set	/* Set cursor status (On/Off) */
#define 	lcd_csr_put 	fnctbl1->_lcd_csr_put	/* Set cursor position */
#define 	lcd_csr_get 	fnctbl1->_lcd_csr_get	/* Get cursor position */
#define 	lcd_char		fnctbl1->_lcd_char		/* Display 1 character */
#define 	lcd_string		fnctbl1->_lcd_string	/* Display string */
#define 	lcd_line		fnctbl1->_lcd_line		/* Draw/Erase line */
#define 	lcd_cls 		fnctbl1->_lcd_cls		/* Clear screen */
#define 	lcd_led 		fnctbl1->_lcd_led		/* Set LED status (On/Off) */
#define 	lcd_gaiji		fnctbl1->_lcd_gaiji 	/* Change extra font */
#define 	lcd_string2 	fnctbl1->_lcd_string2	/* Display string 2 */
#define 	lcd_usrfont 	fnctbl1->_lcd_usrfont	/* Set user font file */
#define 	lcd_romfont 	fnctbl1->_lcd_romfont	/* Set ROM font */
#define 	lcd_userstr 	fnctbl1->_lcd_userstr	/* Display user string */

/*************************************************/
/*		 Key BIOS					#004		 */
/*************************************************/
#define 	key_read		fnctbl1->_key_read		/* Input 1 character */
#define 	key_string		fnctbl1->_key_string	/* Input string */
#define 	key_num 		fnctbl1->_key_num		/* Input numeric data */
#define 	key_check		fnctbl1->_key_check 	/* Check keyboard buffer status */
#define 	key_clear		fnctbl1->_key_clear 	/* Clear keyboard buffer */
#define 	key_fnc 		fnctbl1->_key_fnc		/* Set/Get code of functional key */
#define 	key_fnc_mode	fnctbl1->_key_fnc_mode	/* Set/Cancel notice status of functional key */

/*************************************************/
/*		 Communication BIOS 		#005		 */
/*************************************************/
#define 	c_open			fnctbl1->_c_open		/* Open COM port */
#define 	c_close 		fnctbl1->_c_close		/* Close COM port */
#define 	c_status		fnctbl1->_c_status		/* Read status of COM port */
#define 	c_hold			fnctbl1->_c_hold		/* Occupy COM port */
#define 	c_chkopen		fnctbl1->_c_chkopen 	/* Check COM port status */
#define 	c_dout			fnctbl1->_c_dout		/* Send string */
#define 	c_din			fnctbl1->_c_din 		/* Receive 1 character */
#define 	c_tmdin 		fnctbl1->_c_tmdin		/* Receive with watching time out */
#define 	c_mdout 		fnctbl1->_c_mdout		/* Send memory block */
#define 	c_mdin			fnctbl1->_c_mdin		/* Receive memory block */
#define 	c_out			fnctbl1->_c_out 		/* Send 1 character */
#define 	c_break 		fnctbl1->_c_break		/* Enable/Disable break character send */
#define 	c_txrx			fnctbl1->_c_txrx		/* Enable/Disable send & receive */
#define 	c_iobox 		fnctbl1->_c_iobox		/* Set send mode of I/O box */
#define 	c_irout 		fnctbl1->_c_irout		/* Send data to I/O box */
#define 	c_timer 		fnctbl1->_c_timer		/* Set time out value (DR/CS/CD) */
#define 	c_rs			fnctbl1->_c_rs			/* Set signal (RS) */
#define 	c_er			fnctbl1->_c_er			/* Set signal (ER) */
#define 	c_errs			fnctbl1->_c_errs		/* Set signal (ER/RS) */
#define 	c_flush 		fnctbl1->_c_flush		/* Clear receive buffer */
#define 	c_bfsts 		fnctbl1->_c_bfsts		/* Check receive buffer status */
#define 	c_errbfring 	fnctbl1->_c_errbfring	/* Set error code buffering control mode */
#define 	c_rderrsts		fnctbl1->_c_rderrsts	/* Read standard error status */
#define 	c_chghdr		fnctbl1->_c_chghdr		/* Change receive handler */

/*************************************************/
/*		 Power BIOS 				#006		 */
/*************************************************/
#define 	pwr_inhabit 	fnctbl1->_pwr_inhabit	/* Set notice mode */
#define 	pwr_hold_apo	fnctbl1->_pwr_hold_apo	/* Set APO mode */
#define 	pwr_off 		fnctbl1->_pwr_off		/* Command of power off */
#define 	pwr_inhabit_clr fnctbl1->_pwr_inhabit_clr
													/* Clear notice event */

/*************************************************/
/*		 OBR BIOS					#007		 */
/*************************************************/
#define 	OBR_open		fnctbl1->_OBR_open		/* Activity directive of OBR BIOS */
#define 	OBR_close		fnctbl1->_OBR_close 	/* Close directive of OBR BIOS */
#define 	OBR_getc		fnctbl1->_OBR_getc		/* Read 1 character from OBR */
#define 	OBR_gets		fnctbl1->_OBR_gets		/* Read string from OBR */
#define 	OBR_moderd		fnctbl1->_OBR_moderd	/* Get operation mode of OBR */
#define 	OBR_modewt		fnctbl1->_OBR_modewt	/* Set operation mode of OBR */
#define 	OBR_stat		fnctbl1->_OBR_stat		/* Check OBR buffer status */
#define 	OBR_flush		fnctbl1->_OBR_flush 	/* Clear OBR buffer */
#define 	OBR_gain		fnctbl1->_OBR_gain		/* Change receive gain of laser scanner */
#define 	OBR_chgbuf		fnctbl1->_OBR_chgbuf	/* Change buffer of OBR */

/*************************************************/
/*		 Timer BIOS 				#008		 */
/*************************************************/
#define 	s_beep			fnctbl1->_s_beep		/* Ring a error beep */
#define 	s_sound 		fnctbl1->_s_sound		/* Ring a beep (Variable frequency) */
#define 	s_dateget		fnctbl1->_s_dateget 	/* Get date */
#define 	s_dateset		fnctbl1->_s_dateset 	/* Set date */
#define 	s_timeget		fnctbl1->_s_timeget 	/* Get time */
#define 	s_timeset		fnctbl1->_s_timeset 	/* Set time */
#define 	s_settimer		fnctbl1->_s_settimer	/* Set timer */
#define 	s_timerend		fnctbl1->_s_timerend	/* Cancel timer */
#define 	s_settimer2 	fnctbl1->_s_settimer2	/* Set timer 2 */
#define 	s_timerend2 	fnctbl1->_s_timerend2	/* Cancel timer 2 */

/*************************************************/
/*		 Communication utility		#031		 */
/*************************************************/
#define 	cu_open 		fnctbl1->_cu_open		/* Initialize COM port */
#define 	cu_stopKeySet	fnctbl1->_cu_stopKeySet /* Set/Reset stop key */
#define 	cu_fileSend 	fnctbl1->_cu_fileSend	/* Multi files send */
#define 	cu_fileSendSet	fnctbl1->_cu_fileSendSet
													/* Set file send mode */
#define 	cu_fileSend1	fnctbl1->_cu_fileSend1	/* Send 1 file */
#define 	cu_fileRecv 	fnctbl1->_cu_fileRecv	/* Multi files receive */
#define 	cu_msgSend		fnctbl1->_cu_msgSend	/* Send disply message */
#define 	cu_end			fnctbl1->_cu_end		/* Cancel communication */
#define 	cu_close		fnctbl1->_cu_close		/* Close communication port */
#define 	cu_readErrStat	fnctbl1->_cu_readErrStat
													/* Get error information */
#define 	cu_readDlRjInfo fnctbl1->_cu_readDlRjInfo
													/* Get veto information of data link */

/*************************************************/
/*		 Common function		  #052			 */
/*************************************************/

#define 	abort			fnctbl1->_abort 		/* Abort */
#define 	exit			fnctbl1->_exit			/* Exit */
#define 	wkup_cost		fnctbl1->_wkup_cost 	/* Execute environment menu */

/*************************************************/
/*		 Library								 */
/*************************************************/
#define 	fread			fnctbl1->_fread 		/* fread   */
#define 	fwrite			fnctbl1->_fwrite		/* fwrite  */
#define 	calloc			fnctbl1->_calloc		/* calloc  */
#define 	malloc			fnctbl1->_malloc		/* malloc  */
#define 	realloc 		fnctbl1->_realloc		/* realloc */
#define 	free			fnctbl1->_free			/* free    */
#define 	fclose			fnctbl1->_fclose		/* fclose  */

