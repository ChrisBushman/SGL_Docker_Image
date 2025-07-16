@echo off
IF EXIST sega_mem.lib del sega_mem.lib
IF EXIST sega_mem.h copy sega_mem.h ..\include
del *.obj
..\..\sh\bin\shc -sub=mem.shc mem_call.c >mem_call.err
..\..\sh\bin\shc -sub=mem.shc mem_free.c >mem_free.err
..\..\sh\bin\shc -sub=mem.shc mem_init.c >mem_init.err
..\..\sh\bin\shc -sub=mem.shc mem_mall.c >mem_mall.err
..\..\sh\bin\shc -sub=mem.shc mem_real.c >mem_real.err
..\..\sh\bin\lbr /sub=mem.lbr
copy sega_mem.lib ..\lib
dir *.err
