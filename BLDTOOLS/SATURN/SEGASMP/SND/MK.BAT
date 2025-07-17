:	mk.bat -- PC 用 サンプル作成バッチファイル
:	Copyright(c) 1994 SEGA
:	Written by M.K on 1994-11-11 Ver.1.10
:	Modified by K.K on 1995-09-20
@echo off
if "%1"=="--" goto test
if not "%1"=="-?" goto job
    echo Usage: %0 [-option]
    echo  (ex.) %0 -debug
    goto exit
:job
    ..\..\sh\bin\shc -sub=shc.sub sddrvs.c >sddrvs.err
:exit
