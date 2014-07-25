project(lfm_hdf5)

if(HAL)
    set(PREFIX "/home/jjm390/Developer/gkbuild/install")
    set(ParaView_DIR "${PREFIX}/../paraview/src/paraview-build")
    set(CMAKE_INSTALL_PREFIX "/home/jjm390/Developer/gkbuild/install")
    set(PV_SUPERBUILD_LIST_DIR "/home/jjm390/Developer/GHOST")
endif()

if( PREFIX )
  set(CMAKE_INCLUDE_PATH /usr/include ${PREFIX}/include ${CMAKE_INCLUDE_PATH})
  set(CMAKE_LIBRARY_PATH /usr/lib ${PREFIX}/lib ${CMAKE_LIBRARY_PATH})
endif()

FIND_PACKAGE(ParaView REQUIRED)
INCLUDE(${PARAVIEW_USE_FILE})

include(LFM_hdf5_Files.cmake)

ADD_PARAVIEW_PLUGIN(lfmHdf5Reader   "1.0"
        SERVER_MANAGER_XML      ${LFMHDF5_SM_XML}
        SERVER_MANAGER_SOURCES  ${LFMHDF5_SM_SRC}
        GUI_RESOURCE_FILES      ${LFMHDF5_GUI_XML}
        SOURCES                 ${LFMHDF5_SM_XML} ${LFMHDF5_GUI_XML}
    )



