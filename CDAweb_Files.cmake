#Splitting out files list

# wrap QT
QT4_WRAP_CPP(
    MOC_BUILT_SOURCES
        filterNetworkAccessModule.h
        ScInfoPropWidget.h
        status.h
        filedownloader.h
        vtkSpaceCraftInfo.h
        pathSelector.h
        newFileChooserWidget.h
    )

# Wrap UI
QT4_WRAP_UI(
    UI_BUILT_SOURCES
    ScInfoPropWidget.ui
    status.ui
    pathSelector.ui
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
set(SM_XML SpaceCraftInfoFilter.xml SpaceCraftInfoSource.xml)
set(PROP_ifaces ${outifaces0} ${outifaces1})
set(PROP_src ${outsrcs0} ${outsrcs1})
set(QT_Auto_src ${MOC_BUILT_SOURCES} ${UI_BUILT_SOURCES})
set(PLUGIN_src filterNetworkAccessModule.cpp
                vtkSpaceCraftInfo.cxx
                ScInfoPropWidget.cpp
                status.cpp
                filedownloader.cpp
                timefithandler.cpp
                BadDataHandler.cpp
                cdfDataReader.cpp
                omitBDHandler.cpp
                pathSelector.cxx
                newFileChooserWidget.cxx)

set(PLUGIN_SM_src vtkSpaceCraftInfoFilter.cpp
                  vtkSpaceCraftInfoSource.cpp)
