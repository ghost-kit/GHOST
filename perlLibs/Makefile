# Build and install local copy of dependencies for mklfmparascripts perl scripts.

all: Carp-Clan Bit-Vector Date-Calc LTR-Inputs

Carp-Clan:
	gunzip < Carp-Clan-5.3.tar.gz | tar -x
	cd Carp-Clan-5.3 && perl Makefile.PL PREFIX=../../..

	cd Carp-Clan-5.3 && ${MAKE}; ${MAKE} install

Bit-Vector:
	gunzip < Bit-Vector-6.4.tar.gz | tar -x
	cd Bit-Vector-6.4 && perl Makefile.PL PREFIX=../../..
	cd Bit-Vector-6.4 && ${MAKE}; ${MAKE} install

Date-Calc:
	gunzip < Date-Calc-5.4.tar.gz | tar -x
	cd Date-Calc-5.4 && perl Makefile.PL PREFIX=../../..
	cd Date-Calc-5.4 && ${MAKE}; ${MAKE} install

LTR-Inputs:
	cd LTR && perl Makefile.PL PREFIX=../../..
	cd LTR && ${MAKE}; ${MAKE} install

clean:
	rm -rf Carp-Clan-5.3
	rm -rf Bit-Vector-6.4
	rm -rf Date-Calc-5.4
	-test -e LTR/Makefile && cd LTR && ${MAKE} realclean

distclean: clean
