@echo off
IF EXIST sega_csh.lib del sega_csh.lib
IF EXIST sega_csh.h copy sega_csh.h ..\include
..\..\sh\bin\shc -sub=csh.shc csh_main.c >csh_main.err
..\..\sh\bin\lbr /sub=csh.lbr
copy sega_csh.lib ..\lib
dir *.err