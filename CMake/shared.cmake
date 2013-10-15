#Shared Libraries specific to our GHOST needs

add_external_project(
    shared
    DEPENDS hdf4 hdf5 cdf qt

    CMAKE_ARGS
        -DSHARED_BUILD_IO=true
        -DSHARED_BUILD_DATETIME=true
        -DSHARED_BUILD_CXFORM=true
        -DUSE_MASTER_BUILD=true

)
