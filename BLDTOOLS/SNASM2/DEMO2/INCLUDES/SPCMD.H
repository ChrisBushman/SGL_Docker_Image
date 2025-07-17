;
; GLOBAL DEFINES/MACROS DEFINES
;

CTRL_END            equ	$8000     ;* control word end bit mask        

JUMP_NEXT           equ	$0000     ; jump next command                
JUMP_ASSIGN         equ	$1000     ; jump assign command              
JUMP_CALL           equ	$2000     ; call assign command              
JUMP_RETURN         equ	$3000     ; return command subroutine        

SKIP_NEXT           equ	$4000     ; skip next command                
SKIP_ASSIGN         equ	$5000     ; skip assign command              
SKIP_CALL           equ	$6000     ; call assign command              
SKIP_RETURN         equ	$7000     ; return command subroutine        

ZOOM_NOPOINT        equ	$0000     ; zoom no point                    
ZOOM_TL             equ	$0500     ; zoom point top left              
ZOOM_TM             equ	$0600     ; zoom point top middle            
ZOOM_TR             equ	$0700     ; zoom point top right             
ZOOM_ML             equ	$0900     ; zoom point middle left           
ZOOM_MM             equ	$0a00     ; zoom point center                
ZOOM_MR             equ	$0b00     ; zoom point middle right          
ZOOM_BL             equ	$0c00     ; zoom point bottom left           
ZOOM_BM             equ	$0e00     ; zoom point bottom middle         
ZOOM_BR             equ	$0f00     ; zoom point bottom right          

DIR_NOREV           equ	$0000     ; char read not reverse            
DIR_TBREV           equ	$0020     ; char read top/bottom reverse     
DIR_LRREV           equ	$0010     ; char read left/right reverse     
DIR_LRTBREV         equ	$0030     ; char read left/right/top/bot rev 

FUNC_NORMALSP       equ	$0000     ; draw normal sprite function      
FUNC_SCALESP        equ	$0001     ; draw scaled sprite function      
FUNC_DISTORSP       equ	$0002     ; draw distorted sprite function   
FUNC_POLYGON        equ	$0004     ; draw polygon function            
FUNC_POLYLINE       equ	$0005     ; draw polyline function           
FUNC_LINE           equ	$0006     ; draw line function               
FUNC_SCLIP          equ	$0009     ; set system clipping function     
FUNC_UCLIP          equ	$0008     ; set user clipping function       
FUNC_LCOORD         equ	$000a     ; set local coordinate function    
FUNC_TEXTURE        equ	$0004     ; texture command group mask       

UCLIP_EXTER         equ	$0400     ; Clip=1/Cmod=0 interior plot 
UCLIP_INTER         equ	$0600     ; Clip=1/Cmod=1 exterior plot 
MESH_ENABLE         equ	$0100     ; plot after mesh processing 
ECD_DISABLE         equ	$0080     ; ECD disabe & SPD enable          
SPD_DISABLE         equ	$0040     ; ECD enable & SPD disable         
ECDSPD_DISABLE      equ	$00c0     ; ECD & SPD disable                

COLOR_0             equ	$0000     ; 4 bit/pixel & 16 color bank mode 
COLOR_1             equ	$0008     ; 4 bit/pixel & 16 color lookup tbl
COLOR_2             equ	$0010     ; 8 bit/pixel &  64 color bank mode
COLOR_3             equ	$0018     ; 8 bit/pixel & 128 color bank mode
COLOR_4             equ	$0020     ; 8 bit/pixel & 256 color bank mode
COLOR_5             equ	$0028     ; 16 bit/pixel & RGB mode          

CTRL_SKIP           equ	$4000     ; control word skip bit mask       
CTRL_JUMP           equ	$3000     ; control word jump bit mask       
CTRL_ZOOM           equ	$0f00     ; control word zoom point bit mask 
CTRL_FUNC           equ	$000f     ; control word function bit mask   
CTRL_DIR            equ	$0030     ; control word char read bit mask  

DRAW_COMPO          equ	$0003     ; draw mode word color compose mask
DRAW_GOURAU         equ	$0004     ; draw mode word gouraud shading   
DRAW_GOURAU_GRAY    equ	$0804     ; draw mode word gray gour shading 
DRAW_COLOR          equ	$0038     ; draw mode word color mode mask   
DRAW_MESH           equ	$0100     ; draw mode word mesh on           

COMPO_REP           equ	$0000     ; color compose reprace mode       
COMPO_SHADOW        equ	$0001     ; color compose shadow mode        
COMPO_HARF          equ	$0002     ; color compose harf luminance mode
COMPO_TRANS         equ	$0003     ; color compose trunslucent mode   

NO_GOUR             equ	$ffff     ; no use Gouraud Shading Table No. 


