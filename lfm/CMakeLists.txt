CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
project(lfm)

# Required variables:
#  * ParaView_DIR: Path to directory containing ParaViewConfig.cmake
#  * ParaView_SuperBuild: Mac OS-X only:  SuperBuild is shipped with fixup_plugin.py, which is part of the SuperBuild.
#  * PREFIX:  Path to relevant libraries (HDF4, JPEG, etc)
#
# There are two ways to set required variables for building this plugin:
#  1.  Pass via command-line arguments (recommended):
#  
#      cmake .. -DParaView_DIR=/Users/schmitt/paraview/opt/ParaView-3.98.1_OSX-10.7/paraview/src/paraview-build \
#               -DParaView_SuperBuild_DIR=/Users/schmitt/paraview/opt/src/ParaViewSuperbuild-3.98.0 \
#               -PREFIX=/Users/schmitt/paraview/opt
#  
#  2.  Hard-code it in this file by uncommenting the following lines:

#set(ParaView_DIR /Users/schmitt/paraview/opt/ParaView-3.98.1_OSX-10.7/paraview/src/paraview-build)
#set(ParaView_SuperBuild_DIR /Users/schmitt/paraview/opt/src/ParaViewSuperbuild-3.98.1)
#set(ParaView_DIR /Users/schmitt/paraview/opt/ParaView-3.98.0_OSX-10.7/paraview/src/paraview-build)
#set(ParaView_SuperBuild_DIR /Users/schmitt/paraview/opt/src/ParaViewSuperbuild-3.98.0)

########################################################################

if( PREFIX )
  set( CMAKE_INCLUDE_PATH /usr/include ${PREFIX}/include ${CMAKE_INCLUDE_PATH} )
  set( CMAKE_LIBRARY_PATH /usr/lib ${PREFIX}/lib ${CMAKE_LIBRARY_PATH} )
endif( PREFIX )

##CACHE SETTINGS! ##
message(WARNING "IF you are having problems building the shared repo, try deleting your CMakeCache.txt file and rebuilding")
set(SHARED_USE_FPIC ON CACHE INTERNAL "")
set(SHARED_BUILD_IO ON CACHE INTERNAL "")

## Add shared Library ##
add_subdirectory(Shared)

include_directories( ${SHARED_INCLUDE_DIR} )
message(STATUS "Shared Include: ${SHARED_INCLUDE_DIR}")
message(STATUS "Shared LIBRARY: ${SHARED_LIBRARIES}")

FIND_PACKAGE(ParaView REQUIRED)

INCLUDE(${PARAVIEW_USE_FILE})

include(LFM_Files.cmake)

ADD_PARAVIEW_PLUGIN(vtkLFMReader "1.0"
        SERVER_MANAGER_XML      ${LFM_SM_XML} #vtkLFMReader.xml
        SERVER_MANAGER_SOURCES  ${LFM_SM_SRC} #vtkLFMReader.cxx
        GUI_RESOURCE_FILES      ${LFM_GUI_XML} #vtkLFMGUI.xml
    )

Target_LINK_LIBRARIES(vtkLFMReader 
  io
  ${SHARED_LIBRARIES}
#  vtkhdf5
)
