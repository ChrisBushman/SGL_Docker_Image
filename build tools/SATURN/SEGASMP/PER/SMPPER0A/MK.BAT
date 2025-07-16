@echo off
..\..\..\sh\bin\shc -sub=shc.sub main.c>main.err
..\..\..\sh\bin\lnk -sub=..\..\lib\shlnk.sub >SHLNK.ERR
..\..\..\sh\bin\rof2bin -q sh.abs
echo *
dir *.err
