del ..\..\..\sgl\cdsrc\obj\*.*
del ..\..\..\sgl\cdsrc\lib\*.*
cd ..\..\..\segalib\gfs
make clean
make DFLAGS="-DUSE_SGL -I../../sgl/inc"
copy *.a ..\..\sgl\cdsrc\obj
make clean
cd ..\..\segalib\stm
make clean
make DFLAGS="-DUSE_SGL -I../../sgl/inc"
copy *.a ..\..\sgl\cdsrc\obj
make clean
copy ..\..\segalib\lib\sega_cdc.a ..\..\sgl\cdsrc\obj
cd ..\..\sgl\cdsrc\src
make -f Makefile.scd
copy *.o ..\..\..\sgl\cdsrc\obj
cd ..\..\..\sgl\cdsrc\obj
ar x sega_gfs.a
ar x sega_stm.a
ar x sega_cdc.a
ar rv ..\..\..\sgl\cdsrc\lib\libcd.a *.o
del *.o
ar x segadgfs.a
ar x sega_stm.a
ar x sega_cdc.a
ar rv ..\..\..\sgl\cdsrc\lib\libcd_d.a *.o
del *.o
del *.a

