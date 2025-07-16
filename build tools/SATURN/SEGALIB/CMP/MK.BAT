@echo off
IF EXIST sega_cmp.lib del sega_cmp.lib
IF EXIST sega_cmp.h copy sega_cmp.h ..\include
del *.obj

..\..\sh\bin\shc -sub=cmp.shc cmplrun.c  >cmp_lrun.err
..\..\sh\bin\shc -sub=cmp.shc cmplrunb.c >cmp_lrunb.err
..\..\sh\bin\shc -sub=cmp.shc cmplrunw.c >cmp_lrunw.err
..\..\sh\bin\shc -sub=cmp.shc cmplrund.c >cmp_lrund.err
..\..\sh\bin\shc -sub=cmp.shc cmpltool.c >cmp_ltool.err
..\..\sh\bin\lbr /sub=cmp.lbr
copy sega_cmp.lib ..\lib
dir *.err
