#
# Makefile to compile objects common to LTR-para:
#

# Include compiler information
include ../../env/Make.${MACHINE}
# Specify compiler flags
include Make.machine

#
# Compiler instructions:
#

.SUFFIXES: .F .c .o .f .C 

.c.o:
	$(Ccompiler) -c $(CFLAGS) $<

.C.o:
	$(CXX) -c  $(CCFLAGS) $<

.F.o:
	$(F77) -c $(FORTOPT) $(FFLAGS) $<

#
# Make targets:
#

OBJS = geopack.o DateTime.o MHDInnerBoundaryInterface.o

common: $(OBJS)
	$(AR) rcs libcommon.a $(OBJS)

clean:
	rm -f *.o
	rm -f libcommon.a
