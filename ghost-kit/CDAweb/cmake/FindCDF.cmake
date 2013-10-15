# Try to find NASA Common Data Format (CDF) Library
# Defines:  CDF_FOUND - system contains CDF
#           CDF_INCLUDE_DIR - Include Directories for CDF
#           CDF_LIB - Lirbraries required for CDF

if(CDF_INCLUDES)
    #already in cache
    set(CDF_FIND_QUIETLY TRUE)
endif(CDF_INCLUDES)

find_path(CDF_INCLUDES cdf.h
            HINTS "/Applications/cdf34_1-dist/include")

if(CDF_USE_STATIC_LIBS)
    find_library(CDF_LIB NAMES ${CMAKE_STATIC_LIBRARY_PREFIX}cdf${CMAKE_STATIC_LIBRARY_SUFFIX}
                 HINTS "/Applications/cdf34_1-dist/lib")
else()
    find_library(CDF_LIB NAMES  ${CMAKE_SHARED_LIBRARY_PREFIX}cdf${CMAKE_SHARED_LIBRARY_SUFFIX}
                 HINTS "/Applications/cdf34_1-dist/lib")
endif(CDF_USE_STATIC_LIBS)

set(CDF_LIBRARIES "${CDF_LIB}")

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args( CDF DEFAULT_MSG CDF_LIBRARIES CDF_INCLUDES )

mark_as_advanced(CDF_LIB CDF_INCLUDES)
