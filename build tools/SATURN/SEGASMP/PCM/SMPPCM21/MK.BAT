@echo off
shc -sub=shc.sub main.c > main.err
lnk -sub=shlnk.sub > lnk.err
rof2bin -q sh.abs
echo *
dir *.err
