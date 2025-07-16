:	mk.bat -- PC 用 サンプル作成バッチファイル
:	Copyright(c) 1994 SEGA
:	Written by M.K on 1994-11-11 Ver.1.10
:	Modified by K.K on 1995-09-20
:	Modified by A.H on 1997-03-18
@echo off
if "%1"=="lnk" goto lnk
if "%1"=="LNK" goto lnk
if "%1"=="clean" goto clean
if "%1"=="CLEAN" goto clean
if "%1"=="-?" goto help
if "%1"=="/?" goto help
goto job
:help
    echo Usage: %0 [-option]
    echo  (ex.) %0 -debug
    goto exit
:clean
    deltree /Y sh.* *.obj *.lst *.err
    goto exit2
:job
if exist main.obj deltree /Y *.obj
..\..\..\sh\bin\shc -sub=shc.sub main.c >main.err
..\..\..\sh\bin\shc -sub=shc.sub sddrvs.c >sddrvs.err
IF ERRORLEVEL 1 GOTO error
:lnk
..\..\..\sh\bin\lnk -sub=shlnk.sub >shlnk.err
IF ERRORLEVEL 1 GOTO error
..\..\..\sh\bin\rof2bin -q sh.abs
    ECHO *
  ECHO COMPLETE!!
  GOTO exit
:error
    echo ERROR!!
:exit
	dir *.err
	dir *.bin
:exit2
