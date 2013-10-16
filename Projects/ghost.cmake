### Adding the actual ghost build ###

add_external_project(
    ghost
    DEPENDS io CDFreader DateTime boost qt paraview

    CMAKE_ARGS
        -DGHOST_MASTER_BUILD=true
        -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
        -DParaView_DIR=${CMAKE_BINARY_DIR}/paraview/src/paraview-build
        -DDEP_INCLUDE_DIR=${install_location}/include
        -DGHOST_PV_CMAKE_DIR=${GHOST_PV_CMAKE_DIR}

    INSTALL_COMMAND ${CMAKE_MAKE_PROGRAM} dist

)

