# Build and install local copy of dependencies for mklfmparascripts perl scripts.

all: Carp-Clan Bit-Vector Date-Calc LTR-Inputs
	- cp -r lib ../..

Carp-Clan:
	gunzip < Carp-Clan-5.3.tar.gz > Carp-Clan-5.3.tar
	tar -xf Carp-Clan-5.3.tar
	rm Carp-Clan-5.3.tar
	cd Carp-Clan-5.3 && perl Makefile.PL PREFIX=${PWD}
	cd Carp-Clan-5.3 && ${MAKE}; ${MAKE} install

Bit-Vector:
	gunzip < Bit-Vector-6.4.tar.gz > Bit-Vector-6.4.tar
	tar -xf Bit-Vector-6.4.tar
	rm Bit-Vector-6.4.tar
	cd Bit-Vector-6.4 && perl Makefile.PL PREFIX=${PWD}
	cd Bit-Vector-6.4 && ${MAKE}; ${MAKE} install

Date-Calc:
	gunzip < Date-Calc-5.4.tar.gz > Date-Calc-5.4.tar
	tar -xf Date-Calc-5.4.tar
	rm Date-Calc-5.4.tar
	cd Date-Calc-5.4 && perl Makefile.PL PREFIX=${PWD}
	cd Date-Calc-5.4 && ${MAKE}; ${MAKE} install

LTR-Inputs:
	cd LTR && perl Makefile.PL PREFIX=${PWD}
	cd LTR && ${MAKE}; ${MAKE} install

clean:
	rm -rf Carp-Clan-5.3 Carp-Clan-5.3.tar
	rm -rf Bit-Vector-6.4 Bit-Vector-6.4.tar
	rm -rf Date-Calc-5.4 Date-Calc-5.4.tar
	-test -e LTR/Makefile && cd LTR && ${MAKE} realclean
	rm -rf lib man

distclean: clean
