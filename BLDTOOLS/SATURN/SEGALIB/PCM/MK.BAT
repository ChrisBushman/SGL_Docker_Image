IF EXIST sega_pcm.lib DEL sega_pcm.lib
IF EXIST sega_pcm.h copy sega_pcm.h ..\include
DEL *.obj

shc pcm_adp.c  -sub=pcm.shc > pcm_adp.err
shc pcm_aif.c  -sub=pcm.shc > pcm_aif.err
shc pcm_aif2.c -sub=pcm.shc > pcm_aif2.err
shc pcm_audi.c -sub=pcm.shc > pcm_audi.err
shc pcm_deb.c  -sub=pcm.shc > pcm_deb.err
shc pcm_drv.c  -sub=pcm.shc > pcm_drv.err
shc pcm_etc.c  -sub=pcm.shc > pcm_etc.err
shc pcm_gfs.c  -sub=pcm.shc > pcm_gfs.err
shc pcm_lib.c  -sub=pcm.shc > pcm_lib.err
shc pcm_mp.c   -sub=pcm.shc > pcm_mp.err
shc pcm_stm.c  -sub=pcm.shc > pcm_stm.err
shc pcm_time.c -sub=pcm.shc > pcm_time.err
shc pcm_xaud.c -sub=pcm.shc > pcm_xaud.err
shc pcm_xcmn.c -sub=pcm.shc > pcm_xcmn.err
shc pcm_xgfs.c -sub=pcm.shc > pcm_xgfs.err
shc pcm_xmp.c  -sub=pcm.shc > pcm_xmp.err
shc pcm_xstm.c -sub=pcm.shc > pcm_xstm.err
lbr /sub=pcm.lbr
COPY SEGA_PCM.LIB ..\LIB
dir *.err
