@echo off
IF EXIST sega_bpl.h COPY sega_bpl.h ..\include
IF EXIST sega_bpl.lib DEL sega_bpl.lib
DEL *.obj
..\..\sh\bin\shc -sub=bpl.shc bpl_main.c>bpl_main.err
..\..\sh\bin\lbr /sub=bpl.lbr
DIR *.err
copy sega_bpl.lib ..\lib
