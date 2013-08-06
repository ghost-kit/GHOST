cmake_minimum_required(VERSION 2.8.9)
project(GHOST)

###DEVELOPMENT SWITCHES####
if(JOSH_MURPHY)
    if(APPLE)
        set(CMAKE_OSX_DEPLOYMENT_TARGET "10.6")
        set(CMAKE_OSX_SYSROOT "/Applications/Xcode-4.3.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.6.sdk")
    endif()
endif()
###END DEVELOPMENT SWITCHES###

###########################################################################################################
## This file is the master build file for the GHOST ParaView plugin.  It is partially derived from the   ##
## ParaViewSuperbuild CMakeLists.txt file distributed in the ParaViewSuperbuild package.                 ##
###########################################################################################################


if(APPLE)
    if(NOT CMAKE_OSX_DEPLOYMENT_TARGET)
        message(WARNING "Ensure that CMAKE_OSX_SYSROOT, CMAKE_OSX_DEPLOYMENT_TARGET are set correctly")
        set(CMAKE_OSX_ARCHITECTURES "x86_64" CACHE STRING "By default, build for 64-bit Leopard")
    endif()
endif()

set(GHOST_CMAKE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/CMake")
set(GHOST_PROJECTS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/projects")
set(GHOST_PV_CMAKE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Utilities/ParaView")
set(GHOST_BINARY_DIR "${GHOST_BINARY_DIR}")

set(platform)
if(APPLE)
    set(platform "apple")
elseif(UNIX)
    set(platform "unix")
elseif(WIN32)
    set(platform "win32")
endif()


###------------------------------------------------------------------------------------------------------
# Setup CMAKE_MODULE_PATH

set(CMAKE_MODULE_PATH
  "${GHOST_PROJECTS_DIR}/${platform}"
  "${GHOST_PROJECTS_DIR}/common"
  "${GHOST_PROJECTS_DIR}"
  "${GHOST_CMAKE_DIR}"
  "${GHOST_PV_CMAKE_DIR}"
  "${CMAKE_CURRENT_SOURCE_DIR}"
  ${CMAKE_MODULE_PATH})


###------------------------------------------------------------------------------------------------------
# setup some standard variables
set(install_location "${CMAKE_CURRENT_BINARY_DIR}/install")
set (download_location "${CMAKE_CURRENT_BINARY_DIR}/downloads"
     CACHE PATH "Location where source tar-balls are (to be) downloaded.")
mark_as_advanced(download_location)

###------------------------------------------------------------------------------------------------------

if(WIN32)
    set(ldflags)
    set(cppflags)
    set(cxxflags)
    set(ld_library_path)
else()
    set(ldflags "$ENV{LDFLAGS} -L${install_location}/lib")
    set(cppflags "$ENV{CPPFLAGS} -fPIC")
    set(cxxflags "$ENV{CXXFLAGS} -fPIC")
    set(cflags "$ENV{CFLAGS} -fPIC")
    set(ld_library_path "${install_location}/lib:$ENV{LD_LIBRARY_PATH}")
endif()

if(UNIX AND NOT APPLE)
    option(PASS_LD_LIBRARY_PATH_FOR_BUILDS
         "Pass LD_LIBRARY_PATH to build scripts."
         ON)
    mark_as_advanced(PASS_LD_LIBRARY_PATH_FOR_BUILDS)
endif()

if(APPLE)
  set(cflags "${cflags} -arch x86_64 -mmacosx-version-min=${CMAKE_OSX_DEPLOYMENT_TARGET} -sysroot=${CMAKE_OSX_SYSROOT} ")
  set(cppflags "${cppflags} -arch x86_64 -mmacosx-version-min=${CMAKE_OSX_DEPLOYMENT_TARGET} -sysroot=${CMAKE_OSX_SYSROOT} ")
  set(cxxflags "${cxxflags} -arch x86_64 -mmacosx-version-min=${CMAKE_OSX_DEPLOYMENT_TARGET} -sysroot=${CMAKE_OSX_SYSROOT} ")
endif()
set(prefix_path "${install_location}")

#merge in default/user-specified CMAKE flags
set(ldflags "${CMAKE_SHARED_LINKER_FLAGS} ${ldflags}")
set(cppflags "${CMAKE_CXX_FLAGS} ${cppflags}")
set(cxxflags "${CMAKE_CXX_FLAGS} ${cxxflags}")
set(cflags "${CMAKE_C_FLAGS} ${cflags}")

string(STRIP "${ldflags}" ldflags)
string(STRIP "${cppflags}" cppflags)
string(STRIP "${cxxflags}" cxxflags)
string(STRIP "${cflags}"  cflags)

###-------------------------------------------------------------------------------------------------------
include(ParaViewModules)
include(dep_versions)

###-------------------------------------------------------------------------------------------------------
# Projects to be built
set (CM_PROJECTS_ALL)

if(NOT WIN32)
  # Check if there's a Fortran compiler. If there is, CMAKE_Fortran_COMPILER
  # will be set. Note that as of CMake 2.8.11 that CMAKE_Fortran_COMPILER
  # isn't passed to the external projects so setting it to something else
  # will give unexpected results. This is CMake Mantis issue 14,231.
    include(CheckFortran)
endif()

include(hdf4)

process_dependencies()

####
#if(QTCREATOR)
#    set(ParaView_DIR "/home/jjm390/Developer/ParaView/build401/paraview/src/paraview-build")
#endif()


###OPTIONS##
##Turn on or off the components of the tool kit
#option(GHOST_BUILD_LFM "Build the ParaView LFM Reader" ON)
#option(GHOST_BUILD_ENLIL "Build the ParaView ENLIL Reader" ON)
#option(GHOST_BUILD_CDAWEB "Build the ParaView CDAweb Tool Kit" ON)
#option(GHOST_BUILD_STATIC "Build the plugin with static libraries." ON)


##Disallow in-source builds
#if("${GHOST_SOURCE_DIR}" STREQUAL "${GHOST_BINARY_DIR}")
#    message (FATAL_ERROR
#        "GhostKit REQUIRES an out of source Build."
#        "Please Create a separate binary build directory and run CMake there.")
#endif()

##Set Output Direcotries
#set(CMAKE_RUNTIME_OUTPUT_DIRECOTORY "${CMAKE_BINARY_DIR}/bin")

#if(UNIX)
#    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY  "${CMAKE_BINARY_DIR}/lib")
#else()
#    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
#endif()
#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY  "${CMAKE_BINARY_DIR}/lib")

##Utilities for include
#include(Utilities/Git/Git.cmake)
#include(Utilities/ParaView/ParaViewDetermineVersion.cmake)
#include(CMakeDependentOption)

###shared files
#set(SHARED_BUILD_DATETIME ON CACHE INTERNAL "")
#set(SHARED_BUILD_IO ON CACHE INTERNAL "")
#set(SHARED_BUILD_CXFORM ON CACHE INTERNAL "")
#set(SHARED_USE_FPIC ON CACHE INTERNAL "")
#add_subdirectory("${PROJECT_SOURCE_DIR}/Shared")
#INCLUDE_DIRECTORIES(${SHARED_INCLUDE_DIR})
#set(LIBS ${LIBS} ${SHARED_LIBRARIES})

##Get ParaView (so we can build our module!!)
#if(NOT ParaView_BINARY_DIR)
#    find_package(ParaView REQUIRED)
#    include(${PARAVIEW_USE_FILE})
#endif()

##Versioning of the plugin
#set(ORGANIZATION "LASP & HAO")
#set(GHOST_VERSION_MAJOR 1)
#set(GHOST_VERSION_MINOR 0)
#set(GHOST_VERSION_PATCH 0)
#set(GHOST_VERSION_PATCH_EXTRA "beta 1")
#set(GHOST_VERSION "1.0")
#set(GHOST_VERSION_FULL "1.0.0 Beta 1")

#determine_version(${CMAKE_SOURCE_DIR} ${GIT_EXECUTABLE} "GHOST")

#message(STATUS "Using Git Executable: ${GIT_EXECUTABLE}")
#message(STATUS "BUILDING VERSION ${GHOST_VERSION_FULL}")

#message(STATUS "Building Selected Modules into GHOST.
#If Problems are Encountered while building,
#Try deleting your CMakeCache.txt file and rebuilding")

##Collect the information on the components
##----------------------------------------#
##LFM Reader
#if(GHOST_BUILD_LFM)
#    message(STATUS "Building LFM Reader Module")
#    include(lfm/LFM_Files.cmake)

#    #Need LFM
#    include(lfm/LFM_Files.cmake)


#endif()

##ENLIL Reader
#if(GHOST_BUILD_ENLIL)
#    message(STATUS "Building ENLIL Reader Module")

#    #Need Enlil
#    set(LIBS ${LIBS} vtkNetCDF_cxx)
#    include(enlil/Enlil_Files.cmake)

#endif()

##CDAweb ToolKit
#if(GHOST_BUILD_CDAWEB)
#    message(STATUS "Building CDAweb ToolKit Module")

#    #Need CDAweb
#    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_LIST_DIR}/CDAweb/cmake)
#    if(GHOST_BUILD_STATIC)
#        set(CDF_USE_STATIC_LIBS true)
#    endif()

#    find_package(CDF REQUIRED)
#    set(LIBS ${LIBS} ${CDF_LIBRARIES})
#    include(CDAweb/CDAweb_Files.cmake)
#endif()

###gather all built files
#set(SERVER_MAN_XML_LIST ${ENLIL_SM_XML}
#                        ${LFM_SM_XML}
#                        ${CDAWebTK_SM_XML}
#    )

#set(GUI_RF_XML_LIST     ${ENLIL_GUI_XML}
#                        ${LFM_GUI_XML}
#    )

#set(GUI_INTERFACE_LIST  ${CDAWebTK_PROP_IFACES}
#    )

#set(SERVER_MAN_SRC_LIST ${ENLIL_SM_SRC}
#                        ${LFM_SM_SRC}
#                        ${CDAWebTK_SM_SRC}
#    )

#set(ADD_SRC_LIST        ${ENLIL_SRC}
#                        ${CDAWebTK_SRC}
#    )


### Add the Plugin Definitions
#ADD_PARAVIEW_PLUGIN(GhostKit                ${GHOST_VERSION_FULL}
#                    SERVER_MANAGER_XML      ${SERVER_MAN_XML_LIST}
#                    GUI_RESOURCE_FILES      ${GUI_RF_XML_LIST}
#                    GUI_INTERFACES          ${GUI_INTERFACE_LIST}
#                    SERVER_MANAGER_SOURCES  ${SERVER_MAN_SRC_LIST}
#                    SOURCES                 ${ADD_SRC_LIST}
#                   )


#target_link_libraries(GhostKit ${LIBS})
