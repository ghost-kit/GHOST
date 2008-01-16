#
# Makefile to compile objects common to LTR-para:
#

# Include compiler information
include ../../env/Make.${machine}
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
	$(F77) -c  $(FFLAGS) $<

#
# Make targets:
#

OBJS = DateTime.o

common: $(OBJS)
	$(AR) rcs libcommon.a $(OBJS)

clean:
	rm -f *.o
	rm -f libcommon.a
