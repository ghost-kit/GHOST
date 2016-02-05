CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
project(LFMreader)

message("BUILDING LFM Reader!!!")


if(DEP_TREE)
    message(STATUS "Configuring with DEP_TREE")
    set(PREFIX ${DEP_TREE}/install)
    include_directories(BEFORE SYSTEM ${DEP_TREE}/include)
    set(ParaView_DIR ${DEP_TREE}/paraview/src/paraview-build)
endif()

if(DEBUG)
    message(STATUS "Building with DEBUG symbols")
    set(CMAKE_BUILD_TYPE "debug")
else()
    message(STATUS "Building in RELEASE mode")
    set(CMAKE_BUILD_TYPE "release")
endif()

if( PREFIX )
  set( CMAKE_INCLUDE_PATH /usr/include ${PREFIX}/include ${CMAKE_INCLUDE_PATH} )
  set( CMAKE_LIBRARY_PATH /usr/lib ${PREFIX}/lib ${CMAKE_LIBRARY_PATH} )
endif( PREFIX )


FIND_PACKAGE(ParaView REQUIRED)
INCLUDE(${PARAVIEW_USE_FILE})

include(LFM_Files.cmake)

ADD_PARAVIEW_PLUGIN(LFMreader   "1.0.1"
        SERVER_MANAGER_XML      ${LFM_SM_XML}
        SERVER_MANAGER_SOURCES  ${LFM_SM_SRC}
        GUI_RESOURCE_FILES      ${LFM_GUI_XML}
        SOURCES                 ${LFM_SM_XML} ${LFM_GUI_XML}
    )

Target_LINK_LIBRARIES(LFMreader
    ${PREFIX}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}ltrDateTime${CMAKE_STATIC_LIBRARY_SUFFIX}
    ${PREFIX}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}io${CMAKE_STATIC_LIBRARY_SUFFIX}
    ${PREFIX}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}mfhdf${CMAKE_STATIC_LIBRARY_SUFFIX}
    ${PREFIX}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}df${CMAKE_STATIC_LIBRARY_SUFFIX}
    ${PREFIX}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}jpeg${CMAKE_STATIC_LIBRARY_SUFFIX}
    ${PREFIX}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}io${CMAKE_STATIC_LIBRARY_SUFFIX}
    ${PREFIX}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}sz${CMAKE_SHARED_LIBRARY_SUFFIX}
    ${PREFIX}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}hdf5${CMAKE_SHARED_LIBRARY_SUFFIX}
    ${PREFIX}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}hdf5_hl${CMAKE_SHARED_LIBRARY_SUFFIX}
    z
    )


add_custom_target(dist
    COMMAND ${CMAKE_COMMAND}  -E make_directory ${CMAKE_INSTALL_PREFIX}/release &&
            ${CMAKE_COMMAND}  -DBINARY_DIR:PATH=${CMAKE_CURRENT_BINARY_DIR}
                              -DPV_SUPERBUILD_LIST_DIR:PATH=${PV_SUPERBUILD_LIST_DIR}
                              -DPARAVIEW_BINARY_DIR:PATH=${ParaView_DIR}
                              -DQT_LIBRARY_DIR:PATH=${QT_LIBRARY_DIR}
                              -DTMP_DIR:PATH=${CMAKE_CURRENT_BINARY_DIR}
                              -DPROJECT_NAME:STRING=${CMAKE_PROJECT_NAME}
                              -DTARGET_NAME:STRING="LFMreader"
                              -Dbundle_name:STRING=${CMAKE_INSTALL_PREFIX}/release/LFMreader_plugin.tgz
                              -P ${CMAKE_CURRENT_LIST_DIR}/install_lfm_mac.cmake &&
            ${CMAKE_COMMAND}  -E copy ${CMAKE_CURRENT_BINARY_DIR}/${TMP_DIR}/${PROJECT_NAME}/* ${CMAKE_INSTALL_PREFIX}/release/
)
