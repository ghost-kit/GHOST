if(NOT gkSwFTpanels_SOURCE_DIR)
    set(gkSwFTpanels_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR})
    INCLUDE_DIRECTORIES(${CMAKE_CURRENT_LIST_DIR}/src)
    INCLUDE_DIRECTORIES(${CMAKE_CURRENT_LIST_DIR}/../../PVcommon)
    if(NOT ${GK_SOURCE_DIR})
        set(GK_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/../..)
    endif()
endif()

set(GK_COMMON_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/../../PVcommon)

include_directories(${gkSwFTpanels_SOURCE_DIR}/src)

#IF (PARAVIEW_QT_VERSION VERSION_GREATER "4")
#    QT5_WRAP_CPP(SW_MOC_SRCS src/SwFTdockPanel.h)
#    QT5_WRAP_UI(SW_UI_SRCS ui/SwFTdockPanel.ui)
#ELSE ()
    QT4_WRAP_CPP(SW_MOC_SRCS ${gkSwFTpanels_SOURCE_DIR}/src/SwFTdockPanel.h)
    QT4_WRAP_UI(SW_UI_SRCS ${gkSwFTpanels_SOURCE_DIR}/ui/SwFTdockPanel.ui)
#ENDIF ()

ADD_PARAVIEW_DOCK_WINDOW(
    SW_OUTIFACES_1
    SW_OUTSRCS_1
    CLASS_NAME SwFTdockPanel
    DOCK_AREA Left)

set(GK_SWFT_GUI_SOURCES
        ${SW_OUTSRCS_1}
        ${SW_MOC_SRCS}
        ${SW_UI_SRCS}
        ${gkSwFTpanels_SOURCE_DIR}/src/SwFTdockPanel.cpp
    )

set(GK_SWFT_INTERFACES
        ${SW_OUTIFACES_1}
    )
