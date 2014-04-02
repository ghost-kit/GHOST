project(gkPlotSource)
cmake_minimum_required(VERSION 2.8.9 FATAL_ERROR)

if(NOT PREFIX)
 #set(PREFIX /Volumes/Data/Development/GHOST-Kit/build/install/)
endif()

set( CMAKE_INCLUDE_PATH /usr/include ${PREFIX}/include ${CMAKE_INCLUDE_PATH} )
set( CMAKE_LIBRARY_PATH /usr/lib ${PREFIX}/lib ${CMAKE_LIBRARY_PATH} )
set( ParaView_DIR ${PREFIX}/../paraview/src/paraview-build)

if(NOT ParaView_BINARY_DIR)
  find_package(ParaView REQUIRED)
  include(${PARAVIEW_USE_FILE})
endif()

#needed to build plugins
include(ParaViewPlugins)

#get file list
INCLUDE_DIRECTORIES(src)

include(GKplotSourceFiles.cmake)

set(XML_FILES xml/gkPlotSource.xml)


#plugin
ADD_PARAVIEW_PLUGIN(gkPlotSource  "1.0.0.beta1"
                    SERVER_MANAGER_XML      ${GKplotSource_SM_XML}
                    GUI_INTERFACES          ${GKplotSource_PROP_IFACES}
                    SERVER_MANAGER_SOURCES  ${GKplotSource_SM_SRC}
                    SOURCES ${GKplotSource_SRC} ${XML_FILES}
                )


add_custom_target(dist
    COMMAND ${CMAKE_COMMAND}  -E make_directory ${CMAKE_INSTALL_PREFIX}/release &&
            ${CMAKE_COMMAND}  -DBINARY_DIR:PATH=${CMAKE_CURRENT_BINARY_DIR}
                              -DPV_SUPERBUILD_LIST_DIR:PATH=${PV_SUPERBUILD_LIST_DIR}
                              -DPARAVIEW_BINARY_DIR:PATH=${ParaView_DIR}
                              -DQT_LIBRARY_DIR:PATH=${QT_LIBRARY_DIR}
                              -DTMP_DIR:PATH=${CMAKE_CURRENT_BINARY_DIR}
                              -DPROJECT_NAME:STRING=${CMAKE_PROJECT_NAME}
                              -DTARGET_NAME:STRING="gkPlotSource"
                              -Dbundle_name:STRING=${CMAKE_INSTALL_PREFIX}/release/gkPlotSource_plugin.tgz
                              -P ${CMAKE_CURRENT_LIST_DIR}/install_gkPlotSource_mac.cmake &&
            ${CMAKE_COMMAND}  -E copy ${CMAKE_CURRENT_BINARY_DIR}/${TMP_DIR}/${PROJECT_NAME}/* ${CMAKE_INSTALL_PREFIX}/release/
)
