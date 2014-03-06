if(NOT cxformFilter_SOURCE_DIR)
    set(cxformFilter_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR})
    INCLUDE_DIRECTORIES(${CMAKE_CURRENT_LIST_DIR}/src)
    INCLUDE_DIRECTORIES(${CMAKE_CURRENT_LIST_DIR}/../../PVCommon)
endif()

if(GK_SOURCE_DIR)
    message(STATUS "Found GK_SOURCE_DIR... setting GK_COMMON_SOURCE_DIR")
    set(GK_COMMON_SOURCE_DIR ${GK_SOURCE_DIR}/PVcommon)
endif()


message("${GK_COMMON_SOURCE_DIR}")

QT4_WRAP_CPP(
    GKcxForm_MOC_BUILT_SOURCES
        ${GK_COMMON_SOURCE_DIR}/gk_dropDownBoxWidget.h
    )

QT4_WRAP_UI(
    GKcxForm_UI_BUILT_SOURCES
        ${GK_COMMON_SOURCE_DIR}/gk_dropDownBoxWidget.ui

    )

# Property Widgets
set(GKcxForm_outifaces0)
set(GKcxForm_outsrcs0)
ADD_PARAVIEW_PROPERTY_WIDGET(GKcxForm_outifaces0 GKcxForm_outsrcs0
        TYPE "GK_DropDownBox"
        CLASS_NAME "gk_dropDownBoxWidget")

set(GKcxForm_PROP_IFACES
        ${GKcxForm_outifaces0}
    )

set(GKcxForm_PROP_SRC
        ${GKcxForm_outsrcs0}
    )

message(STATUS "IFACES: ${GKcxForm_PROP_IFACES}")
message(STATUS "PROP:   ${GKcxForm_PROP_SRC}")


set(GKcxForm_SRC
     ${GKcxForm_UI_BUILT_SOURCES}
     ${GKcxForm_MOC_BUILT_SOURCES}
     ${GKcxForm_PROP_SRC}
     ${GK_COMMON_SOURCE_DIR}/gk_dropDownBoxWidget.cpp
    )

message(STATUS "SRC: ${GKcxForm_SRC}")

set(GKcxForm_SM_XML
    ${cxformFilter_SOURCE_DIR}/xml/gk_cxform.xml
    ${cxformFilter_SOURCE_DIR}/xml/gk_cxFormField.xml)

message(STATUS "XML: ${GKcxForm_SM_XML}")

set(GKcxForm_SM_SRC
    ${cxformFilter_SOURCE_DIR}/src/gk_cxform.cpp
    ${cxformFilter_SOURCE_DIR}/src/gk_cxFormField.cpp)

message(STATUS "SM SRC: ${GKcxForm_SM_SRC}")

