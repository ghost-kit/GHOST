#io Libraries specific to our GHOST needs

add_external_project(
    io
    DEPENDS hdf4 hdf5

    CMAKE_ARGS
        -DMACOSX_VERSION_MIN=10.6

    BUILD_COMMAND
        make

    INSTALL_COMMAND
        make install
)
