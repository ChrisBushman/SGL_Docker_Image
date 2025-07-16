rem ＳＨＣ環境用 実行ファイル作成バッチファイル
@echo off
shc -div=cpu -macsave=0 /deb /def=_SH /CP=76 /ST=C /OP=0 /I=..\..\..\sh\include,..\..\..\segalib\include main.c > main.err
lnk -SUB=shlnk.sub
ROF2BIN -q sh.abs
ECHO *
dir *.err
