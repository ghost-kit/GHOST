if(NOT cxformFilter_SOURCE_DIR)
    set(cxformFilter_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR})
    INCLUDE_DIRECTORIES(${CMAKE_CURRENT_LIST_DIR}/src)
endif()


set(GKcxForm_SM_XML
    ${cxformFilter_SOURCE_DIR}/xml/gk_cxform.xml
    ${cxformFilter_SOURCE_DIR}/xml/gk_cxFormField.xml)

set(GKcxForm_SM_SRC
    ${cxformFilter_SOURCE_DIR}/src/gk_cxform.cpp
    ${cxformFilter_SOURCE_DIR}/src/gk_cxFormField.cpp)
