rem @echo off
IF EXIST sega_int.lib del sega_int.lib
IF EXIST sega_int.h copy sega_int.h ..\include
del *.obj
shc -sub=int.shc int_main.c > int_main.err
rem asmsh int_trmp.src -cpu=7600
lbr /sub=int.lbr
dir *.err
copy sega_int.lib ..\lib
