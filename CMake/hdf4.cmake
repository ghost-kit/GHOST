add_external_project(
    hdf4
    DEPENDS jpeg zlib szip

    CONFIGURE_COMMAND
        <SOURCE_DIR>/configure
        --prefix=<INSTALL_DIR>
        --enable-static
        --enable-production
        --with-szlib
        --with-jpeg
        --with-zlib

)
