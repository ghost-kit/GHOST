#Splitting out files list

if(NOT CDAWebTK_SOURCE_DIR)
    set(CDAWebTK_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR})
    INCLUDE_DIRECTORIES(${CMAKE_CURRENT_LIST_DIR}/src)
endif()

# wrap QT
QT4_WRAP_CPP(
    MOC_BUILT_SOURCES
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
    UI_BUILT_SOURCES
    ${CDAWebTK_SOURCE_DIR}/ui/ScInfoPropWidget.ui
    ${CDAWebTK_SOURCE_DIR}/ui/status.ui
    ${CDAWebTK_SOURCE_DIR}/ui/pathSelector.ui
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
                ${CDAWebTK_SOURCE_DIR}/xml/SpaceCraftInfoFilter.xml
                ${CDAWebTK_SOURCE_DIR}/xml/SpaceCraftInfoSource.xml
    )

set(PROP_ifaces ${outifaces0} ${outifaces1})
set(PROP_src ${outsrcs0} ${outsrcs1})
set(QT_Auto_src ${MOC_BUILT_SOURCES} ${UI_BUILT_SOURCES})
set(PLUGIN_src
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
    )

set(PLUGIN_SM_src
                    ${CDAWebTK_SOURCE_DIR}/src/vtkSpaceCraftInfoFilter.cxx
                    ${CDAWebTK_SOURCE_DIR}/src/vtkSpaceCraftInfoSource.cxx
    )
