@echo off
IF EXIST sega_spr.h copy sega_spr.h ..\include
IF EXIST sega_spr.lib del sega_spr.lib
DEL *.obj
..\..\sh\bin\shc -sub=spr.shc spr_slv.c >spr_slv.err
..\..\sh\bin\shc -sub=spr.shc spr_1c.c >spr_1c.err
..\..\sh\bin\shc -sub=spr.shc spr_2c.c >spr_2c.err
..\..\sh\bin\shc -sub=spr.shc spr_3c0.c >spr_3c0.err
..\..\sh\bin\shc -sub=spr.shc spr_3c1.c >spr_3c1.err
..\..\sh\bin\shc -sub=spr.shc spr_3dsp.c >spr_3dsp.err
..\..\sh\bin\asmsh spr_2a.src -cpu=7600 >spr_2a.err
..\..\sh\bin\asmsh spr_3a0.src -cpu=7600 >spr_3a0.err
..\..\sh\bin\asmsh spr_3a1.src -cpu=7600 >spr_3a1.err
..\..\sh\bin\asmsh spr_3tri.src -cpu=7600 >spr_3tri.err
..\..\sh\bin\lbr /sub=spr.lbr
copy sega_spr.lib ..\lib
dir *.err
