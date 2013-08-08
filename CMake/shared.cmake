#Shared Libraries specific to our GHOST needs

add_external_project(
    shared
    DEPENDS hdf4 hdf5 cdf

    CMAKE_ARGS
        -DSHARED_BUILD_IO:BOOL=true
        -DSHARED_BUILD_DATETIME:BOOL=true
        -DSHARED_BUILD_CXFORM:BOOL=true

)
