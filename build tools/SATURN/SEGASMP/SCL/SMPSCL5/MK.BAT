@echo off
del *.obj
..\..\..\sh\bin\shc -sub=shc.sub main.c >main.err
..\..\..\sh\bin\shc -sub=shc.sub asciifon.c >asciifon.err
..\..\..\sh\bin\shc -sub=shc.sub textur.c >textur.err
..\..\..\sh\bin\lnk -sub=shlnk.sub
..\..\..\sh\bin\rof2bin -q sh.abs
echo *
dir *.err
