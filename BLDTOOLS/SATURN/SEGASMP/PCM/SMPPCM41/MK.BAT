shc -sub=shc.sub main.c > main.err
lnk -sub=shlnk.sub > shlnk.err
rof2bin sh.abs
dir *.err
