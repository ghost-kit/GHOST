if(NOT gkPlotSource_SOURCE_DIR)
    set(gkPlotSource_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR})
    INCLUDE_DIRECTORIES(${CMAKE_CURRENT_LIST_DIR}/src)
    INCLUDE_DIRECTORIES(${CMAKE_CURRENT_LIST_DIR}/../../PVcommon)
    if(NOT ${GK_SOURCE_DIR})
        set(GK_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/../..)
    endif()
endif()

set(GK_COMMON_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/../../PVcommon)



set(GKplotSource_SRC

    )



set(GKplotSource_SM_SRC
    ${gkPlotSource_SOURCE_DIR}/src/gkPlotSource.cpp
    )


set(GKplotSource_SM_XML
    ${gkPlotSource_SOURCE_DIR}/xml/gkPlotSource.xml
    )
