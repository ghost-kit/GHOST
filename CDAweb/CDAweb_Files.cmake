#Splitting out files list

if(NOT CDAWebTK_SOURCE_DIR)
    set(CDAWebTK_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR})
    INCLUDE_DIRECTORIES(${CMAKE_CURRENT_LIST_DIR})
endif()

# wrap QT
QT4_WRAP_CPP(
    MOC_BUILT_SOURCES
        ${CDAWebTK_SOURCE_DIR}/filterNetworkAccessModule.h
        ${CDAWebTK_SOURCE_DIR}/ScInfoPropWidget.h
        ${CDAWebTK_SOURCE_DIR}/status.h
        ${CDAWebTK_SOURCE_DIR}/filedownloader.h
        ${CDAWebTK_SOURCE_DIR}/vtkSpaceCraftInfo.h
        ${CDAWebTK_SOURCE_DIR}/pathSelector.h
        ${CDAWebTK_SOURCE_DIR}/newFileChooserWidget.h
    )

# Wrap UI
QT4_WRAP_UI(
    UI_BUILT_SOURCES
    ${CDAWebTK_SOURCE_DIR}/ScInfoPropWidget.ui
    ${CDAWebTK_SOURCE_DIR}/status.ui
    ${CDAWebTK_SOURCE_DIR}/pathSelector.ui
    )

# Property Widgets
set(outifaces0)
set(outsrcs0)
ADD_PARAVIEW_PROPERTY_WIDGET(outifaces0 outsrcs0
        TYPE "SpaceCraftInfoProperties"
        CLASS_NAME "ScInfoPropWidget")

set(outifaces1)
set(outsrcs1)
ADD_PARAVIEW_PROPERTY_WIDGET(outifaces1 outsrcs1
        TYPE "PathSelectionProperty"
        CLASS_NAME "pathSelector")

# Set up files for compilation
set(SM_XML
                ${CDAWebTK_SOURCE_DIR}/SpaceCraftInfoFilter.xml
                ${CDAWebTK_SOURCE_DIR}/SpaceCraftInfoSource.xml
    )

set(PROP_ifaces ${outifaces0} ${outifaces1})
set(PROP_src ${outsrcs0} ${outsrcs1})
set(QT_Auto_src ${MOC_BUILT_SOURCES} ${UI_BUILT_SOURCES})
set(PLUGIN_src
                ${CDAWebTK_SOURCE_DIR}/filterNetworkAccessModule.cxx
                ${CDAWebTK_SOURCE_DIR}/vtkSpaceCraftInfo.cxx
                ${CDAWebTK_SOURCE_DIR}/ScInfoPropWidget.cxx
                ${CDAWebTK_SOURCE_DIR}/status.cxx
                ${CDAWebTK_SOURCE_DIR}/filedownloader.cxx
                ${CDAWebTK_SOURCE_DIR}/timefithandler.cxx
                ${CDAWebTK_SOURCE_DIR}/BadDataHandler.cxx
                ${CDAWebTK_SOURCE_DIR}/cdfDataReader.cxx
                ${CDAWebTK_SOURCE_DIR}/omitBDHandler.cxx
                ${CDAWebTK_SOURCE_DIR}/pathSelector.cxx
                ${CDAWebTK_SOURCE_DIR}/newFileChooserWidget.cxx
    )

set(PLUGIN_SM_src
                    ${CDAWebTK_SOURCE_DIR}/vtkSpaceCraftInfoFilter.cxx
                    ${CDAWebTK_SOURCE_DIR}/vtkSpaceCraftInfoSource.cxx
    )
