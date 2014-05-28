project(ghost-kit)
cmake_minimum_required(VERSION 2.8.9)

message("BUILDING GHOST!!! IF you got this far, you have won half the battle!")

if(DEP_INCLUDE_DIR)
    include_directories(BEFORE SYSTEM ${DEP_INCLUDE_DIR})
endif()


###OPTIONS##
##Turn on or off the components of the tool kit
option(GHOST_BUILD_LFM "Build the ParaView LFM Reader" ON)
option(GHOST_BUILD_ENLIL "Build the ParaView ENLIL Reader" ON)
option(GHOST_BUILD_CDAWEB "Build the ParaView CDAweb Tool Kit" ON)
option(GHOST_BUILD_CXFORM "Build the cxForm Tranform Filter" ON)
option(GHOST_BUILD_PLOTS "Build the Plots Source" ON)
option(GHOST_BUILD_SWFT "Build SwFT Panels" ON)

option(GHOST_MASTER_BUILD "Plugin Being being built as Master Build (DO NOT SET MANUALLY)" OFF)

#Set Output Direcotries
set(CMAKE_RUNTIME_OUTPUT_DIRECOTORY "${CMAKE_BINARY_DIR}/bin")

if(UNIX)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY  "${CMAKE_BINARY_DIR}/lib")
else()
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
endif()
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY  "${CMAKE_BINARY_DIR}/lib")

#Utilities for include
include(../Utilities/Git/Git.cmake)
include(../Utilities/ParaView/ParaViewDetermineVersion.cmake)
include(CMakeDependentOption)

    set(HDF4_LIBS
            ${CMAKE_INSTALL_PREFIX}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}df${CMAKE_STATIC_LIBRARY_SUFFIX}
            ${CMAKE_INSTALL_PREFIX}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}mfhdf${CMAKE_STATIC_LIBRARY_SUFFIX}
        )

    set(HDF_EXTRA_LIBS
        z
       sz
       ${CMAKE_INSTALL_PREFIX}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}jpeg${CMAKE_STATIC_LIBRARY_SUFFIX}
       m
        )

    set(HDF5_LIBS
       hdf5
       hdf5_hl
        )

    set(SHARED_LIBS
        ${CMAKE_INSTALL_PREFIX}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}io${CMAKE_STATIC_LIBRARY_SUFFIX}
        ${CMAKE_INSTALL_PREFIX}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}ltrDateTime${CMAKE_STATIC_LIBRARY_SUFFIX}
        ${CMAKE_INSTALL_PREFIX}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}cppForm${CMAKE_STATIC_LIBRARY_SUFFIX}
        )

    set(CDF_LIBS
        ${CMAKE_INSTALL_PREFIX}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}cdf${CMAKE_STATIC_LIBRARY_SUFFIX}
        )

    set(BOOST_LIBS
        ${CMAKE_INSTALL_PREFIX}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}boost_date_time${CMAKE_STATIC_LIBRARY_SUFFIX}
        )


    set(OTHER_REQ_LIBS
        vtkNetCDF_cxx
        )


#Get ParaView (so we can build our module!!)
if(NOT ParaView_BINARY_DIR)
    find_package(ParaView REQUIRED)
    include(${PARAVIEW_USE_FILE})
endif()

#Versioning of the plugin
set(ORGANIZATION "LASP & HAO")
set(GHOST_VERSION_MAJOR 1)
set(GHOST_VERSION_MINOR 0)
set(GHOST_VERSION_PATCH 0)
set(GHOST_VERSION_PATCH_EXTRA "beta 1")
set(GHOST_VERSION "1.0")
set(GHOST_VERSION_FULL "1.0.0 Beta 1")

determine_version(${CMAKE_SOURCE_DIR} ${GIT_EXECUTABLE} "GHOST")

message(STATUS "Using Git Executable: ${GIT_EXECUTABLE}")
message(STATUS "BUILDING VERSION ${GHOST_VERSION_FULL}")

