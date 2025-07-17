/*----------------------------------------------------------------------*/
/*  Pad Control + スプライトカラー演算 + ラインカラー画面               */
/*  元サンプル：sample_9_1                                              */
/*                                                                      */
/*  修正者：A.H                                                         */
/*  修正日：97/04/07                                                    */
/*                                                                      */
/*  追加内容：スプライト毎に異なった割合にて、カラー演算を行うサンプル  */
/*                                                                      */
/*                                                                      */
/*  内容：文字表示エリアの周辺のみを、カラー演算を使用して              */
/*        輝度を落とし、文字を見やすくするサンプル および、             */
/*        スプライト毎に演算割合を設定するサンプル。                    */
/*                                                                      */
/*  機能１：                                                            */
/*         NBG1 に対し、ラインカラー画面とのカラー演算を指定します．    */
/*         カラー演算の範囲を TV画面中央部分に限定する為、              */
/*         ノーマル矩形ウィンドウを併用しています．                     */
/*         演算割合は、上下キーによって変更可能です。                   */
/*                                                                      */
/*  機能２：                                                            */
/*         スプライトと NBG1とを、カラー演算します。                    */
/*         スプライト毎にアトリビュートの値を個別に設定し、             */
/*         演算割合を個別に設定しています。                             */
/*         スプライトカラー演算の条件として、特定のプライオリティ値     */
/*         のスプライトのみに設定しています。                           */
/*                                                                      */
/*                                                                      */
/*  表示画面：  文字表示:NBG0                                           */
/*              指カーソル:スプライト(パレットコード)                   */
/*              ＰＡＤの背景:NBG1                                       */
/*              バックカラー面をオレンジ色に設定                        */
/*                                                                      */
/*  特殊画面：  黒色合成用面:ラインカラー画面                           */
/*              カラー演算範囲指定：矩形ウィンドウ                      */
/*                                                                      */
/*  補足：カラー演算の設定は、次のように設定しています．                */
/*        ・演算割合による合成                                          */
/*            セカンド画像側で指定(ラインカラー使用時)                  */
/*            トップ画像側で指定(スプライトに対してカラー演算使用時)    */
/*                                                                      */
/*                                                                      */
/*                                                                      */
/*  操作方法：全てのボタン→対応するスプライト(指カーソル)を表示        */
/*            Ａボタン  演算割合   CLRate22_10   約７０％               */
/*            Ｂボタン  演算割合   CLRate15_17     ５０％               */
/*            Ｃボタン  演算割合   CLRate6_26    約２０％弱             */
/*            その他のボタン  通常のスプライト表示（カラー演算なし）    */
/*                                                                      */
/*            スタートボタン：再初期化                                  */
/*                                                                      */
/*            上下キー：ラインカラー画面のカラー演算の割合を変更        */
/*            Ｘボタン：機能切換                                        */
/*                          ラインカラー,カラー演算ウィンドウの有効     */
/*                          スプライトと BG とのカラー演算              */
/*            Ｙボタン：全てのカラー演算を無効                          */
/*            Ｚボタン：Ｙボタンによる無効を解除                        */
/*                                                                      */
/*                                                                      */
/*  補足事項：ラインカラー画面は、複数の画面に同時に設定可能です。      */
/*            ラインカラー画面は、バック画面に対しての指定は出来ません。*/
/*            RGBコードのスプライトは、個別に演算割合を設定出来ません。 */
/*                                                                      */
/*----------------------------------------------------------------------*/
#include    "sgl.h"

#define     NBG1_CEL_ADR        ( VDP2_VRAM_B1 + 0x02000 )
#define     NBG1_MAP_ADR        ( VDP2_VRAM_B1 + 0x12000 )
#define     NBG1_COL_ADR        ( VDP2_COLRAM + 0x00200 )
#define     BACK_COL_ADR        ( VDP2_VRAM_A1 + 0x1fffe )
#define     PAD_NUM         13   /*  デジタルPAD のボタン数 */

/*  Add 96/07/18   スプライトをパレット形式で表示する為に追加  */
#define     SPR_COL_ADR     ( VDP2_COLRAM + 0x00400 )
/*  Add 96/07/18 */

