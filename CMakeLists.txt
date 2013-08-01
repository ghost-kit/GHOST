cmake_minimum_required(VERSION 2.8.9)
project(GhostKit)

#Disallow in-source builds
if("${GhostKit_SOURCE_DIR}" STREQUAL "${GhostKit_BINARY_DIR}")
    message (FATAL_ERROR
        "GhostKit REQUIRES an out of source Build."
        "Please Create a separate binary build directory and run CMake there.")
endif()

#Set Output Direcotries
set(CMAKE_RUNTIME_OUTPUT_DIRECOTORY "${CMAKE_BINARY_DIR}/bin")

if(UNIX)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY  "${CMAKE_BINARY_DIR}/lib")
else()
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
endif()
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY  "${CMAKE_BINARY_DIR}/lib")

#Utilities for include
include(Utilities/Git/Git.cmake)
include(Utilities/ParaView/ParaViewDetermineVersion.cmake)
include(CMakeDependentOption)

#Get ParaView (so we can build our module!!)
if(NOT ParaView_BINARY_DIR)
    find_package(ParaView REQUIRED)
    include(${PARAVIEW_USE_FILE})
endif()

#Versioning of the plugin
set(ORGANIZATION "LASP & HAO")
set(GHOST_VERSION_MAJOR 1)
set(GHOST_VERSION_MINOR 0)
set(GHOST_VERSION_PATCH 0)
set(GHOST_VERSION_PATCH_EXTRA "beta 1")
set(GHOST_VERSION "1.0")
set(GHOST_VERSION_FULL "1.0.0 Beta 1")

determine_version(${CMAKE_SOURCE_DIR} ${GIT_EXECUTABLE} "GHOST")

message(STATUS "Using Git Executable: ${GIT_EXECUTABLE}")
message(STATUS "BUILDING VERSION ${GHOST_VERSION_FULL}")

#Turn on or off the components of the tool kit
option(GHOST_BUILD_LFM "Build the ParaView LFM Reader" ON)
option(GHOST_BUILD_ENLIL "Build the ParaView ENLIL Reader" ON)
option(GHOST_BUILD_CDAWEB "Build the ParaView CDAweb Tool Kit" ON)


#Collect the information on the components
#----------------------------------------#
#LFM Reader
if(GHOST_BUILD_LFM)
    message(STATUS "Building LFM Reader Module")



endif()

#ENLIL Reader
if(GHOST_BUILD_ENLIL)
    message(STATUS "Building ENLIL Reader Module")
endif()

#CDAweb ToolKit
if(GHOST_BUILD_CDAWEB)
    message(STATUS "Building CDAweb ToolKit Module")
endif()



## Add the Plugin Definitions
ADD_PARAVIEW_PLUGIN(GHOST                   ${GHOST_VERSION_FULL}
                    SERVER_MANAGER_XML      ${SERVER_MAN_XML_LIST}
                    GUI_RESOURCE_FILES      ${GUI_RF_XML_LIST}
                    GUI_INTERFACES          ${GUI_INTERFACE_LIST}
                    SERVER_MANAGER_SOURCES  ${SERVER_MAN_SRC_LIST}
                    SOURCES                 ${ADD_SRC_LIST}
                   )
