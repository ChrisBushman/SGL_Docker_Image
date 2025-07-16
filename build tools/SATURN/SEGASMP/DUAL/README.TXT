
--- スレーブ CPU サンプルプログラムの README.txt

これらは、スレーブ CPU の使用方法のサンプルプログラムです。
多くのファイルは、ソフトウェア・ライブラリから引用しましたが、
スレーブ CPU が実行する行列演算モジュールは、意図的にマスタ CPU とは
別の名前をつけてあります。（slv_mtx3.src）
リエントラントなモジュール／関数の場合には、マスタ CPU とスレーブ CPU は
全く同様に実行することができます。

Makefile:	GNU make 用 makefile
slvsamp.abs:	実行ファイル
slvsamp.c:	メイン・モジュール
slvsamp.map:	このサンプルプログラムのマップファイル
slvsamp.sub:	このサンプルプログラムのリンク指定ファイル

slave.lib:	slaveSH.c から生成される初期化ライブラリ
slaveSH.c:	スレーブ CPU 初期化ライブラリ・メインモジュール
slaveSH.sub:	スレーブ CPU 初期化ライブラリ・リンク指定ファイル
slv_mtx3.h:	スレーブ CPU 用行列演算モジュール・ヘッダ
slv_mtx3.src:	スレーブ CPU 用行列演算モジュール

spr_mtx3.h:	マスタ CPU 用行列演算モジュール・ヘッダ
spr_mtx3.src:	マスタ CPU 用行列演算モジュール

smpstrt1.src:	SHC の実行環境初期化モジュール（１）
smpstrt2.c:	SHC の実行環境初期化モジュール（２）
sega_spr.h:	スプライト基本ライブラリヘッダ
sega_xpt.h:	共通ヘッダファイル
spr_fmat.src:	32 bit 固定小数点演算モジュール
spr_sinc.src:	SIN, COS 演算モジュール（１）
spr_xpt.c:	SIN, COS 演算モジュール（２）

dual.bat:DOS BAT file
