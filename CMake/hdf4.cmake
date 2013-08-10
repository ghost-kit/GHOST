add_external_project(
    hdf4
    DEPENDS jpeg zlib szip

    CONFIGURE_COMMAND
        <SOURCE_DIR>/configure
        --prefix=<INSTALL_DIR>
        --enable-static
        --enable-production
        --disable-fortran
        --with-szlib=${install_location}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}sz${CMAKE_STATIC_LIBRARY_SUFFIX}
        --with-jpeg=${install_location}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}jpeg${CMAKE_STATIC_LIBRARY_SUFFIX}
        --with-zlib=${install_location}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}z${CMAKE_STATIC_LIBRARY_SUFFIX}

)
