# Try to find NASA Common Data Format (CDF) Library
# Defines:  CDF_FOUND - system contains CDF
#           CDF_INCLUDE_DIR - Include Directories for CDF
#           CDF_LIBRARIES - Lirbraries required for CDF
#           CDF_DEFINITIONS - Compiler Switches required for using CDF

find_package(PkgConfig)
pkg_check_modules(PC_cdf QUIET cdf)
set(cdf_DEFINITIONS ${PC_cdf_CFLAGS_OTHER})

find_path(cdf_INCLUDE_DIR cdf/xpath.h
                HINTS ${PC_cdf_INCLUDEDIR} ${PC_cdf_INCLUDE_DIRS}
                PATH_SUFFIXES cdf )

find_library(cdf_LIBRARY_NAMES cdf
                HINTS ${PC_cdf_LIBDIR} ${PC_cdf_LIBRARY_DIRS})

set(cdf_LIBRARIES ${cdf_LIBRARY})
set(cdf_INCLUDE_DIR ${cdf_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(cdf DEFAULT_MSG
                                  cdf_LIBRARY cdf_INCLUDE_DIR)

mark_as_advanced(cdf_INCLUDE_DIR cdf_LIBRARY)
