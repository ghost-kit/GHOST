#Files List for CDAweb Tool Kit

if(NOT CDAWebTK_SOURCE_DIR)
    set(CDAWebTK_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR})
    INCLUDE_DIRECTORIES(${CMAKE_CURRENT_LIST_DIR}/src)
endif()

# wrap QT
QT4_WRAP_CPP(
    CDAWebTK_MOC_BUILT_SOURCES
        ${CDAWebTK_SOURCE_DIR}/src/filterNetworkAccessModule.h
        ${CDAWebTK_SOURCE_DIR}/src/ScInfoPropWidget.h
        ${CDAWebTK_SOURCE_DIR}/src/status.h
        ${CDAWebTK_SOURCE_DIR}/src/filedownloader.h
        ${CDAWebTK_SOURCE_DIR}/src/vtkSpaceCraftInfo.h
        ${CDAWebTK_SOURCE_DIR}/src/pathSelector.h
        ${CDAWebTK_SOURCE_DIR}/src/newFileChooserWidget.h
    )

# Wrap UI
QT4_WRAP_UI(
    CDAWebTK_UI_BUILT_SOURCES
        ${CDAWebTK_SOURCE_DIR}/ui/ScInfoPropWidget.ui
        ${CDAWebTK_SOURCE_DIR}/ui/status.ui
        ${CDAWebTK_SOURCE_DIR}/ui/pathSelector.ui
    )

# Property Widgets
set(CDAWebTK_outifaces0)
set(CDAWebTK_outsrcs0)
ADD_PARAVIEW_PROPERTY_WIDGET(CDAWebTK_outifaces0 CDAWebTK_outsrcs0
        TYPE "SpaceCraftInfoProperties"
        CLASS_NAME "ScInfoPropWidget")

set(CDAWebTK_outifaces1)
set(CDAWebTK_outsrcs1)
ADD_PARAVIEW_PROPERTY_WIDGET(CDAWebTK_outifaces1 CDAWebTK_outsrcs1
        TYPE "PathSelectionProperty"
        CLASS_NAME "pathSelector")

# Set up files for compilation
set(CDAWebTK_SM_XML
        ${CDAWebTK_SOURCE_DIR}/xml/SpaceCraftInfoFilter.xml
        ${CDAWebTK_SOURCE_DIR}/xml/SpaceCraftInfoSource.xml
    )

set(CDAWebTK_PROP_IFACES
        ${CDAWebTK_outifaces0}
        ${CDAWebTK_outifaces1}
    )

set(CDAWebTK_PROP_SRC
        ${CDAWebTK_outsrcs0}
        ${CDAWebTK_outsrcs1}
    )

set(CDAWebTK_QT_SRC
        ${CDAWebTK_MOC_BUILT_SOURCES}
        ${CDAWebTK_UI_BUILT_SOURCES}
    )

set(CDAWebTK_SRC
        ${CDAWebTK_SOURCE_DIR}/src/filterNetworkAccessModule.cxx
        ${CDAWebTK_SOURCE_DIR}/src/vtkSpaceCraftInfo.cxx
        ${CDAWebTK_SOURCE_DIR}/src/ScInfoPropWidget.cxx
        ${CDAWebTK_SOURCE_DIR}/src/status.cxx
        ${CDAWebTK_SOURCE_DIR}/src/filedownloader.cxx
        ${CDAWebTK_SOURCE_DIR}/src/timefithandler.cxx
        ${CDAWebTK_SOURCE_DIR}/src/BadDataHandler.cxx
        ${CDAWebTK_SOURCE_DIR}/src/cdfDataReader.cxx
        ${CDAWebTK_SOURCE_DIR}/src/omitBDHandler.cxx
        ${CDAWebTK_SOURCE_DIR}/src/pathSelector.cxx
        ${CDAWebTK_SOURCE_DIR}/src/newFileChooserWidget.cxx
        ${CDAWebTK_MOC_BUILT_SOURCES}
        ${CDAWebTK_UI_BUILT_SOURCES}
        ${CDAWebTK_QT_SRC}
    )

set(CDAWebTK_SM_SRC
        ${CDAWebTK_SOURCE_DIR}/src/vtkSpaceCraftInfoFilter.cxx
        ${CDAWebTK_SOURCE_DIR}/src/vtkSpaceCraftInfoSource.cxx
    )