/*  Add 97/03/25   カラー演算ウィンドウの設定追加  */
#define    WIN0_LEFT   50    /* カラー演算ウィンドウ領域   左限 */
#define    WIN0_TOP    70    /* カラー演算ウィンドウ領域   上限 */
#define    WIN0_RIGHT  270   /* カラー演算ウィンドウ領域   右限 */
#define    WIN0_BOTTOM 140   /* カラー演算ウィンドウ領域   下限 */

#define    LINE_COR_TABLE_ADR  0x25E7FFFE
        /* ラインカラーテーブル格納アドレス */
#define    LINE_COR_TABLE_COR_CODE    00
        /* ラインカラー画面のパレットコード */

#define    DEFAULT_ccRATE  0x0f   /* ラインカラー演算割合初期値  50%  */
/*  Add 97/03/25  */


static Uint16 pad_asign[] = {
    PER_DGT_KU,
    PER_DGT_KD,
    PER_DGT_KR,
    PER_DGT_KL,
    PER_DGT_TA,
    PER_DGT_TB,
    PER_DGT_TC,
    PER_DGT_ST,
    PER_DGT_TX,
    PER_DGT_TY,
    PER_DGT_TZ,
    PER_DGT_TR,
    PER_DGT_TL,
};

extern Uint8 pad_cel[];
extern Uint16 pad_map[];
extern Uint16 pad_pal[];
extern TEXTURE tex_spr[];
extern PICTURE pic_spr[];
extern FIXED stat[][XYZS];
extern SPR_ATTR attr[];
extern ANGLE angz[];

extern void Cel2VRAM(Uint8 *  , void * , Uint32 ) ;
extern void Map2VRAM(Uint16 * , void * , Uint16 , Uint16 , Uint16 , Uint32 ) ;
extern void Pal2CRAM(Uint16 * , void * , Uint32 ) ;

/*********  Add 96/07/18 **************/
extern spr_pal[];
/** End **  Add 96/07/18 **************/

static void set_sprite(PICTURE *pcptr , Uint32 NbPicture)
{
    TEXTURE *txptr;
 
    for(; NbPicture-- > 0; pcptr++){
        txptr = tex_spr + pcptr->texno;
        slDMACopy((void *)pcptr->pcsrc,
            (void *)(SpriteVRAM + ((txptr->CGadr) << 3)),
            (Uint32)((txptr->Hsize * txptr->Vsize * 4) >> (pcptr->cmode)));
    }
}

static void disp_sprite()
{
    static Sint32 i;
    Uint16 data;

    if(!Per_Connect1) return;
    data = Smpc_Peripheral[0].data;

    for(i=0;i<PAD_NUM;i++){
        if((data & pad_asign[i])==0){
            slDispSprite((FIXED *)stat[i],
                (SPR_ATTR *)(&attr[i].texno),(ANGLE)angz[i]);
        }
    }
}

static Uint16 ccRate = DEFAULT_ccRATE;

void init_moji_window(void)
{
  extern  Uint16*    VDP2_LCTA;  /* ラインカラー画面テーブルアドレス */
                                  /* システム変数を使用 */
  VDP2_LCTA = (Uint16 *)((LINE_COR_TABLE_ADR - VDP2_VRAM_A0) / 2);
    /*  ラインカラーテーブルアドレスの設定(単色設定) */

  *(Uint16 *)LINE_COR_TABLE_ADR = LINE_COR_TABLE_COR_CODE;
    /*  ラインカラーのカラーコードの設定 */

  slScrWindow0(WIN0_LEFT, WIN0_TOP, WIN0_RIGHT, WIN0_BOTTOM);
  slScrWindowMode(scnCCAL , win0_IN);

#if 0 /* ここを復活させると、スプライトもラインカラーで演算されます。 */
    slSpriteCCalcCond(CC_pr_CN );  /*  スプライトカラー演算条件 */
/*     CC_Condition
            CC_pr_CN : ( Priority <= 演算条件ナンバー )
            CC_PR_CN : ( Priority == 演算条件ナンバー )
            CC_PR_cn : ( Priority >= 演算条件ナンバー )
            CC_MSB   : ( MSB of ColorData == 1)
*/

    slSpriteCCalcNum(7);  /*  スプライトカラー演算条件ナンバー  */
  slColorCalc(CC_RATE|CC_2ND|NBG1ON|NBG2ON|NBG3ON|RBG0ON|SPRON);
        /*            ↑      ↑   ^^^^^^^^^^^^↑^^^^^^^^^^^^^^^^^^^  */
        /*          割合を、セカンド側で指定、 演算する画面 の指定    */
  slLineColDisp((Uint16)NBG1ON|NBG2ON|NBG3ON|RBG0ON|LNCLON);
    /*  !!注意  'LNCLON' は、スプライト画面の指定 */
#else

  slColorCalc(CC_RATE|CC_2ND|NBG1ON|NBG2ON|NBG3ON|RBG0ON);
  ccRate = DEFAULT_ccRATE;
  slLineColDisp((Uint16)NBG1ON|NBG2ON|NBG3ON|RBG0ON);
#endif
}


