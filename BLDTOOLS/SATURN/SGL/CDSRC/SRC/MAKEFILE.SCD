#!/bin/make

CC = sh-gcc
AR  = sh-ar
CP  = cp
RM  = rm -rf

CCFLAGS = -Wall -m2 -O2 -c -I$(SGLINC) -I$(GFSDIR) -I$(STMDIR)
DFLAGS  =
LDFLAGS = 
ARFLAGS = rv 

LIBDIR  = ../lib
OBJDIR  = ../obj
SRCDIR  = ../src
INCDIR  = ../inc

SATURN  = ../../..
SEGALIB = $(SATURN)/segalib
STMDIR  = $(SEGALIB)/stm
GFSDIR  = $(SEGALIB)/gfs

SGLDIR = ../..
SGLINC = $(SGLDIR)/inc
SGLLDR = $(SGLDIR)/lib

include $(SRCDIR)/objects

all:
	$(RM) $(STMDIR)/*.o $(GFSDIR)/*.o $(STMDIR)/*.a $(GFSDIR)/*.a
	$(RM) *.a $(OBJDIR)/*.a $(OBJDIR)/*.o
	$(MAKE) -f Makefile.gfs
	$(MAKE) -f Makefile.stm
	$(CP) $(SEGALIB)/lib/sega_cdc.a .
	$(AR) x sega_cdc.a
	$(AR) x sega_gfs.a
	$(AR) x sega_stm.a
	$(CC) $(CCFLAGS) -o sgl_cd.o sgl_cd.c
	$(CP) *.o $(OBJDIR)
	$(RM) *.o *.a
	$(AR) $(ARFLAGS) $(LIBDIR)/libcd.a $(OBJDIR)/*.o
	$(RM) $(GFSDIR)/*.o  $(OBJDIR)/gfs*.o
	$(MAKE) -f Makefile.gfs DFLAGS=DEBUG
	$(AR) x segadgfs.a
	$(CP) *.o $(OBJDIR)
	$(AR) $(ARFLAGS) $(LIBDIR)/libcd_d.a $(OBJDIR)/*.o
	$(RM) *.o *.a


clean:
	$(RM) *.a $(OBJDIR)/*
