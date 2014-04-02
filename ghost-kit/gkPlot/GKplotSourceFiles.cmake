if(NOT gkPlotSource_SOURCE_DIR)
    set(gkPlotSource_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR})
    INCLUDE_DIRECTORIES(${CMAKE_CURRENT_LIST_DIR}/src)
    INCLUDE_DIRECTORIES(${CMAKE_CURRENT_LIST_DIR}/../../PVcommon)
    if(NOT ${GK_SOURCE_DIR})
        set(GK_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/../..)
    endif()
endif()

set(GK_COMMON_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/../../PVcommon)

QT4_WRAP_CPP(
    GKplotSource_MOC_BUILT_SOURCES
        ${gkPlotSource_SOURCE_DIR}/src/gkStringProperty.h
    )

QT4_WRAP_UI(
    GKplotSource_UI_BUILT_SOURCES
        ${gkPlotSource_SOURCE_DIR}/src/gkStringProperty.ui
    )


set(GKplotSource_outifaces0)
set(GKplotSource_outsrcs0)
ADD_PARAVIEW_PROPERTY_WIDGET(GKplotSource_outifaces0 GKplotSource_outsrcs0
        TYPE "GKStringBox"
        CLASS_NAME "gkStringProperty")


set(GKplotSource_PROP_IFACES
        ${GKplotSource_outifaces0}
    )

set(GKplotSource_PROP_SRC
        ${GKplotSource_outsrcs0}
    )

set(GKplotSource_SRC
        ${GKplotSource_MOC_BUILT_SOURCES}
        ${GKplotSource_UI_BUILT_SOURCES}
        ${GKplotSource_PROP_SRC}
        ${gkPlotSource_SOURCE_DIR}/src/gkStringProperty.cpp
    )



set(GKplotSource_SM_SRC
    ${gkPlotSource_SOURCE_DIR}/src/gkPlotSource.cpp
    )


set(GKplotSource_SM_XML
    ${gkPlotSource_SOURCE_DIR}/xml/gkPlotSource.xml
    )