void control_moji_window_cc(void)
{

  slPrint("LINECOLOR ON            " , slLocate(9,10));
  slPrint("                        " , slLocate(9,12));
  slPrint("                        " , slLocate(9,13));
  slPrint("UP:   ccRATE++          " , slLocate(9,14));
  slPrint("DOWN: ccRATE--          " , slLocate(9,15));

  if(!(Smpc_Peripheral[0].push & PER_DGT_KU)) {
    if(ccRate == 31) ccRate = 0;
    else ccRate++;
  }
  if(!(Smpc_Peripheral[0].push & PER_DGT_KD)) {
    if(ccRate == 0 ) ccRate = 31;
    else ccRate--;
  }
  slColRateLNCL(ccRate);

}
void ss_main(void)
{
  Uint8 cc_window_ena = ON;

    slInitSystem(TV_320x224,tex_spr,1);
    slTVOff();
    set_sprite(pic_spr,1);
    slPrint("Sample program 9.1" , slLocate(9,2));
    slPrint("         +"         , slLocate(9,3));
    slPrint("Sprite ColorCalc & LINECOLOR"  , slLocate(3,4));
    slPrint("                   (with Window)"  , slLocate(3,5));
    slPrint("START   :RESET(RESTART)", slLocate(9,24));
    slPrint("BUTTON X:TOGLE LINECOLOR", slLocate(9,25));
    slPrint("BUTTON Y:OFF COLOR_CALC", slLocate(9,26));
    slPrint("BUTTON Z:ON  COLOR_CALC", slLocate(9,27));

    slColRAMMode(CRM16_1024);
    slBack1ColSet((void *)BACK_COL_ADR , 0x50F);

    slCharNbg1(COL_TYPE_256 , CHAR_SIZE_1x1);
    slPageNbg1((void *)NBG1_CEL_ADR , 0 , PNB_1WORD|CN_12BIT);
    slPlaneNbg1(PL_SIZE_1x1);
    slMapNbg1((void *)NBG1_MAP_ADR , (void *)NBG1_MAP_ADR , (void *)NBG1_MAP_ADR , (void *)NBG1_MAP_ADR);
    Cel2VRAM((Uint8 *)pad_cel , (void *)NBG1_CEL_ADR , 483*64);
    Map2VRAM((Uint16 *)pad_map , (void *)NBG1_MAP_ADR , 32 , 19 , 1 , 256);
    Pal2CRAM((Uint16 *)pad_pal , (void *)NBG1_COL_ADR , 256);

    slScrPosNbg1(toFIXED(-32.0) , toFIXED(-36.0));
    slScrAutoDisp(NBG0ON | NBG1ON);
    slTVOn();

/* ---- 960718  added ----------------------------------------------------- */
    Pal2CRAM( (Uint16 *)spr_pal , (void *)SPR_COL_ADR , 256);

    slPriority(scnSPR0 , 7); /* カラー演算を行わないスプライト */
    slPriority(scnSPR1 , 4); /* カラー演算を行うスプライト */
    slPriority(scnNBG1 , 2); /* PADの背景 */

    slSpriteCCalcCond(CC_PR_CN );
/*
     Condition  CC_pr_CN , CC_PR_CN , CC_PR_cn , CC_MSB
       CC_pr_CN : ( Priority <= ConditionNumber )
       CC_PR_CN : ( Priority == ConditionNumber )
       CC_PR_cn : ( Priority >= ConditionNumber )
       CC_MSB   : ( MSB of ColorData == 1)
*/
    slSpriteCCalcNum(4);  /*  ConditionNumber  */


  slColRate(scnSPR1  , CLRate22_10 );  /*  BUTTON A */
  slColRate(scnSPR2  , CLRate15_17 );  /*  BUTTON B */
  slColRate(scnSPR3  , CLRate6_26 );  /*  BUTTON C */
/*  
   scrn : scnNBG0 , scnNBG1 , scnNBG2 , scnNBG3 , scnRBG0 , scnLNCL , scnBACK
          scnSPR0 , scnSPR1 , scnSPR2 , scnSPR3 , scnSPR4 , scnSPR5 ,
          scnSPR6 , scnSPR7

   rate : CLRate31_1 ～ CLRate0_32
           rate      Top Screen : Second Screen
         CLRate31_1         31  :  1
         CLRate30_2         32  :  2
                :
         CLRate1_31          1  :  31
         CLRate0_32          0  :  32
*/

  slSpriteType(3);  /* スプライトタイプ３に設定 */
#if 0
    slColorCalc( CC_RATE|CC_TOP|SPRON );  /* カラー演算の設定 */
       /*  演算割合を使用、トップ画像側、スプライト画面 */
#endif

/* END - 96/07/18  added -------------------------------------------- */
  init_moji_window();
    while(1) {
        disp_sprite();

        /* ラインカラーのON/OFF */
    if(!(Smpc_Peripheral[0].push & PER_DGT_TX)) {
      if(cc_window_ena == OFF){
        cc_window_ena = ON;
        init_moji_window(); /* カラー演算ウィンドウ ＯＮ */
/*        slLineColDisp((Uint16)NBG1ON|NBG2ON|NBG3ON|RBG0ON|LNCLON); */
            /* ラインカラー ON */
      }else{
        cc_window_ena = OFF;
          slColorCalc( CC_RATE|CC_TOP|SPRON );  /* カラー演算の設定 */
             /*  演算割合を使用、トップ画像側、スプライト画面 */
        slScrWindowMode(scnCCAL ,(Uint16)NULL);
            /* カラー演算ウィンドウ OFF */
        slLineColDisp((Uint16)NULL);
            /* ラインカラー OFF */
        slPrint("LINECOLOR OFF           " , slLocate(9,10));
          slPrint("BUTTON A:CLRate22_10    " , slLocate(9,12));
          slPrint("BUTTON B:CLRate15_17    " , slLocate(9,13));
          slPrint("BUTTON C:CLRate6_26     " , slLocate(9,14));
          slPrint("OTHERS  :NO COLOR_CALC  " , slLocate(9,15));
      }
    }

    /* カラー演算 ON */
    if(!(Smpc_Peripheral[0].push & PER_DGT_TZ)) {
      if(cc_window_ena == OFF){
        slColorCalc(CC_RATE|CC_TOP|NBG1ON|NBG2ON|NBG3ON|RBG0ON|SPRON);
      }else
        slColorCalc(CC_RATE|CC_2ND|NBG1ON|NBG2ON|NBG3ON|RBG0ON);
    }

    /* 全てのカラー演算をキャンセル */
    if(!(Smpc_Peripheral[0].push & PER_DGT_TY)) {
      slColorCalc((Uint32)NULL);
    }
#if 0    /*拡張カラー演算のテスト */
    if(!(Smpc_Peripheral[0].push & PER_DGT_TL)) {
      slLineColDisp((Uint16)NULL); /* ラインカラーをキャンセル */
      slColorCalc(CC_RATE|CC_TOP|CC_EXT|NBG0ON|SPRON); /* 拡張カラー演算登録 */
/*      slColorCalc(CC_EXT|NBG0ON|SPRON); */
      slColRateNbg0(ccRate);
    }
#endif
    if(cc_window_ena == ON)
          control_moji_window_cc();
    if(!(Smpc_Peripheral[0].push & PER_DGT_ST))
      init_moji_window();
        slSynch();
    } 
}

