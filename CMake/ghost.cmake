### Adding the actual ghost build ###

add_external_project(
    ghost
    DEPENDS shared boost

    CMAKE_ARGS
        -DGHOST_MASTER_BUILD:BOOL=true
        -DParaView_DIR=${ParaView_DIR}
        -DDEP_INCLUDE_DIR=${install_location}/include

)
