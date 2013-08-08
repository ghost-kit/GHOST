cmake_minimum_required(VERSION 2.8.9)
project(io)

if(QTCREATOR)
    file(GLOB IO_CMAKE "cmake/*")
endif()

#FIXME:  Use "find_package" for A++, MPI and Hdf4 rather than these options:
option (USE_APP "Use A++ and/or P++ for array storage" OFF)
option (USE_MPI "Build parallel I/O (Warning: leave this off, it's untested)" OFF)
option (USE_FPIC "Fix build error that required -fPIC" OFF)
option (USE_STATIC_LIBRARIES "Use Static Libraries to build" ON)

#Build statically if needed
if(USE_STATIC_LIBRARIES)
    message("BUILDING WITH STATIC LIBRARIES")
    set(HDF4_LIBRARIES_NAME "StaticHDF4")
    set(HDF5_USE_STATIC_LIBRARIES true)
    set(JPEG_USE_STATIC_LIBRARIES true)
    set(IO_ZLIB StaticZLIB)
else()
    message("BUILDING WITH DYNAMIC LIBRARIES")
    set(HDF4_LIBRARIES_NAME "HDF4")
    set(IO_ZLIB ZLIB)
endif()

#this is the temp fix for fPIC error... just uncomment and rebuild
if(USE_FPIC)
   set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -fPIC)
endif(USE_FPIC)

## Use APP option
if (USE_APP)
  add_definitions(-DBUILD_WITH_APP)
endif (USE_APP)

## Use MPI option
if (USE_MPI)
  add_definitions(-DBUILD_WITH_MPI)
endif (USE_MPI)

########
# Hdf4 #
########################################################################
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_LIST_DIR}/cmake/")
find_package(${HDF4_LIBRARIES_NAME} REQUIRED)

if (HDF4_FOUND)
  message(STATUS "HDF4 Found!")
  message(STATUS "Linking HDF4 Libraries: ${HDF4_LIBRARIES}")
  message(STATUS "EXTRA HDF4 Lirbraries: ${HDF4_EXTRA_LIBRARIES}")

  find_package(JPEG)
  message(STATUS "JPEG: ${JPEG_LIBRARY}")

  find_package(${IO_ZLIB})
  set(HDF4_LIBRARIES ${HDF4_LIBRARIES} ${ZLIB_LIBRARY})

  add_definitions(-DHAS_HDF4)
endif (HDF4_FOUND)

########
# Hdf5 #
########################################################################
    find_package(HDF5 COMPONENTS C )
    message(STATUS "IO: Linking HDF5 Libraries: ${HDF5_LIBRARIES} ${PHDF5_LIBRARIES}")

    if (HDF5_FOUND)
        message(STATUS "HDF5 Found!")
        set(HDF5_INCLUDE_DIRS ${HDF5_C_INCLUDE_DIR})
        set( HDF5_LIBRARIES ${HDF5_hdf5_LIBRARY} ${HDF5_m_LIBRARY} ${HDF5_z_LIBRARY} )
        add_definitions(-DHAS_HDF5)

        #################
        # Parallel Hdf5 #
        ########################################################################
        if (HDF5_IS_PARALLEL)
            add_definitions(-DHAS_PHDF5)
        endif (HDF5_IS_PARALLEL)
    endif (HDF5_FOUND)

##############
# io library #
########################################################################
set( IO_INCLUDES ${HDF4_INCLUDE_DIR} ${HDF5_INCLUDE_DIRS} )
include_directories(${IO_INCLUDES})
set( IO_LIBRARIES
            ${HDF5_LIBRARIES}
            ${PHDF5_LIBRARIES}
            ${HDF4_LIBRARIES}
            ${JPEG_LIBRARIES} )

message(STATUS "IO: LINKING LIBRARIES: ${IO_LIBRARIES}")

add_library(io STATIC 
            ErrorQueue.h ErrorQueue.C
            Io.hpp Io.C
            IoTemplates.hpp Util.hpp
            Hdf.hpp Hdf.C Hdf4.hpp Hdf4.C
            Hdf5.hpp Hdf5.C
            PHdf5.hpp PHdf5.C
            ${IO_CMAKE} )



###########
# cxxtest #
########################################################################
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${PROJECT_SOURCE_DIR}/../../Utilities/cxxtest)
find_package(CxxTest)
if(CXXTEST_FOUND)
    message(STATUS "cxxtest Found!")
    set(CXXTEST_USE_PYTHON TRUE)
    include_directories(${CXXTEST_INCLUDE_DIR})
    enable_testing()
    CXXTEST_ADD_TEST(testErrorQueue testErrorQueue.C "${PROJECT_SOURCE_DIR}/testErrorQueue.cxxTest")
    ADD_LIBRARY(test_ErrorQueue ErrorQueue.h ErrorQueue.C)
    TARGET_LINK_LIBRARIES(testErrorQueue test_ErrorQueue)
    CXXTEST_ADD_TEST(testNoppNompi testNoppNompi.C "${PROJECT_SOURCE_DIR}/testNoppNompi.cxxTest")
    target_link_libraries(testNoppNompi io ${IO_LIBRARIES} )
else ()
    message ( STATUS "Could NOT find CxxTest - unit testing not available" )
endif ()
