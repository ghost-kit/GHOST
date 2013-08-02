#Files List for Enlil Reader

if(NOT ENLIL_SOURCE_DIR)
    set(ENLIL_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR})
    INCLUDE_DIRECTORIES(${CMAKE_CURRENT_LIST_DIR}/src)
endif()


set(ENLIL_SM_XML
        ${ENLIL_SOURCE_DIR}/xml/vtkEnlilReader.xml
    )

set(ENLIL_SRC
        ${ENLIL_SOURCE_DIR}/src/readerCache.cxx
        ${ENLIL_SOURCE_DIR}/src/readerCacheManager.cxx
    )

set(ENLIL_SM_SRC
        ${ENLIL_SOURCE_DIR}/src/vtkEnlilReader.cxx
    )

set(ENLIL_GUI_XML
        ${ENLIL_SOURCE_DIR}/xml/vtkEnlilGUI.xml
    )

