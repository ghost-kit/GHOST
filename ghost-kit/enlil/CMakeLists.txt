CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT("ENLIL")

if(TMLMP8)
    set(PREFIX "/Users/developer/Developer/GK/GHOST-build/install")
    set(ParaView_DIR "/Users/developer/Developer/GK/build3/paraview/src/paraview-build")
    set(CMAKE_INSTALL_PREFIX "/Users/developer/Developer/GK/Install")
    set(PV_SUPERBUILD_LIST_DIR "/Users/developer/Developer/GK/GHOST")
endif()

if( PREFIX )
  set( CMAKE_INCLUDE_PATH /usr/include ${PREFIX}/include ${CMAKE_INCLUDE_PATH} )
  set( CMAKE_LIBRARY_PATH /usr/lib ${PREFIX}/lib ${CMAKE_LIBRARY_PATH} )
endif( PREFIX )

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_LIST_DIR}/src)


FIND_PACKAGE(ParaView REQUIRED)
INCLUDE(${PARAVIEW_USE_FILE})

include(Enlil_Files.cmake)

ADD_PARAVIEW_PLUGIN(ENLILreader "2.0.1"
  SERVER_MANAGER_XML        ${ENLIL_SM_XML}
  SERVER_MANAGER_SOURCES    ${ENLIL_SM_SRC}
  SOURCES                   ${ENLIL_SRC}
  GUI_RESOURCE_FILES        ${ENLIL_GUI_XML}
)

SET(KIT_LIBS  vtkNetCDF_cxx
              )

Target_LINK_LIBRARIES(ENLILreader ${KIT_LIBS}
                    ${PREFIX}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}boost_date_time${CMAKE_STATIC_LIBRARY_SUFFIX}
                    ${PREFIX}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}ltrDateTime${CMAKE_STATIC_LIBRARY_SUFFIX}
                )


add_custom_target(dist
    COMMAND ${CMAKE_COMMAND}  -E make_directory ${CMAKE_INSTALL_PREFIX}/release &&
            ${CMAKE_COMMAND}  -DBINARY_DIR:PATH=${CMAKE_CURRENT_BINARY_DIR}
                              -DPV_SUPERBUILD_LIST_DIR:PATH=${PV_SUPERBUILD_LIST_DIR}
                              -DPARAVIEW_BINARY_DIR:PATH=${ParaView_DIR}
                              -DQT_LIBRARY_DIR:PATH=${QT_LIBRARY_DIR}
                              -DTMP_DIR:PATH=${CMAKE_CURRENT_BINARY_DIR}
                              -DPROJECT_NAME:STRING=${CMAKE_PROJECT_NAME}
                              -DTARGET_NAME:STRING="ENLILreader"
                              -Dbundle_name:STRING=${CMAKE_INSTALL_PREFIX}/release/ENLILreader_plugin.tgz
                              -P ${CMAKE_CURRENT_LIST_DIR}/install_enlil_mac.cmake &&
            ${CMAKE_COMMAND}  -E copy ${CMAKE_CURRENT_BINARY_DIR}/${TMP_DIR}/${PROJECT_NAME}/* ${CMAKE_INSTALL_PREFIX}/release/
)
