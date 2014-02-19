#DateTime Libraries specific to our GHOST needs

if(NOT ${CMAKE_OSX_DEPLOYMENT_TARGET})
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.6")
endif()

add_external_project(
    DateTime

    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>


)
