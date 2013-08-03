cmake_minimum_required(VERSION 2.8.9)
project(io)

#FIXME:  Use "find_package" for A++, MPI and Hdf4 rather than these options:
option (USE_APP "Use A++ and/or P++ for array storage" OFF)
option (USE_MPI "Build parallel I/O (Warning: leave this off, it's untested)" OFF)
option (USE_FPIC "Fix build error that required -fPIC" OFF)
option (USE_STATIC_LIBRARIES "Use Static Libraries to build" OFF)
option (USE_PARAVIEW_LIBS "Use the ParaView Libraries" OFF)

if(USE_STATIC_LIBRARIES)
    message("BUILDING WITH STATIC LIBRARIES")
    set(CMAKE_FIND_LIBRARY_PREFIXES "${CMAKE_STATIC_LIBRARY_SUFFIX}")


    find_package(JPEG)
    find_library(HDF4_LIBRARY NAMES df dfalt PATH_SUFFIXES hdf)
    find_library(MFHDF4_LIBRARY NAMES mfhdf mfhdfalt PATH_SUFFIXES hdf)
    set(HDF4_LIBRARIES ${MFHDF4_LIBRARY} ${HDF4_LIBRARY})
    find_path(HDF4_INCLUDE_DIR NAMES hdf.h PATH_SUFFIXES hdf)
    include(CheckLibraryExists)
    include(FindPackageHandleStandardArgs)
    if(HDF4_LIBRARIES)
        set(CMAKE_REQUIRED_LIBRARIES z ${JPEG_LIBRARIES})
        check_library_exists("${HDF4_LIBRARIES}" Hopen "" HDF4_WO_SZIP)
        if(HDF4_WO_SZIP)
                set(HDF4_EXTRA_LIBRARIES ${CMAKE_REQUIRED_LIBRARIES})
                find_package_handle_standard_args(HDF4 DEFAULT_MSG HDF4_LIBRARIES HDF4_EXTRA_LIBRARIES HDF4_INCLUDE_DIR)
        else(HDF4_WO_SZIP)
                find_library(SZIP_LIBRARIES NAMES sz szip)
                if(SZIP_LIBRARIES)
                        set(CMAKE_REQUIRED_LIBRARIES ${CMAKE_REQUIRED_LIBRARIES} ${SZIP_LIBRARIES})
                        check_library_exists("${HDF4_LIBRARIES}" Hopen "" HDF4_W_SZIP)
                        if(HDF4_W_SZIP)
                                set(HDF4_EXTRA_LIBRARIES ${CMAKE_REQUIRED_LIBRARIES})
                                find_package_handle_standard_args(HDF4 DEFAULT_MSG HDF4_LIBRARIES HDF4_EXTRA_LIBRARIES HDF4_INCLUDE_DIR)
                        endif(HDF4_W_SZIP)
                endif(SZIP_LIBRARIES)
        endif(HDF4_WO_SZIP)
        set(CMAKE_REQUIRED_LIBRARIES)
     endif(HDF4_LIBRARIES)


    set(HDF5_LIBRARIES ${H5_STATIC_LIBS})
    set(HDF4_LIBRARIES ${H4_STATIC_LIBS} ${SZIP_STATIC_LIBRARIES} ${JPEG_STATIC_LIBRARIES})

    message("HDF5: LIBS: ${HDF5_LIBRARIES} INC: ${HDF5_INCLUDE_DIR}")
    message("HDF4: LIBS: ${HDF4_LIBRARIES} INC: ${HDF4_INCLUDE_DIR}")

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
if(NOT USE_STATIC_LIBRARIES)
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
