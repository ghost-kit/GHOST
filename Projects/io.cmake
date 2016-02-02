#io Libraries specific to our GHOST needs

if(NOT ${CMAKE_OSX_DEPLOYMENT_TARGET})
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.6")
endif()

add_external_project(
    io
    DEPENDS hdf4 hdf5

    CMAKE_ARGS
        -DMACOSX_VERSION_MIN=${CMAKE_OSX_DEPLOYMENT_TARGET}
	-DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
	-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
    BUILD_COMMAND
        make

    INSTALL_COMMAND
        make install
)
