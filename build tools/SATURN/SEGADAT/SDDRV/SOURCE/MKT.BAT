@echo off
copy scsp.lib back.lib
copy TRGT.asm t0.asm
copy CHG.asm t1.asm
copy LED.asm t2.asm
copy HOST.asm t3.asm
copy INT_MIDI.asm t5.asm
copy MIDI.asm t6.asm
copy NON.asm t7.asm
copy NOFF.asm t8.asm
copy INIT.asm t9.asm
copy TABLE.asm t10.asm
copy SDMAIN.asm t11.asm
copy SDDRV.asm t12.asm
copy SDCOM.asm t13.asm
copy DEC2.asm t14.asm
copy SEGA.asm t15.asm

sed -f stv.sed scsp.lib > tmp
copy tmp scsp.lib
x68k -q t0.asm
x68k -q t1.asm
x68k -q t2.asm
x68k -q t3.asm
x68k -q t5.asm
x68k -q t6.asm
x68k -q t7.asm
x68k -q t8.asm
x68k -q t9.asm
x68k -q t10.asm
x68k -q t11.asm
x68k -q t12.asm
x68k -q t13.asm
x68k -q t14.asm
x68k -q t15.asm
L68 SDDRVT.LNK -d
copy back.lib scsp.lib
del t0.asm
del t1.asm
del t2.asm
del t3.asm
del t5.asm
del t6.asm
del t7.asm
del t8.asm
del t9.asm
del t10.asm
del t11.asm
del t12.asm
del t13.asm
del t14.asm
del t15.asm
del t0.obj
del t1.obj
del t2.obj
del t3.obj
del t5.obj
del t6.obj
del t7.obj
del t8.obj
del t9.obj
del t10.obj
del t11.obj
del t12.obj
del t13.obj
del t14.obj
del t15.obj
del back.lib
del tmp
echo Created sound driver for ST-V...
