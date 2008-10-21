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

OBJS = geopack.o DateTime.o parse_xjd.o \
	TinyXML/tinystr.o TinyXML/tinyxml.o TinyXML/tinyxmlerror.o TinyXML/tinyxmlparser.o

OBJS_MHD = MHDInnerBoundaryInterface.o MHD_IC_InnerBoundaryInterface.o MHD_FE_InnerBoundaryInterface.o

all: common RMSerror 

common: common-MIX common-LFM
	$(AR) rcs libcommon.a $(OBJS) $(OBJS_MHD)

common-MIX: $(OBJS)
	$(AR) rcs libcommon-MIX.a $(OBJS)

common-LFM: $(OBJS) $(OBJS_MHD)
	$(AR) rcs libcommon-LFM.a $(OBJS) $(OBJS_MHD)

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
	rm -f libcommon.a libcommon-MIX.a libcommon-LFM.a

distclean: clean
	cd perlLibs && $(MAKE) distclean
	cd RMSerror && $(MAKE) distclean
	cd TinyXML && $(MAKE) distclean
	rm -rf ../lib ../lib64

FORCE:
