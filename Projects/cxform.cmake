
if(NOT ${CMAKE_OSX_DEPLOYMENT_TARGET})
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.6")
endif()

add_external_project(
    cxform
    DEPENDS DateTime


    CMAKE_ARGS
        -DMACOSX_VERSION_MIN=${CMAKE_OSX_DEPLOYMENT_TARGET}
        -DPREFIX=<INSTALL_DIR>

    BUILD_COMMAND
        make

    INSTALL_COMMAND
        make install


)
