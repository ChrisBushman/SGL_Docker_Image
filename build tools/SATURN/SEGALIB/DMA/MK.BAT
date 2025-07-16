@echo off
IF EXIST sega_dma.h COPY sega_dma.h ..\include
IF EXIST sega_dma.lib DEL sega_dma.lib
DEL *.obj
..\..\sh\bin\shc -sub=dma.shc dma_cpu0.c >dma_cpu0.err
..\..\sh\bin\shc -sub=dma.shc dma_cpu1.c >dma_cpu1.err
..\..\sh\bin\shc -sub=dma.shc dma_cpu2.c >dma_cpu2.err
..\..\sh\bin\shc -sub=dma.shc dma_cpu3.c >dma_cpu3.err
..\..\sh\bin\shc -sub=dma.shc dma_cpu4.c >dma_cpu4.err
..\..\sh\bin\shc -sub=dma.shc dma_cpu5.c >dma_cpu5.err
..\..\sh\bin\shc -sub=dma.shc dma_scu0.c >dma_scu0.err
..\..\sh\bin\shc -sub=dma.shc dma_scu1.c >dma_scu1.err
..\..\sh\bin\lbr /sub=dma.lbr
DIR *.err
copy sega_dma.lib ..\lib
