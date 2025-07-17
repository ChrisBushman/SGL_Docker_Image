@echo off
echo ---------------------------------------------------------------------------
echo   main.bat -- PC 用 サンプル作成コマンドファイル
echo   Copyright(c) 1994 SEGA
echo   Written by H.E on 1994-02-15 Ver.0.80
echo   Updated by K.M on 1994-05-10 Ver.0.80
echo ---------------------------------------------------------------------------

IF NOT EXIST sega_wrn.da snddata
IF EXIST game.abs del game.abs
del *.obj
shc -sub=mk.cfg title.c >title.err
shc -sub=mk.cfg demo.c >demo.err
shc -sub=mk.cfg logo.c >logo.err
shc -sub=mk.cfg v_blank.c >v_blank.err
shc -sub=mk.cfg polygon.c >polygon.err
shc -sub=mk.cfg game_sub.c >game.sub.err
shc -sub=mk.cfg act.c >act.err
shc -sub=mk.cfg game.c >game.err
shc -sub=mk.cfg data.c >data.err
shc -sub=mk.cfg cd_ctrl.c >cd_ctrl.err
shc -sub=mk.cfg sequence.c >sequence.err
lnk /sub=shlnk.sub
rof2bin -q game.abs
echo *
rem end of file
