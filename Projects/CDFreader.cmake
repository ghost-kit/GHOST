#CDFreader Library specific to our GHOST needs

add_external_project(
    CDFreader
    DEPENDS cdf boost DateTime qt

    CMAKE_ARGS
	-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
        -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
        -DMACOSX_VERSION_MIN=10.6
        -DPREFIX=<INSTALL_DIR>
        -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>

    BUILD_COMMAND
        make

    INSTALL_COMMAND
        make install

)
