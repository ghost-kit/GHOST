cmake_minimum_required(VERSION 2.8.9)
project(io)

#FIXME:  Use "find_package" for A++, MPI and Hdf4 rather than these options:
option (USE_APP "Use A++ and/or P++ for array storage" OFF)
option (USE_MPI "Build parallel I/O (Warning: leave this off, it's untested)" OFF)
option (USE_FPIC "Fix build error that required -fPIC" OFF)
option (USE_STATIC_LIBRARIES "Use Static Libraries to build" OFF)
option (USE_PARAVIEW_LIBS "Use the ParaView Libraries" ON)

if(USE_PARAVIEW_LIBS)
    message(STATUS "Building with ParaView Libraries")
    if(NOT PV_SBUILD_DIR)
        message(FATAL_ERROR "You MUST define PV_SBUILD_DIR
to be the location of the successfully built ParaView Superbuild that
you will be building against.  Please set and try again.")

    else()
        message(STATUS "Superbuild Directory :${PV_SBUILD_DIR}")
    endif()

    set(PV_LIB_DIR ${PV_SBUILD_DIR}/install/lib)

    set(PV_HDF5_LIBS ${PV_LIB_DIR}/libhdf5.so;${PV_LIB_DIR}/libhdf5_hl.so;${PV_LIB_DIR}/)
    set(PV_HDF5_INCLUDE_DIR ${PV_SBUILD_DIR}/install/include)

    message(STATUS: "PV_HDF5 libraries: ${PV_HDF5_LIBS}")

    message(STATUS "PARAVIEW LIBRARIES: ${ParaView_LIBRARIES}")

endif()

if(USE_STATIC_LIBRARIES)
    message(STATUS "BUILDING WITH STATIC LIBRARIES")
    set(CMAKE_FIND_LIBRARY_PREFIXES "${CMAKE_STATIC_LIBRARY_SUFFIX}")
    add_definitions(-DHDF5_USE_STATIC_LIBRARIES=true)
    message(STATUS "Static Suffix: ${CMAKE_STATIC_LIBRARY_SUFFIX}")
    set(CMAKE_LINK_DEPENDS_NO_SHARED true)
endif()

#this is the temp fix for fPIC error... just uncomment and rebuild
if(USE_FPIC)
   set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -fPIC)
endif(USE_FPIC)

if (USE_APP)
  add_definitions(-DBUILD_WITH_APP)
endif (USE_APP)

if (USE_MPI)
  add_definitions(-DBUILD_WITH_MPI)
endif (USE_MPI)

########
# Hdf4 #
########################################################################
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_LIST_DIR}/cmake/")
find_package(HDF4 REQUIRED)
message(STATUS "Linking to HDF4: ${HDF4_LIBRARIES}")
if (HDF4_FOUND)
  message(STATUS "HDF4 Found!")
  find_package(JPEG)
  find_package(ZLIB)
  
  set(HDF4_LIBRARIES ${ZLIB_LIBRARY} ${JPEG_LIBRARY} ${HDF4_LIBRARIES})  

  add_definitions(-DHAS_HDF4)
endif (HDF4_FOUND)

########
# Hdf5 #
########################################################################
if(NOT USE_PARAVIEW_LIBS)
    find_package(HDF5 COMPONENTS C )
    message(STATUS "Linking to HDF5: ${HDF5_LIBRARIES} ${PHDF5_LIBRARIES}")

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
else()
    set(HDF5_LIBRARIES ${PV_HDF5_LIBS})
    set(HDF5_INCLUDE_DIRS ${PV_HDF5_INCLUDE_DIR})

endif()
##############
# io library #
########################################################################
set( IO_INCLUDES ${HDF4_INCLUDE_DIR} ${HDF5_INCLUDE_DIRS} )
include_directories(${IO_INCLUDES})
set( IO_LIBRARIES ${HDF4_LIBRARIES} ${HDF5_LIBRARIES} ${PHDF5_LIBRARIES} )
add_library(io STATIC 
            ErrorQueue.h ErrorQueue.C
            Io.hpp Io.C
            IoTemplates.hpp Util.hpp
            Hdf.hpp Hdf.C Hdf4.hpp Hdf4.C
            Hdf5.hpp Hdf5.C
            PHdf5.hpp PHdf5.C)



###########
# cxxtest #
########################################################################
if(NOT USE_STATIC_LIBRARIES)
    set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${PROJECT_SOURCE_DIR}/cxxtest)
    find_package(CxxTest)
    if(CXXTEST_FOUND)
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
endif()
