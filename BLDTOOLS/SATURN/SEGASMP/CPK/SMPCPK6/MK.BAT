@echo off
IF "%1" == "LINK" GOTO LINK
IF "%1" == "link" GOTO LINK

..\..\..\sh\bin\shc -sub=shc.sub main.c>main.err
:LINK
..\..\..\sh\bin\lnk -sub=shlnk.sub
..\..\..\sh\bin\rof2bin sh.abs -q
echo *
dir *.err
