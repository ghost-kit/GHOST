add_external_project(
    hdf4
    DEPENDS jpeg zlib szip

    CONFIGURE_COMMAND
        <SOURCE_DIR>/configure
        --prefix=<INSTALL_DIR>
        --enable-static
        --enable-production
        --disable-fortran
        --with-szlib=${install_location}/lib
        --with-jpeg=${install_location}/lib
        --with-zlib=${install_location}/lib

)
