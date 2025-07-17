shc -sub=shc.sub main.c>main.err
lnk -sub=shlnk.sub
rof2bin -q sh.abs
echo *
dir *.err
