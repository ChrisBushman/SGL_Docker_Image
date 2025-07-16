:	mk.bat -- PC 用 サンプル作成バッチファイル
:	Copyright(c) 1994 SEGA
:	Written by M.K on 1994-11-11 Ver.1.21
:	Modified by K.K on 1995-09-20
:	Modified by A.H on 1997-07-03
:	Modified by A.H on 1997-07-29
@echo off
if "%1"=="" goto job
if "%1"=="lnk" goto lnk
if "%1"=="LNK" goto lnk
if "%1"=="clean" goto clean
if "%1"=="CLEAN" goto clean
if "%1"=="?" goto help
if "%1"=="H" goto help
if "%1"=="h" goto help
goto job

:help
    echo Usage: %0 [lnk, clean, ?]
    echo  (ex.) %0 lnk
    goto exit2

:clean
    @echo on
    deltree /Y sh.* *.obj *.lst *.err
    @echo off
    goto exit2

:job
if exist main.obj deltree /Y *.obj
del sh.abs

shc -sub=shc.sub main.c>main.err
IF ERRORLEVEL 1 GOTO error
shc -sub=shc.sub smperm11.c>smperm11.err
IF ERRORLEVEL 1 GOTO error
shc -sub=shc.sub smperm12.c>smperm12.err
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