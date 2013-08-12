### Adding the actual ghost build ###

add_external_project(
    ghost
    DEPENDS shared boost

    CMAKE_ARGS
        -DGHOST_MASTER_BUILD=true
        -DParaView_DIR=${ParaView_DIR}
        -DDEP_INCLUDE_DIR=${install_location}/include
        -DGHOST_PV_CMAKE_DIR=${GHOST_PV_CMAKE_DIR}

    INSTALL_COMMAND ${CMAKE_MAKE_PROGRAM} dist

)

