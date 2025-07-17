@ECHO OFF
IF EXIST sega_gfs.h COPY sega_gfs.h ..\include

IF EXIST sega_gfs.lib DEL sega_gfs.lib
IF EXIST segadgfs.lib DEL segadgfs.lib
DEL *.obj

shc gfs.c -sub=gfs.shc > gfs.err
shc gfs_buf.c -sub=gfs.shc  >gfs_buf.err
shc gfs_cdb.c -sub=gfs.shc  >gfs_cdb.err
shc gfs_cdc.c -sub=gfs.shc  >gfs_cdc.err
shc gfs_cdf.c -sub=gfs.shc  >gfs_cdf.err
shc gfs_dir.c -sub=gfs.shc  >gfs_dir.err
shc gfs_trn.c -sub=gfs.shc  >gfs_trn.err
lbr -sub=gfs.lbr > gfs_lbr.err
COPY sega_gfs.lib ..\lib
DIR *.err
PAUSE

shc gfs.c -ob=gfsd.obj -def=DEBUG_LIB -sub=gfs.shc  >gfsd
shc gfs_buf.c -ob=gfsd_buf.obj -def=DEBUG_LIB  -sub=gfs.shc  >gfsd_buf.err
shc gfs_cdb.c -ob=gfsd_cdb.obj -def=DEBUG_LIB  -sub=gfs.shc  >gfsd_cdb.err
shc gfs_cdc.c -ob=gfsd_cdc.obj -def=DEBUG_LIB  -sub=gfs.shc  >gfsd_cdc.err
shc gfs_cdf.c -ob=gfsd_cdf.obj -def=DEBUG_LIB  -sub=gfs.shc  >gfsd_cdf.err
shc gfs_dir.c -ob=gfsd_dir.obj -def=DEBUG_LIB  -sub=gfs.shc  >gfsd_dir.err
shc gfs_mmb.c -ob=gfsd_mmb.obj -def=DEBUG_LIB  -sub=gfs.shc  >gfsd_mmb.err
shc gfs_mmc.c -ob=gfsd_mmc.obj -def=DEBUG_LIB  -sub=gfs.shc  >gfsd_mmc.err
shc gfs_mmf.c -ob=gfsd_mmf.obj -def=DEBUG_LIB  -sub=gfs.shc  >gfsd_mmf.err
shc gfs_sfb.c -ob=gfsd_sfb.obj -def=DEBUG_LIB  -sub=gfs.shc  >gfsd_sfb.err
shc gfs_sff.c -ob=gfsd_sff.obj -def=DEBUG_LIB  -sub=gfs.shc  >gfsd_sff.err
shc gfs_trn.c -ob=gfsd_trn.obj -def=DEBUG_LIB  -sub=gfs.shc  >gfsd_trn.err
lbr -sub=gfsd.lbr >gfsd_lbr.err
COPY segadgfs.lib ..\lib

DIR *.err
