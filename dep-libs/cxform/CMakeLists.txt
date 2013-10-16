CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT("cxform")

add_library(
            cxform
            STATIC
            cxform-auto.c
            cxform-manual.c
            )


INSTALL(FILES
        ${PROJECT_SOURCE_DIR}/cxform/cxform.h
        DESTINATION include/)

INSTALL(FILES
            ${PROJECT_BINARY_DIR}/cxform/${CMAKE_STATIC_LIBRARY_PREFIX}cxform${CMAKE_STATIC_LIBRARY_SUFFIX}
        DESTINATION lib/)
