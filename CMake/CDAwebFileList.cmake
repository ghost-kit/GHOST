PROJECT(CDAWEB)
#This file tracks all of the files needed for compiling CDAweb
#This file is used so that the monolithic plugin can find the correct files
#as well as the individual build.

QT_WRAP_CPP(
    CDAWEB_MOC_BUILT_SOURCES
    ${CDAWEB_SOURCE_DIR}/include/filterNetworkAccessModule.h
    ${CDAWEB_SOURCE_DIR}/include/ScInfoPropWidget.h
    ${CDAWEB_SOURCE_DIR}/include/status.h
    ${CDAWEB_SOURCE_DIR}/include/filedownloader.h
    ${CDAWEB_SOURCE_DIR}/include/vtkSpaceCraftInfo.h
    ${CDAWEB_SOURCE_DIR}/include/pathSelector.h
    ${CDAWEB_SOURCE_DIR}/include/newFileChooserWidget.h
    )


QT4_WRAP_UI(
    CDAWEB_UI_BUILT_SOURCES
    ${CDAWEB_SOURCE_DIR}/ui/ScInfoPropWidget.ui
    ${CDAWEB_SOURCE_DIR}/ui/status.ui
    ${CDAWEB_SOURCE_DIR}/ui/pathSelector.ui
    )

set(CDAWEB_outifaces0)
set(CDAWEB_outsrcs0)
ADD_PARAVIEW_PROPERTY_WIDGET(CDAWEB_outifaces0 CDAWEB_outsrcs0
        TYPE "SpaceCraftInfoProperties"
        CLASS_NAME "ScInfoPropWidget")


set(CDAWEB_outifaces1)
set(CDAWEB_outsrcs1)
ADD_PARAVIEW_PROPERTY_WIDGET(CDAWEB_outifaces1 CDAWEB_outsrcs1
        TYPE "PathSelectionProperty"
        CLASS_NAME "pathSelector")

# Set up files for compilation
set(CDAWEB_SM_XML
                    ${CDAWEB_SOURCE_DIR}/xml/SpaceCraftInfoFilter.xml
                    ${CDAWEB_SOURCE_DIR}/xml/SpaceCraftInfoSource.xml
   )

set(CDAWEB_PROP_ifaces
                    ${CDAWEB_outifaces0}
                    ${CDAWEB_outifaces1}
   )

set(CDAWEB_PROP_src
                    ${CDAWEB_outsrcs0}
                    ${CDAWEB_outsrcs1}
   )

set(CDAWEB_QT_Auto_src
                    ${CDAWEB_MOC_BUILT_SOURCES}
                    ${CDAWEB_UI_BUILT_SOURCES}
   )

set(CDAWEB_SRC_LIST
                    ${CDAWEB_SOURCE_DIR}/src/filterNetworkAccessModule.cpp
                    ${CDAWEB_SOURCE_DIR}/src/vtkSpaceCraftInfo.cxx
                    ${CDAWEB_SOURCE_DIR}/src/ScInfoPropWidget.cpp
                    ${CDAWEB_SOURCE_DIR}/src/status.cpp
                    ${CDAWEB_SOURCE_DIR}/src/filedownloader.cpp
                    ${CDAWEB_SOURCE_DIR}/src/timefithandler.cpp
                    ${CDAWEB_SOURCE_DIR}/src/BadDataHandler.cpp
                    ${CDAWEB_SOURCE_DIR}/src/cdfDataReader.cpp
                    ${CDAWEB_SOURCE_DIR}/src/omitBDHandler.cpp
                    ${CDAWEB_SOURCE_DIR}/src/pathSelector.cxx
                    ${CDAWEB_SOURCE_DIR}/src/newFileChooserWidget.cxx
   )


set(CDAWEB_SM_SRC
                    ${CDAWEB_SOURCE_DIR}/src/vtkSpaceCraftInfoFilter.cpp
                    ${CDAWEB_SOURCE_DIR}/src/vtkSpaceCraftInfoSource.cpp
   )
