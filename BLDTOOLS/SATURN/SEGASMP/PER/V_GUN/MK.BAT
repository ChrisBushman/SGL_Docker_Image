@echo off
IF EXIST sh.bin del sh.bin
DEL *.obj
..\..\..\sh\bin\shc -sub=shc.sub main.c  >main.err
..\..\..\sh\bin\shc -sub=shc.sub image.c >image.err
..\..\..\sh\bin\shc -sub=shc.sub move.c >move.err
..\..\..\sh\bin\shc -sub=shc.sub spr2.c >spr2.err
..\..\..\sh\bin\shc -sub=shc.sub test.c >test.err
..\..\..\sh\bin\lnk -sub=shlnk.sub
..\..\..\sh\bin\rof2bin -q sh.abs
echo *
dir *.err