message(STATUS "Building Selected Modules into GHOST.
If Problems are Encountered while building,
Try deleting your CMakeCache.txt file and rebuilding")

#Collect the information on the components
#----------------------------------------#
#LFM Reader
if(GHOST_BUILD_LFM)
    message(STATUS "Building LFM Reader Module")

    #Need LFM
    include(lfm/LFM_Files.cmake)


endif()

#ENLIL Reader
if(GHOST_BUILD_ENLIL)
    message(STATUS "Building ENLIL Reader Module")

    #Need Enlil
    include(enlil/Enlil_Files.cmake)

endif()

#CDAweb ToolKit
if(GHOST_BUILD_CDAWEB)
    message(STATUS "Building CDAweb ToolKit Module")

    #Need CDAweb
    include(CDAweb/CDAweb_Files.cmake)
endif()

#Transform Filter
if(GHOST_BUILD_CXFORM)
    message(STATUS "Building CXForm Transform Module")

    #Need CDAweb
    include(cxForm/GKcxFormFiles.cmake)
endif()

#Plots
if(GHOST_BUILD_PLOTS)
    message(STATUS "Building Plots Module")
    include(gkPlot/GKplotSourceFiles.cmake)
endif()

#SwFT Panels
if(GHOST_BUILD_SWFT)
    message(STATUS "Building SwFT Panels")
    include(SwFT/GKSwFTSourceFiles.cmake)
endif()

##gather all built files
set(SERVER_MAN_XML_LIST ${CDAWebTK_SM_XML}
                        ${ENLIL_SM_XML}
                        ${LFM_SM_XML}
                        ${GKcxForm_SM_XML}
                        ${GKplotSource_SM_XML}

    )

set(GUI_RF_XML_LIST     xml/ghost_gui.xml
                        ${ENLIL_GUI_XML}
                        ${LFM_GUI_XML}
    )

set(GUI_INTERFACE_LIST  ${CDAWebTK_PROP_IFACES}
                        ${GKcxForm_PROP_IFACES}
                        ${GKplotSource_PROP_IFACES}
                        ${GK_SWFT_INTERFACES}
    )

set(SERVER_MAN_SRC_LIST ${CDAWebTK_SM_SRC}
                        ${ENLIL_SM_SRC}
                        ${LFM_SM_SRC}
                        ${GKcxForm_SM_SRC}
                        ${GKplotSource_SM_SRC}
    )

set(ADD_SRC_LIST        ${CDAWebTK_SRC}
                        ${CDAWeb_PROP_SRC}
                        ${ENLIL_SRC}
                        ${GKcxForm_SRC}
                        ${GKplotSource_SRC}

    )

set(GUI_SOURCES_LIST  ${GK_SWFT_GUI_SOURCES}
    )

## Add the Plugin Definitions
ADD_PARAVIEW_PLUGIN(GhostKit                ${GHOST_VERSION_FULL}
                    SERVER_MANAGER_XML      ${SERVER_MAN_XML_LIST}
                    GUI_RESOURCE_FILES      ${GUI_RF_XML_LIST}
                    GUI_INTERFACES          ${GUI_INTERFACE_LIST}
                    GUI_SOURCES             ${GUI_SOURCES_LIST}
                    SERVER_MANAGER_SOURCES  ${SERVER_MAN_SRC_LIST}
                    SOURCES                 ${ADD_SRC_LIST}
                   )

#BECAUSE OF STRANGE LINKING REQUIREMENTS FOR STATIC LIBRARIES, SOME LIBRARIES NEED TO BE LISTED TWICE.
set(GHOST_LIBS ${HDF4_LIBS} ${HDF_EXTRA_LIBS} ${CDF_LIBS} ${BOOST_LIBS} ${SHARED_LIBS} ${CDF_LIBS} ${HDF4_LIBS} ${HDF5_LIBS} ${HDF_EXTRA_LIBS} ${OTHER_REQ_LIBS} ${BOOST_LIBS})

message("GHOST_LIBS: ${GHOST_LIBS}")

#AWEFUL HACK, but it works.
target_link_libraries(GhostKit ${GHOST_LIBS} ${GHOST_LIBS})

# Adding 'dist' Make target to deploy plugin that's compatible with
# KitWare release of ParaView for Mac OS-X.  Requires SuperBuild with
# GIT_TAG set accordingly in versions.cmake.
add_custom_target(dist
    COMMAND ${CMAKE_COMMAND}  -E make_directory ${CMAKE_INSTALL_PREFIX}/release &&
            ${CMAKE_COMMAND}  -DBINARY_DIR:PATH=${CMAKE_CURRENT_BINARY_DIR}
                              -DPV_SUPERBUILD_LIST_DIR:PATH=${PV_SUPERBUILD_LIST_DIR}
                              -DPARAVIEW_BINARY_DIR:PATH=${ParaView_DIR}
                              -DQT_LIBRARY_DIR:PATH=${QT_LIBRARY_DIR}
                              -DTMP_DIR:PATH=${CMAKE_CURRENT_BINARY_DIR}
                              -DPROJECT_NAME:STRING=${CMAKE_PROJECT_NAME}
                              -DTARGET_NAME:STRING="GhostKit"
                              -Dbundle_name:STRING=${CMAKE_INSTALL_PREFIX}/release/ghost_plugin.tgz
                              -P ${CMAKE_CURRENT_LIST_DIR}/install_ghost_mac.cmake &&
            ${CMAKE_COMMAND}  -E copy ${CMAKE_CURRENT_BINARY_DIR}/${TMP_DIR}/${PROJECT_NAME}/* ${CMAKE_INSTALL_PREFIX}/release/
)

if(UNIX AND (NOT APPLE))

    INSTALL(TARGETS GhostKit
            RUNTIME DESTINATION bin
            LIBRARY DESTINATION release
            ARCHIVE DESTINATION release
    )

endif()
