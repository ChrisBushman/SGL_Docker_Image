@echo off
IF EXIST sega_snd.lib del sega_snd.lib
IF EXIST sega_snd.h copy sega_snd.h ..\include
..\..\sh\bin\shc -sub=snd.shc snd_main.c >snd_main.err
..\..\sh\bin\lbr /sub=snd.lbr
copy sega_snd.lib ..\lib
dir *.err