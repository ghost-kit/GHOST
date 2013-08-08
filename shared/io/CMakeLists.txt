cmake_minimum_required(VERSION 2.8.9)
project(io)

if(QTCREATOR)
    file(GLOB IO_CMAKE "cmake/*")
endif()

#FIXME:  Use "find_package" for A++, MPI and Hdf4 rather than these options:
option (USE_APP "Use A++ and/or P++ for array storage" OFF)
option (USE_MPI "Build parallel I/O (Warning: leave this off, it's untested)" OFF)
option (USE_MASTER_BUILD "Use Master Build Libraries to build" OFF)

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
if(NOT USE_MASTER_BUILD)
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_LIST_DIR}/cmake/")
    find_package(HDF4 REQUIRED)

    message("SETTING AUTO-DETECTED LIBRARIES")

    if(HDF4_FOUND)
        message(STATUS "HDF4 Found!")
        message(STATUS "Linking HDF4 Libraries: ${HDF4_LIBRARIES}")
        message(STATUS "EXTRA HDF4 Lirbraries: ${HDF4_EXTRA_LIBRARIES}")

        find_package(JPEG)
        message(STATUS "JPEG: ${JPEG_LIBRARY}")

        find_package(ZLIB)
        set(HDF4_LIBRARIES ${HDF4_LIBRARIES} ${ZLIB_LIBRARY} ${JPEG_LIBRARY})

        add_definitions(-DHAS_HDF4)
    endif(HDF4_FOUND)

else()

    message("SETTING MASTER BUILD LIBRARIES")

    INCLUDE_DIRECTORIES(${CMAKE_INSTALL_PREFIX}/include)

    set(HDF4_LIBRARIES
            ${CMAKE_INSTALL_PREFIX}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}df${CMAKE_STATIC_LIBRARY_SUFFIX}
            ${CMAKE_INSTALL_PREFIX}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}mfhdf${CMAKE_STATIC_LIBRARY_SUFFIX}
            ${CMAKE_INSTALL_PREFIX}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}sz${CMAKE_STATIC_LIBRARY_SUFFIX}
            ${CMAKE_INSTALL_PREFIX}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}z${CMAKE_STATIC_LIBRARY_SUFFIX}
            ${CMAKE_STATIC_LIBRARY_PREFIX}m${CMAKE_STATIC_LIBRARY_SUFFIX}
        )

    set(JPEG_LIBRARIES
            ${CMAKE_INSTALL_PREFIX}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}jpeg${CMAKE_STATIC_LIBRARY_SUFFIX}
        )


    add_definitions(-DHAS_HDF4:BOOL=true)

endif()

########
# Hdf5 #
########################################################################
if(NOT USE_MASTER_BUILD)


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

else()

    set(HDF5_LIBRARIES
            ${CMAKE_INSTALL_PREFIX}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}hdf5${CMAKE_STATIC_LIBRARY_SUFFIX}
            ${CMAKE_INSTALL_PREFIX}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}hdf5_hl${CMAKE_STATIC_LIBRARY_SUFFIX}
            ${CMAKE_INSTALL_PREFIX}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}z${CMAKE_STATIC_LIBRARY_SUFFIX}
            ${CMAKE_STATIC_LIBRARY_PREFIX}m${CMAKE_STATIC_LIBRARY_SUFFIX}
        )

endif()

##############
# io library #
########################################################################
set( IO_INCLUDES ${HDF4_INCLUDE_DIR} ${HDF5_INCLUDE_DIRS} )
include_directories(${IO_INCLUDES})
set( IO_LIBRARIES
            ${HDF4_LIBRARIES}
            ${JPEG_LIBRARIES}
            ${HDF5_LIBRARIES}
            ${PHDF5_LIBRARIES}
            )

message(STATUS "IO: LINKING LIBRARIES: ${IO_LIBRARIES}")

add_library(io STATIC 
            ErrorQueue.h ErrorQueue.C
            Io.hpp Io.C
            IoTemplates.hpp Util.hpp
            Hdf.hpp Hdf.C Hdf4.hpp Hdf4.C
            Hdf5.hpp Hdf5.C
            PHdf5.hpp PHdf5.C
            ${IO_CMAKE} )

if(USE_MASTER_BUILD)
    message("LINKING LIBRARIES for MASTER BUILD")
    message("Libraries Linked: ${IO_LIBRARIES}")
    target_link_libraries(io ${IO_LIBRARIES})
endif()


###########
# cxxtest #
########################################################################
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
        target_link_libraries(testNoppNompi io ${HDF4_LIBRARIES} ${IO_LIBRARIES} )
    else()
        message ( STATUS "Could NOT find CxxTest - unit testing not available" )
    endif()
