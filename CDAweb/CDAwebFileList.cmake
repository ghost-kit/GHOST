PROJECT(CDAWEB)
#This file tracks all of the files needed for compiling CDAweb
#This file is used so that the monolithic plugin can find the correct files
#as well as the individual build.


INCLUDE_DIRECTORIES(${CDAWEB_SOURCE_DIR}/src/headers)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

QT_WRAP_CPP(
    CDAWEB_MOC_BUILT_SOURCES
    ${CDAWEB_SOURCE_DIR}/src/header/filterNetworkAccessModule.h
    ${CDAWEB_SOURCE_DIR}/src/header/ScInfoPropWidget.h
    ${CDAWEB_SOURCE_DIR}/src/header/status.h
    ${CDAWEB_SOURCE_DIR}/src/header/filedownloader.h
    ${CDAWEB_SOURCE_DIR}/src/header/vtkSpaceCraftInfo.h
    ${CDAWEB_SOURCE_DIR}/src/header/pathSelector.h
    ${CDAWEB_SOURCE_DIR}/src/header/newFileChooserWidget.h
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

set(CDAWEB_GUI_INT
                    ${CDAWEB_outifaces0}
                    ${CDAWEB_outifaces1}
   )

set(CDAWEB_SRC_LIST
                    ${CDAWEB_SOURCE_DIR}/src/filterNetworkAccessModule.cxx
                    ${CDAWEB_SOURCE_DIR}/src/vtkSpaceCraftInfo.cxx
                    ${CDAWEB_SOURCE_DIR}/src/ScInfoPropWidget.cxx
                    ${CDAWEB_SOURCE_DIR}/src/status.cxx
                    ${CDAWEB_SOURCE_DIR}/src/filedownloader.cxx
                    ${CDAWEB_SOURCE_DIR}/src/timefithandler.cxx
                    ${CDAWEB_SOURCE_DIR}/src/BadDataHandler.cxx
                    ${CDAWEB_SOURCE_DIR}/src/cdfDataReader.cxx
                    ${CDAWEB_SOURCE_DIR}/src/omitBDHandler.cxx
                    ${CDAWEB_SOURCE_DIR}/src/pathSelector.cxx
                    ${CDAWEB_SOURCE_DIR}/src/newFileChooserWidget.cxx
                    ${CDAWEB_MOC_BUILT_SOURCES}
                    ${CDAWEB_UI_BUILT_SOURCES}
                    ${CDAWEB_outsrcs0}
                    ${CDAWEB_outsrcs1}


   )


set(CDAWEB_SM_SRC
                    ${CDAWEB_SOURCE_DIR}/src/vtkSpaceCraftInfoFilter.cxx
                    ${CDAWEB_SOURCE_DIR}/src/vtkSpaceCraftInfoSource.cxx
   )
