@echo off
IF EXIST sega_dbg.h copy sega_dbg.h ..\include
IF EXIST sega_dbg.lib del sega_dbg.lib
DEL *.obj
..\..\sh\bin\shc -sub=dbg.shc dbg_disp.c >dbg_disp.err
..\..\sh\bin\shc -sub=dbg.shc dbg_moni.c >dbg_moni.err
..\..\sh\bin\asmsh dbg_intr.src -cpu=7600 >dbg_inrt.err
..\..\sh\bin\lbr /sub=dbg.lbr

COPY sega_dbg.lib ..\lib
DIR *.err
