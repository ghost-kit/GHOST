### Adding the actual ghost build ###

if(${BUILD_PLUGIN_WITH_DEBUG})
  set(MORE_ARGS "-DCMAKE_BUILD_TYPE=debug")
endif()

add_external_project(
    ghost
    DEPENDS io DateTime boost qt paraview cxform

    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
        -DParaView_DIR=${CMAKE_BINARY_DIR}/paraview/src/paraview-build
        -DDEP_INCLUDE_DIR=${install_location}/include
        -DGHOST_PV_CMAKE_DIR=${GHOST_PV_CMAKE_DIR}
        -DPV_SUPERBUILD_LIST_DIR=${CMAKE_SOURCE_DIR}
        ${MORE_ARGS}

    INSTALL_COMMAND ${CMAKE_MAKE_PROGRAM} dist

)

