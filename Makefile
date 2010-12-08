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
	$(F77) -c $(GEOPACK_FFLAGS) $(FFLAGS) $<

#
# Make targets:
#

OBJS = Check_IC_Status.o geopack.o DateTime.o parse_xjd.o \
	TinyXML/tinystr.o TinyXML/tinyxml.o TinyXML/tinyxmlerror.o TinyXML/tinyxmlparser.o

all: common RMSerror 

common: $(OBJS)
	$(AR) rcs libcommon.a $(OBJS)

parse_xjd.o: 
	cd TinyXML && $(MAKE) tinyxml_obj
	$(MPICXX) -ITinyXML -c $(CCFLAGS) -o parse_xjd.o -LTinyXML parse_xjd.C

RMSerror: FORCE
	cd RMSerror && $(MAKE)

test:
	${MAKE} -f Make.test

clean:
	cd RMSerror && $(MAKE) clean
	cd TinyXML && $(MAKE) clean
	rm -f *.o
	rm -f libcommon.a
	${MAKE} -f Make.test clean

distclean: clean
	cd RMSerror && $(MAKE) distclean
	cd TinyXML && $(MAKE) distclean

FORCE:
