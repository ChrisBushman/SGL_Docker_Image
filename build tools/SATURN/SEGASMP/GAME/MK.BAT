:	mk.bat -- PC 用 サンプル作成バッチファイル
:	Copyright(c) 1994 SEGA
:	Written by M.K on 1994-11-11 Ver.1.20
:	Modified by K.K on 1995-09-20
:	Modified by A.H on 1997-07-03
@echo off
if "%1"=="lnk" goto lnk
if "%1"=="LNK" goto lnk
if "%1"=="clean" goto clean
if "%1"=="CLEAN" goto clean
if "%1"=="-?" goto help
if "%1"=="/?" goto help
goto job
:help
    echo Usage: %0 [-lnk, -clean, -?]
    echo  (ex.) %0 -lnk
    goto exit2
:clean
    @echo on
    deltree /Y sh.* *.obj *.lst *.err
    @echo off
    goto exit2
:job
if exist main.obj deltree /Y *.obj
shc -sub=shc.sub title.c>TITLE.ERR
IF ERRORLEVEL 1 GOTO error
shc -sub=shc.sub demo.c>DEMO.ERR
IF ERRORLEVEL 1 GOTO error
shc -sub=shc.sub logo.c>LOGO.ERR
IF ERRORLEVEL 1 GOTO error
shc -sub=shc.sub v_blank.c>V_BLANK.ERR
IF ERRORLEVEL 1 GOTO error
shc -sub=shc.sub polygon.c>POLYGON.ERR
IF ERRORLEVEL 1 GOTO error
shc -sub=shc.sub game_sub.c>GAME_SUB.ERR
IF ERRORLEVEL 1 GOTO error
shc -sub=shc.sub act.c>ACT.ERR
IF ERRORLEVEL 1 GOTO error
shc -sub=shc.sub game.c>GAME.ERR
IF ERRORLEVEL 1 GOTO error
shc -sub=shc.sub data.c>DATA.ERR
IF ERRORLEVEL 1 GOTO error
shc -sub=shc.sub main.c>MAIN.ERR
IF ERRORLEVEL 1 GOTO error
:lnk
lnk -sub=shlnk.sub
IF ERRORLEVEL 1 GOTO error
rof2bin -q sh.abs
  ECHO *
  ECHO COMPLETE!!
  GOTO exit
:error
    echo ERROR!!
:exit
	dir *.err
	dir sh.*
:exit2
