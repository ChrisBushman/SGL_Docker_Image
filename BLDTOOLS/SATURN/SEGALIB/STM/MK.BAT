@echo off
if exist sega_stm.lib del sega_stm.lib
if exist sega_stm.h copy sega_stm.h ..\include
del *.obj

shc stm_init.c -sub=stm.shc >stm_init.err
shc stm_svr.c  -sub=stm.shc >stm_svr.err
shc stm_trn.c  -sub=stm.shc >stm_trn.err
shc stm_acc.c  -sub=stm.shc >stm_acc.err
shc stm_loc.c  -sub=stm.shc >stm_loc.err
lbr -sub=stm.lbr > lbr.err
copy sega_stm.lib ..\lib
dir *.err