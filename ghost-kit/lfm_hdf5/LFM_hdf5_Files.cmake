if(NOT LFMHDF5_SOURCE_DIR)
    set(LFMHDF5_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR})
    INCLUDE_DIRECTORIES(${CMAKE_CURRENT_LIST_DIR}/src)
endif()


set(LFMHDF5_SM_XML
    ${LFMHDF5_SOURCE_DIR}/xml/lfm_hdf5_reader.xml)

set(LFMHDF5_SM_SRC
    ${LFMHDF5_SOURCE_DIR}/src/lfm_hdf5_reader.cpp)

set(LFMHDF5_GUI_XML
    ${LFMHDF5_SOURCE_DIR}/xml/lfm_hdf5_reader_gui.xml)
