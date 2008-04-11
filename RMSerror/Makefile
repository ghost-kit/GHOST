# Include compiler information
include ../../../env/Make.${MACHINE}

HDFINC = -I$(HDF)/include
HDFLIB = -L$(HDF)/lib -lmfhdf -ldf -ljpeg -lz 

CFLAGS = $(HDFINC) -DNDEBUG
CXXFLAGS = $(HDFINC) -DNDEBUG
LIBS = $(HDFLIB)

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

all: RMSerror install

OBJS = LFMpara.o RMSerror.o
RMSerror: $(OBJS)
	$(CXX) $(OBJS) -o RMSerror $(LIBS)

install: RMSerror
	cp RMSerror ../../bin

clean:
	rm -f *.o RMSerror

distclean: clean
	rm -f ../../bin/RMSerror
