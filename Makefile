#
# Makefile to compile objects common to LTR-para:
#

# Include compiler information
include ../../env/Make.${MACHINE}
# Specify compiler flags
include Make.$(shell uname)

#
# Compiler instructions:
#

.SUFFIXES: .F .c .o .f .C 

.c.o:
	$(CC) -c $(CFLAGS) $<

.C.o:
	$(MPICXX) -c  $(CCFLAGS) $<

.F.o:
	$(MPIF77) -c $(FORTOPT) $(FFLAGS) $<

#
# Make targets:
#

.PHONY: perlLibs

OBJS = geopack.o DateTime.o MHDInnerBoundaryInterface.o MHD_IC_InnerBoundaryInterface.o parse_xjd.o \
       TinyXML/tinystr.o TinyXML/tinyxml.o TinyXML/tinyxmlerror.o TinyXML/tinyxmlparser.o

all: common RMSerror 
	- cd perlLibs && $(MAKE) 

common: $(OBJS)
	$(AR) rcs libcommon.a $(OBJS)

perlLibs: FORCE
	cd perlLibs && $(MAKE)

parse_xjd.o: 
	cd TinyXML && $(MAKE) tinyxml_obj
	$(MPICXX) -ITinyXML -c $(CCFLAGS) -o parse_xjd.o -LTinyXML parse_xjd.C

RMSerror: FORCE
	cd RMSerror && $(MAKE)

clean:
	cd perlLibs && $(MAKE) clean
	cd RMSerror && $(MAKE) clean
	cd TinyXML && $(MAKE) clean
	rm -f *.o
	rm -f libcommon.a

distclean: clean
	cd perlLibs && $(MAKE) distclean
	cd RMSerror && $(MAKE) distclean
	cd TinyXML && $(MAKE) distclean
	rm -rf ../lib ../lib64

FORCE:
