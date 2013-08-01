cmake_minimum_required(VERSION 2.8.9)
project(ghost-kit)

#Turn on or off the components of the tool kit
option(GHOST_BUILD_LFM "Build the ParaView LFM Reader" ON)
option(GHOST_BUILD_ENLIL "Build the ParaView ENLIL Reader" ON)
option(GHOST_BUILD_CDAWEB "Build the ParaView CDAweb Tool Kit" ON)


#Collect the information on the components

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



