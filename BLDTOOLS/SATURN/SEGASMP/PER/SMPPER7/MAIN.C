/****************************************
*			include files				*
****************************************/
#define	_SPR2_
#include	"sega_spr.h"

#include	"machine.h"
#include	"sega_int.h"
#include	"sega_scl.h"
#include	"sega_dbg.h"
#include	"sega_per.h"

#include	"per_x.h"
#include	"image.h"

#if 0
extern PerGetPer *tmp_per_data;  /*  lib test  */
#endif
extern PerMulInfo	*mul_info;   /*  lib test  */


/****************************************
*		declare private objects			*
****************************************/
static SysPort	*__port;

/****************************************
*			declare functions			*
****************************************/
void	_spr2_transfercommand( void );
void	_spr2_transferimage( void	*image, Uint32	size );
void	_spr2_initialize( void );
void	_spr2_setspritecoord( int	x, int	y );
void	_spr2_getspritecoord( int	*x, int	*y );
void	move_cursor( const SysDevice	*device, int	*x, int	*y );
void	display_device_info( const SysDevice	*device, int	x, int	y );

static void	smpper7( const SysPort	*port ){
	int		x,y;
	
	_spr2_getspritecoord( &x, &y );
	DBG_DisplayOn();
	while(1){
		const SysDevice	*first_device = NULL;
		Uint32		m,n;
		
		for( m = 0; m < _MAX_PORT; m++ ){
			int		x = 1;
			int		y = m*(_MAX_PERIPHERAL+2)+1;
			
			DBG_SetCursol( x, y );
			DBG_Printf( "PORT%d    %02X ", m, port[m].id );
			y++;
			
			for( n = 0; n < _MAX_PERIPHERAL; n++ ){
				const SysDevice	*device = PER_GetDeviceA( &port[m], n );
				
				if( first_device == NULL )
					first_device = device;
				
				display_device_info( device, x, y+n );
				if( device != NULL && PER_GetType( device ) == 0x20 ){
					int     dx, dy;
					PER_GetPointerXY( (SysDevPointer *)device, &dx, &dy );
					DBG_Printf( " %d,%d   ", dx, dy);
				}
			}
		}
#if 0
		{
			Uint8 *u8per_data;

			int	idx_con, idx_data;
			u8per_data = (Uint8 *)tmp_per_data;
			for ( idx_con = 0; idx_con < 7; idx_con++){
				DBG_SetCursol( 2, 16 + idx_con );
				DBG_Printf( "%02X : ", idx_con+1 );
				for( idx_data =0; idx_data < 8; idx_data++){
					DBG_Printf( "%02X ", *u8per_data );
					u8per_data++;
				}
			}
			DBG_SetCursol( 2, 24 );
			DBG_Printf( "mul_info ADDR:%08X %02X %02X %02X %02X"
			, mul_info, mul_info[0].id, mul_info[0].con, mul_info[1].id, mul_info[1].con );
			DBG_SetCursol( 2, 25 );
			DBG_Printf( "per_data ADDR:%08X", tmp_per_data );

		}
#endif
		if( first_device != NULL ){
			move_cursor( first_device, &x, &y );
			_spr2_setspritecoord( x, y );
			_spr2_transfercommand();
		}
		
		SCL_DisplayFrame();  /* フレームバッファの切り替え */
	}
}

static void	_intr_v_blank_in( void ){
	SCL_VblankStart();
}

static void	_intr_v_blank_out( void ){
	SCL_VblankEnd();
	
	PER_GetPort( __port );
}

int	main( void ){
	Uint16	dummy_pad = ~0;
	
	__port = PER_OpenPort();
	
	SCL_Vdp2Init();
	SCL_SetPriority(SCL_SP0|SCL_SP1|SCL_SP2|SCL_SP3|SCL_SP4|SCL_SP5|SCL_SP6|SCL_SP7,7);
	SCL_SetSpriteMode(SCL_TYPE1,SCL_MIX,SCL_SP_WINDOW);
	SCL_SetColRamMode(SCL_CRM15_2048);
	
	set_imask(15);

	INT_ChgMsk( INT_MSK_NULL, INT_MSK_VBLK_IN|INT_MSK_VBLK_OUT );
	INT_SetScuFunc( INT_SCU_VBLK_IN,  _intr_v_blank_in  );
	INT_SetScuFunc( INT_SCU_VBLK_OUT, _intr_v_blank_out );
	INT_ChgMsk( INT_MSK_VBLK_IN|INT_MSK_VBLK_OUT, INT_MSK_NULL );
	
	set_imask(0);
	
	_spr2_initialize();
	_spr2_transfercommand();
	_spr2_transferimage( cursor_image, sizeof( cursor_image ));
	
	DBG_Initial( &dummy_pad, RGB16_COLOR(31,31,31), 0 );
	
	SCL_SetFrameInterval( 1 );
	
	smpper7( __port );
	
	return	EXIT_SUCCESS;
}
