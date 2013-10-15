PROJECT("CDAWebTK")
cmake_minimum_required(VERSION 2.8.9 FATAL_ERROR)

if(MACD108)
   set(ParaView_DIR /Volumes/Data/Development/ParaView/local/build)
   set(INSTALL_DIR /Volumes/Data/Development/GHOST-Kit/GHOST-build/install)
   set(PV_SuperBuild_DIR /Volumes/Data/Development/ParaView/superbuild)
   set(CMAKE_OSX_DEPLOYMENT_TARGET "10.6")
   set(CMAKE_OSX_SYSROOT "/Applications/Xcode-4.3.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.6.sdk")
endif()


set(CMAKE_LIBRARY_PATH  ${INSTALL_DIR}/lib
                        ${PV_SuperBuild_DIR}/install/lib
                        ${CMAKE_LIBRARY_PATH}
    )


link_directories(BEFORE ${INSTALL_DIR}/lib
                        ${PV_SuperBuild_DIR}/install/lib
    )


include_directories(BEFORE ${INSTALL_DIR}/include
                           ${PV_SuperBuild_DIR}/install/include
    )


#Configure needed librarires
set(LIBS ${INSTALL_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}cdf${CMAKE_STATIC_LIBRARY_SUFFIX}
         ${INSTALL_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}CDFreader${CMAKE_STATIC_LIBRARY_SUFFIX}
         ${INSTALL_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}boost_date_time${CMAKE_STATIC_LIBRARY_SUFFIX}
         ${INSTALL_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}DateTime${CMAKE_STATIC_LIBRARY_SUFFIX}
    )

find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})

if(NOT ParaView_BINARY_DIR)
  find_package(ParaView REQUIRED)
  include(${PARAVIEW_USE_FILE})
endif()

#needed to build plugins
include(ParaViewPlugins)

#get file list
INCLUDE_DIRECTORIES(src)
INCLUDE_DIRECTORIES(BEFORE ${QT_QTNETWORK_INCLUDE_DIR})
INCLUDE_DIRECTORIES(BEFORE ${QT_QTXML_INCLUDE_DIR})

#get build files
include(./CDAweb_Files.cmake)

#plugin (Filter for CDAweb)
ADD_PARAVIEW_PLUGIN(CDAWebTK "1.0.0.beta1"
        SERVER_MANAGER_XML ${CDAWebTK_SM_XML}
        GUI_INTERFACES ${CDAWebTK_PROP_IFACES}
        SERVER_MANAGER_SOURCES ${CDAWebTK_SM_SRC}
        SOURCES ${CDAWebTK_SRC}
                )

target_link_libraries(CDAWebTK ${LIBS})


