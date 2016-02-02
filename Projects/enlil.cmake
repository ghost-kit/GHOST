add_external_project(
    enlil
    DEPENDS paraview boost DateTime

    CMAKE_ARGS
	-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
        -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
        -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}/release
        -DParaView_DIR=${CMAKE_BINARY_DIR}/paraview/src/paraview-build
        -DDEP_INCLUDE_DIR=${install_location}/include
        -DPV_SUPERBUILD_LIST_DIR=${CMAKE_SOURCE_DIR}
        -DPREFIX=<INSTALL_DIR>

    INSTALL_COMMAND ${CMAKE_MAKE_PROGRAM} dist

)
