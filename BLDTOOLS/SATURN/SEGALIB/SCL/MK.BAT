echo off
IF EXIST sega_scl.h COPY sega_scl.h ..\lib
IF EXIST sega_scl.lib del sega_scl.lib
del *.obj
..\..\sh\bin\shc -sub=scl.shc scl_func.c > scl_func.err
..\..\sh\bin\shc -sub=scl.shc scl_fu01.c > scl_fu01.err
..\..\sh\bin\shc -sub=scl.shc scl_ro10.c > scl_ro10.err
..\..\sh\bin\shc -sub=scl.shc scl_pri.c > scl_pri.err
..\..\sh\bin\shc -sub=scl.shc scl_pr01.c > scl_pr01.err
..\..\sh\bin\shc -sub=scl.shc scl_pr02.c > scl_pr02.err
..\..\sh\bin\shc -sub=scl.shc scl_pr03.c > scl_pr03.err
..\..\sh\bin\shc -sub=scl.shc scl_pr04.c > scl_pr04.err
..\..\sh\bin\shc -sub=scl.shc scl_pr05.c > scl_pr05.err
..\..\sh\bin\shc -sub=scl.shc scl_pr06.c > scl_pr06.err
..\..\sh\bin\shc -sub=scl.shc scl_pr07.c > scl_pr07.err
..\..\sh\bin\shc -sub=scl.shc scl_pr08.c > scl_pr08.err
..\..\sh\bin\shc -sub=scl.shc scl_pr09.c > scl_pu09.err
..\..\sh\bin\shc -sub=scl.shc scl_pr10.c > scl_pr10.err
..\..\sh\bin\shc -sub=scl.shc scl_fu02.c > scl_fu02.err
..\..\sh\bin\shc -sub=scl.shc scl_fu03.c > scl_fu03.err
..\..\sh\bin\shc -sub=scl.shc scl_fu04.c > scl_fu04.err
..\..\sh\bin\shc -sub=scl.shc scl_fu05.c > scl_fu05.err
..\..\sh\bin\shc -sub=scl.shc scl_ro00.c > scl_ro00.err
..\..\sh\bin\shc -sub=scl.shc scl_ro01.c > scl_ro01.err
..\..\sh\bin\shc -sub=scl.shc scl_ro02.c > scl_ro02.err
..\..\sh\bin\shc -sub=scl.shc scl_ro03.c > scl_ro03.err
..\..\sh\bin\shc -sub=scl.shc scl_ro04.c > scl_ro04.err
..\..\sh\bin\shc -sub=scl.shc scl_ro05.c > scl_ro05.err
..\..\sh\bin\shc -sub=scl.shc scl_pr11.c > scl_pr11.err
..\..\sh\bin\shc -sub=scl.shc scl_pr12.c > scl_pr12.err
..\..\sh\bin\shc -sub=scl.shc scl_pr13.c > scl_pr13.err
..\..\sh\bin\shc -sub=scl.shc scl_pr14.c > scl_pr14.err
..\..\sh\bin\shc -sub=scl.shc scl_win0.c > scl_win0.err
..\..\sh\bin\shc -sub=scl.shc scl_win1.c > scl_win1.err
..\..\sh\bin\shc -sub=scl.shc scl_win2.c > scl_win2.err
..\..\sh\bin\shc -sub=scl.shc scl_win3.c > scl_win3.err
..\..\sh\bin\shc -sub=scl.shc scl_mosa.c > scl_mosa.err
..\..\sh\bin\shc -sub=scl.shc scl_lncl.c > scl_lncl.err
..\..\sh\bin\shc -sub=scl.shc scl_back.c > scl_back.err
..\..\sh\bin\shc -sub=scl.shc scl_vblv.c > scl_vblv.err
..\..\sh\bin\lbr /sub=scl.lbr
copy sega_scl.lib ..\lib
dir *.err