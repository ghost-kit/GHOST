add_external_project(
    cxform

    CMAKE_ARGS
        -DMACOSX_VERSION_MIN=10.6

    BUILD_COMMAND
        make

    INSTALL_COMMAND
        make install


)
