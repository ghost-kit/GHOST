PROJECT("CDAWebTK")
cmake_minimum_required(VERSION 2.8.9 FATAL_ERROR)

option(QTCREATOR "View all headers and xml files in QTCreator" OFF)

if(QTCREATOR)
#These command are used to reveal code in QTCreator that may not be directly linked.
#This passes those file names to the Files List in Make Plugin, but are not necessary
#to build the final product.
#It is not recomended to build the final binary with this setting on.

    set(ParaView_DIR /Users/jjm390/Developer/ParaView/build)
    set(INSTALL_DIR /Users/jjm390/Developer/install)
    include_directories(BEFORE ${INSTALL_DIR}/include)
    link_directories( ${INSTALL_DIR}/lib)
    set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} /Users/jjm390/Developer/install/lib)
    include_directories(BEFORE /Users/jjm390/Developer/install/include)

endif()

##########################################################################


##########################################################################

if( PREFIX )
  set( CMAKE_INCLUDE_PATH /usr/include ${PREFIX}/include ${CMAKE_INCLUDE_PATH} )
  set( CMAKE_LIBRARY_PATH /usr/lib ${PREFIX}/lib ${CMAKE_LIBRARY_PATH} )
endif( PREFIX )

#convienience funciton for ME. :)

set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake/" )


#needed for these plugins
# At present I am specifying ${LIBCDF_INCLUDE_DIRS} and ${LIBCDF_LIBRARIES} from the cmake command line
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREAD ON)
find_package(Boost COMPONENTS date_time REQUIRED)

##find cdf
if(NOT QTCREATOR)
    set(CDF_USE_STATIC_LIBS ON)
    find_package(CDF REQUIRED)

    include_directories(${CDF_INCLUDES} ${Boost_INCLUDE_DIRS} ${SHARED_INCLUDE_DIR})
    set(LIBS ${LIBS} ${CDF_LIBRARIES} ${Boost_LIBRARIES} ${SHARED_LIBRARIES})
else()

    set(LIBS ${LIBS} ${INSTALL_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}cdf${CMAKE_STATIC_LIBRARY_SUFFIX}
                     ${Boost_LIBRARIES}
                     DateTime
                     )

endif()


if(NOT ParaView_BINARY_DIR)
  find_package(ParaView REQUIRED)
  include(${PARAVIEW_USE_FILE})
endif()

#needed to build plugins
include(ParaViewPlugins)

#get file list
INCLUDE_DIRECTORIES(src)
include(./CDAweb_Files.cmake)

#plugin (Filter for CDAweb)
ADD_PARAVIEW_PLUGIN(CDAWebTK "1.0.0.beta1"
        SERVER_MANAGER_XML ${CDAWebTK_SM_XML}
        GUI_INTERFACES ${CDAWebTK_PROP_IFACES}
        SERVER_MANAGER_SOURCES ${CDAWebTK_SM_SRC}
        SOURCES ${CDAWebTK_SRC}

                )

target_link_libraries(CDAWebTK ${LIBS})



#for the mac distribution build
add_custom_target(dist
    COMMAND ${CMAKE_COMMAND}  -DBINARY_DIR:PATH=${PROJECT_BINARY_DIR}
                              -DPV_SUPERBUILD_LIST_DIR:PATH=${ParaView_SuperBuild_DIR}/Projects
                              -DPARAVIEW_BINARY_DIR:PATH=${ParaView_DIR}
                              -DQT_LIBRARY_DIR:PATH=${QT_LIBRARY_DIR}
                              -DTMP_DIR:PATH=${PROJECT_BINARY_DIR}
                              -Dbundle_name:STRING=${PROJECT_BINARY_DIR}/CDAwebTK.tgz
                              -P ${PROJECT_LIST_DIR}/install_CDAwebTK.cmake
)
