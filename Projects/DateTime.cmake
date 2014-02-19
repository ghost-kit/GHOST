#DateTime Libraries specific to our GHOST needs

if(NOT ${CMAKE_OSX_DEPLOYMENT_TARGET})
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.6")
endif()

add_external_project(
    DateTime

    BUILD_COMMAND
        CXX=${CMAKE_CXX_COMPILER} make

    INSTALL_COMMAND
        make install

)
