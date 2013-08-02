#File for LFM Reader

if(NOT LFM_SOURCE_DIR)
    set(LFM_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR})
    INCLUDE_DIRECTORIES(${CMAKE_CURRENT_LIST_DIR}/src)
endif()



set(LFM_SM_XML
        ${LFM_SOURCE_DIR}/xml/vtkLFMReader.xml)
set(LFM_SM_SRC
        ${LFM_SOURCE_DIR}/src/vtkLFMReader.cxx)
set(LFM_GUI_XML
        ${LFM_SOURCE_DIR}/xml/vtkLFMGUI.xml)

