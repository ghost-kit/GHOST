# Try to find NASA Common Data Format (CDF) Library
# Defines:  CDF_FOUND - system contains CDF
#           CDF_INCLUDE_DIR - Include Directories for CDF
#           CDF_LIBRARIES - Lirbraries required for CDF
#           CDF_DEFINITIONS - Compiler Switches required for using CDF

find_package(PkgConfig)
pkg_check_modules(PC_CDF QUIET libcdf)
set(CDF_DEFINITIONS ${PC_CDF_CFLAGS_OTHER})

find_path(CDF_INCLUDE_DIR libcdf/xpath.h
                HINTS ${PC_CDF_INCLUDEDIR} ${PC_CDF_INCLUDE_DIRS}
                PATH_SUFFIXES libcdf )

find_library(CDF_LIBRARY_NAMES cdf
                HINTS ${PC_CDF_LIBDIR} ${PC_CDF_LIBRARY_DIRS})

set(CDF_LIBRARIES ${CDF_LIBRARY})
set(CDF_INCLUDE_DIR ${CDF_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(cdf DEFAULT_MSG
                                  CDF_LIBRARY CDF_INCLUDE_DIR)

mark_as_advanced(CDF_INCLUDE_DIR CDF_LIBRARY)
