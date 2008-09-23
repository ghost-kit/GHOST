# Build and install local copy of dependencies for mklfmparascripts perl scripts

all:
####### If "lib" exists, but "lib64" doesn't, skip build & inform the user.
	-@ test -d ../../lib -a ! -d ../../lib64 && printf "\"lib\" exists, but \"lib64\" doesnt.  Skipping Perl Library build.\n"
####### If "lib64" exists, but "lib" doesn't, skip build & inform the user.
	-@ test -d ../../lib64 -a ! -d ../../lib && printf "\"lib64\" exists, but \"lib\" doesnt.  Skipping Perl Library build.\n"
####### If neither "lib" nor "lib64" exists, inform the user & build then Perl libraries.
	-@ test ! -d ../../lib -a ! -d ../../lib64 && printf "Neither \"lib\" nor \"lib64\" exists. Building Perl libraries.\n"
	-@ test ! -d ../../lib -a ! -d ../../lib64 && ${MAKE} build-all

build-all: Carp-Clan Bit-Vector Date-Calc XML-Generator LTR-Inputs
	- cp -r lib* ../..

Carp-Clan:
	gunzip < Carp-Clan-5.3.tar.gz > Carp-Clan-5.3.tar
	tar -xf Carp-Clan-5.3.tar
	rm Carp-Clan-5.3.tar
	chmod -R a+w Carp-Clan-5.3
	cd Carp-Clan-5.3 && perl Makefile.PL PREFIX=${PWD}
	cd Carp-Clan-5.3 && ${MAKE}; ${MAKE} install

Bit-Vector:
	gunzip < Bit-Vector-6.4.tar.gz > Bit-Vector-6.4.tar
	tar -xf Bit-Vector-6.4.tar
	rm Bit-Vector-6.4.tar
	chmod -R a+w Bit-Vector-6.4
	cd Bit-Vector-6.4 && perl Makefile.PL PREFIX=${PWD}
	cd Bit-Vector-6.4 && ${MAKE}; ${MAKE} install

Date-Calc:
	gunzip < Date-Calc-5.4.tar.gz > Date-Calc-5.4.tar
	tar -xf Date-Calc-5.4.tar
	rm Date-Calc-5.4.tar
	chmod -R a+w Date-Calc-5.4
	cd Date-Calc-5.4 && perl Makefile.PL PREFIX=${PWD}
	cd Date-Calc-5.4 && ${MAKE}; ${MAKE} install

XML-Generator:
	gunzip < XML-Generator-1.01.tar.gz > XML-Generator-1.01.tar
	tar -xf XML-Generator-1.01.tar
	rm XML-Generator-1.01.tar
	chmod -R a+w XML-Generator-1.01
	cd XML-Generator-1.01 && perl Makefile.PL PREFIX=${PWD}
	cd XML-Generator-1.01 && ${MAKE}; ${MAKE} install

LTR-Inputs:
	cd LTR && perl Makefile.PL PREFIX=${PWD}
	cd LTR && ${MAKE}; ${MAKE} install

clean:
	rm -rf Carp-Clan-5.3 Carp-Clan-5.3.tar
	rm -rf Bit-Vector-6.4 Bit-Vector-6.4.tar
	rm -rf Date-Calc-5.4 Date-Calc-5.4.tar
	rm -rf XML-Generator-1.01 XML-Generator-1.01.tar
	-test -e LTR/Makefile && cd LTR && ${MAKE} realclean
	rm -rf lib lib64 man

distclean: clean
