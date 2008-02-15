#
# Makefile to compile TinyXML objects:
#5A
# NOTE:  Unless you know what you're doing, you shouldn't be here!
#        See the Make targets in "LTR-para/Makefile"
#

# Include compiler information
include ../../../env/Make.${MACHINE}
# Specify compiler flags
include ../Make.machine

#
# Compiler instructions:
#
.SUFFIXES: .o .cpp 

.cpp.o:
	$(MPICXX) -c  $(CCFLAGS) -I. $<

#
# Object files to compile:
#

#
TinyXML_OBJS = tinyxml.o tinystr.o tinyxmlparser.o tinyxmlerror.o

#
# Make targets:
#

tinyxml_obj: $(TinyXML_OBJS)

tinyxml.o: tinyxml.h tinystr.h
tinyxmlparser.o: tinyxml.h tinystr.h
xmltest.o: tinyxml.h tinystr.h
tinyxmlerror.o: tinyxml.h tinystr.h

clean:
	-rm -f *.o