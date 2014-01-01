add_external_project(
    gkcxform
    DEPENDS paraview boost DateTime cxform

    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}/release
        -DParaView_DIR=${CMAKE_BINARY_DIR}/paraview/src/paraview-build
        -DDEP_INCLUDE_DIR=${install_location}/include
        -DPV_SUPERBUILD_LIST_DIR=${CMAKE_SOURCE_DIR}
        -DPREFIX=<INSTALL_DIR>

    INSTALL_COMMAND ${CMAKE_MAKE_PROGRAM} dist

)
