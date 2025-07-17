@echo off
..\..\..\sh\bin\shc -sub=shc.sub main.c >main.err
..\..\..\sh\bin\shc -sub=shc.sub scldata.c >scldata.err
..\..\..\sh\bin\lnk -sub=shlnk.sub
..\..\..\sh\bin\rof2bin -q sh.abs
echo *
dir *.err
