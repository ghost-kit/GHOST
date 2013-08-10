add_external_project(
    hdf4
    DEPENDS jpeg zlib szip

    CONFIGURE_COMMAND

        <SOURCE_DIR>/configure
        CFLAGS=-I${install_location}/include
        CXXFLAGS=-I${install_location}/include
        LDFLAGS=-L${install_location}/lib
        --prefix=<INSTALL_DIR>

        --with-sysroot=${CMAKE_OSX_SYSROOT}
        --enable-static
        --enable-production
        --disable-fortran
        --with-szlib
      )
