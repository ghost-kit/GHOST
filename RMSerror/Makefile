# Include compiler information
include ../../../env/Make.${MACHINE}

HDFINC = -I$(HDF)/include
HDFLIB = -L$(HDF)/lib -lmfhdf -ldf -ljpeg -lz 

CFLAGS = $(ARCH) $(TRAP) $(HDFINC) -DNDEBUG
CXXFLAGS = $(ARCH) $(TRAP) $(HDFINC) -DNDEBUG
LIBS = $(EXTRA_LIBS) $(HDFLIB)

#
# Compiler instructions:
#

.SUFFIXES: .F .c .o .f .C 

.c.o:
	$(CC) -c $(CFLAGS) $<

.C.o:
	$(CXX) -c  $(CXXFLAGS) $<

#
# Make Targets
#

all: RMSerror CalculateNorm install

RMSerror: RMSerror.o
	$(CXX) RMSerror.o -o RMSerror $(LIBS)

CalculateNorm: CalculateNorm.o
	$(CXX) CalculateNorm.o -o CalculateNorm $(LIBS)

install: RMSerror CalculateNorm
	cp RMSerror CalculateNorm ../../../common/bin

clean:
	rm -f *.o RMSerror CalculateNorm

distclean: clean