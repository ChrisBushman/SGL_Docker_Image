@echo off
..\..\sh\bin\asmsh sys_id.src -cpu=7600 %1 %2 %3
..\..\sh\bin\shc -sub=shc.sub main.c
PAUSE
..\..\sh\bin\lnk -sub=shlnk.sub
..\..\sh\bin\rof2bin -q ip_sh.abs
echo *
