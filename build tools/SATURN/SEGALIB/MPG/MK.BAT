@echo off
if exist sega_mpg.lib del sega_mpg.lib
if exist sega_mpg.h copy sega_mpg.h ..\include
del *.obj

shc mpg_sys.c  -sub=mpg.shc >mpg_sys.err
shc mpg_mve.c  -sub=mpg.shc >mpg_mve.err
shc mpg_mve2.c -sub=mpg.shc >mpg_mve2.err
shc mpg_spc.c  -sub=mpg.shc >mpg_spc.err
shc mpg_win.c  -sub=mpg.shc >mpg_win.err
lbr -sub=mpg.lbr
copy sega_mpg.lib ..\lib
dir *.err