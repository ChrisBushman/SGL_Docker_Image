@echo off
IF EXIST sega_mth.lib del sega_mth.lib
IF EXIST sega_mth.h copy sega_mth.h ..\include
del *.obj
..\..\sh\bin\shc -sub=mth.shc mth_curv.c >mth_curv.err
..\..\sh\bin\shc -sub=mth.shc mth_dsps.c >mth_dsps.err
..\..\sh\bin\shc -sub=mth.shc mth_rand.c >mth_rand.err
..\..\sh\bin\asmsh mth_fixd.src -cpu=7600 >mth_fixd.err
..\..\sh\bin\asmsh mth_tri.src -cpu=7600 >mth_tri.err
..\..\sh\bin\asmsh mth_mtrx.src -cpu=7600 >mth_mtrx.err
..\..\sh\bin\asmsh mth_ps2d.src -cpu=7600 >mth_ps2d.err
..\..\sh\bin\asmsh mth_norm.src -cpu=7600 >mth_norm.err
..\..\sh\bin\lbr /sub=mth.lbr
copy sega_mth.lib ..\lib
dir *.err
