#This plugin provides SwFT functionality to ParaView, and relies on Python.  Python MUST be active in paraview for this plugin to function. GHOSTkit is also required.

project(gkSwFTpanels)
cmake_minimum_required(VERSION 2.8.9 FATAL_ERROR)

if(NOT PREFIX)
 #set(PREFIX /Volumes/Data/Development/GHOST-Kit/build/install/)
endif()

set( CMAKE_INCLUDE_PATH /usr/include ${PREFIX}/include ${CMAKE_INCLUDE_PATH} )
set( CMAKE_LIBRARY_PATH /usr/lib ${PREFIX}/lib ${CMAKE_LIBRARY_PATH} )
set( ParaView_DIR ${PREFIX}/../paraview/src/paraview-build)

#Remove before finishing
set( CMAKE_INSTALL_PREFIX /tmp)
set( PV_SUPERBUILD_LIST_DIR /Volumes/Data/Users/jjm390/Developer/ghost-kit/GHOST)


if(NOT ParaView_BINARY_DIR)
  find_package(ParaView REQUIRED)
  include(${PARAVIEW_USE_FILE})
endif()

#needed to build plugins
include(ParaViewPlugins)

#get file list
INCLUDE_DIRECTORIES(src)

IF(PARAVIEW_BUILD_QT_GUI)

    include(GKSwFTSourceFiles.cmake)

    ADD_PARAVIEW_PLUGIN(SwFTdockPanel "1.0b"
                      GUI_INTERFACES ${SW_OUTIFACES_1}
                      GUI_SOURCES ${SW_OUTSRCS_1} ${SW_MOC_SRCS} ${SW_UI_SRCS} src/SwFTdockPanel.cpp)
ENDIF(PARAVIEW_BUILD_QT_GUI)



###Fix up code
add_custom_target(dist
    COMMAND ${CMAKE_COMMAND}  -E make_directory ${CMAKE_INSTALL_PREFIX}/release &&
            ${CMAKE_COMMAND}  -DBINARY_DIR:PATH=${CMAKE_CURRENT_BINARY_DIR}
                              -DPV_SUPERBUILD_LIST_DIR:PATH=${PV_SUPERBUILD_LIST_DIR}
                              -DPARAVIEW_BINARY_DIR:PATH=${ParaView_DIR}
                              -DQT_LIBRARY_DIR:PATH=${QT_LIBRARY_DIR}
                              -DTMP_DIR:PATH=${CMAKE_CURRENT_BINARY_DIR}
                              -DPROJECT_NAME:STRING=${CMAKE_PROJECT_NAME}
                              -DTARGET_NAME:STRING="SwFTdockPanel"
                              -Dbundle_name:STRING=${CMAKE_INSTALL_PREFIX}/release/gkSwFT_plugin.tgz
                              -P ${CMAKE_CURRENT_LIST_DIR}/install_gkSwFT_mac.cmake &&
            ${CMAKE_COMMAND}  -E copy ${CMAKE_CURRENT_BINARY_DIR}/${TMP_DIR}/${PROJECT_NAME}/* ${CMAKE_INSTALL_PREFIX}/release/
)





