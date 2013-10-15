cmake_minimum_required(VERSION 2.8.9)
project(GHOST)

###DEVELOPMENT SWITCHES####
if(JOSH_MURPHY)
    if(APPLE)
        if(NOT AT_HOME)
            set(CMAKE_OSX_DEPLOYMENT_TARGET "10.6")
            set(CMAKE_OSX_SYSROOT "/Applications/Xcode-4.3.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.6.sdk")
            set(ParaView_DIR /Volumes/Data/Development/ParaView/superbuild/paraview/src/paraview-build)

        else() #At Home Settings
            set(CMAKE_OSX_DEPLOYMENT_TARGET "10.7")
            set(CMAKE_OSX_SYSROOT "/Applications/Xcode46.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.7.sdk")
            set(ParaView_DIR /Users/developer/Developer/ParaView/build401)
            #set(ParaView_DIR /Users/jjm390/Developer/ParaView/superbuild/paraview/src/paraview-build)
        endif()

     elseif(UNIX)
        set(ParaView_DIR /home/jjm390/Developer/ParaView/build401/paraview/src/paraview-build)
    endif()

    file(GLOB_RECURSE AllFiles *)
    file(GLOB_RECURSE cdfReader shared/CDFreader/* )
#    message(STATUS "${AllFiles}")
    set(QTCREATOR true)

endif()

###END DEVELOPMENT SWITCHES###

###########################################################################################################
## This file is the master build file for the GHOST ParaView plugin.  It is partially derived from the   ##
## ParaViewSuperbuild CMakeLists.txt file distributed in the ParaViewSuperbuild package.                 ##
## See licence file in licence subdirectory								 ##
###########################################################################################################


if(APPLE)
    if(NOT CMAKE_OSX_DEPLOYMENT_TARGET)
        message(WARNING "Ensure that CMAKE_OSX_SYSROOT, CMAKE_OSX_DEPLOYMENT_TARGET are set correctly")
        set(CMAKE_OSX_ARCHITECTURES "x86_64" CACHE STRING "By default, build for 64-bit Leopard")
    endif()
endif()

set(GHOST_CMAKE_DIR "${CMAKE_CURRENT_LIST_DIR}/CMake")
set(GHOST_DEPEND_DIR "${CMAKE_CURRENT_LIST_DIR}/depends")
set(GHOST_PROJECTS_DIR "${CMAKE_CURRENT_LIST_DIR}/projects")
set(GHOST_PV_CMAKE_DIR "${CMAKE_CURRENT_LIST_DIR}/Utilities/ParaView")
set(GHOST_BINARY_DIR "${GHOST_BINARY_DIR}")

set(platform)
if(APPLE)
    set(platform "apple")
elseif(UNIX)
    set(platform "unix")
elseif(WIN32)
    set(platform "win32")
endif()

#-----------------------------------------------------------------------------
# Cross compilation
set (CROSS_BUILD_STAGE "HOST" CACHE STRING
  "Cross compilation stage: either HOST(ie no crosscompile)|TOOLS|CROSS")
set_property(CACHE CROSS_BUILD_STAGE PROPERTY STRINGS
   "HOST" "TOOLS" "CROSS")


###------------------------------------------------------------------------------------------------------
# Setup CMAKE_MODULE_PATH
string(TOLOWER "${CROSS_BUILD_STAGE}" lstage)
set(CMAKE_MODULE_PATH
  "${GHOST_PROJECTS_DIR}/${platform}/${lstage}"
  "${GHOST_PROJECTS_DIR}/${platform}"
  "${GHOST_PROJECTS_DIR}/common"
  "${GHOST_PROJECTS_DIR}"
  "${GHOST_CMAKE_DIR}"
  "${GHOST_PV_CMAKE_DIR}"
  "${CMAKE_CURRENT_SOURCE_DIR}"
  ${CMAKE_MODULE_PATH})

#-----------------------------------------------------------------------------
if(NOT CROSS_BUILD_STAGE STREQUAL "HOST")
  include(CrossCompilationMacros)

  #ask user to say what machine they are compiling onto
  #so we can get the right environment settings
  query_target_machine()

  if(CROSS_BUILD_STAGE STREQUAL "CROSS")
    #configure the platform dependent settings
    #64bit_build, static_only, mpi search path
    do_cross_platform_settings()

    #find location of hosttools or host build to get compilation tools from
    find_hosttools()
  endif()
endif()

include(CMakeDependentOption)

#-----------------------------------------------------------------------------
# Collect information about the build platform.
include(CheckTypeSize)

if(NOT CROSS_BUILD_STAGE STREQUAL "CROSS")
  check_type_size(void* VOID_PTR_SIZE BUILTIN_TYPES_ONLY)
  if(VOID_PTR_SIZE EQUAL 8)
    set(64bit_build TRUE)
  else()
    set(64bit_build FALSE)
  endif()
endif()


# Set suffix to be used for generating archives. This ensures that the package
# files have decent names that we can directly upload to the website.
set(package_suffix "${CMAKE_SYSTEM_NAME}")
if(64bit_build)
  set(package_suffix "${package_suffix}-64bit")
else()
  set(package_suffix "${package_suffix}-32bit")
endif()

include(paraview_version)

option(TRUST_SVN_CERTIFICATES_AUTOMATICALLY
  "Trust SVN certificates automatically" ON)
mark_as_advanced(TRUST_SVN_CERTIFICATES_AUTOMATICALLY)

###------------------------------------------------------------------------------------------------------
# setup some standard variables
set(install_location "${CMAKE_CURRENT_BINARY_DIR}/install")
set(download_location "${CMAKE_CURRENT_BINARY_DIR}/downloads"
     CACHE PATH "Location where source tar-balls are (to be) downloaded.")
mark_as_advanced(download_location)

###------------------------------------------------------------------------------------------------------

if(WIN32)
    set(ldflags)
    set(cppflags)
    set(cxxflags)
    set(ld_library_path)
else()
    set(ldflags "$ENV{LDFLAGS} -L${install_location}/lib")
    set(cppflags "$ENV{CPPFLAGS} -fPIC")
    set(cxxflags "$ENV{CXXFLAGS} -fPIC")
    set(cflags "$ENV{CFLAGS} -fPIC")
    set(ld_library_path "${install_location}/lib:$ENV{LD_LIBRARY_PATH}")
endif()

if(UNIX AND NOT APPLE)
    option(PASS_LD_LIBRARY_PATH_FOR_BUILDS
         "Pass LD_LIBRARY_PATH to build scripts."
         ON)
    mark_as_advanced(PASS_LD_LIBRARY_PATH_FOR_BUILDS)
endif()

if(APPLE)
  set(cflags "${cflags} -arch x86_64 -mmacosx-version-min=${CMAKE_OSX_DEPLOYMENT_TARGET} -sysroot=${CMAKE_OSX_SYSROOT} ")
  set(cppflags "${cppflags} -arch x86_64 -mmacosx-version-min=${CMAKE_OSX_DEPLOYMENT_TARGET} -sysroot=${CMAKE_OSX_SYSROOT} ")
  set(cxxflags "${cxxflags} -arch x86_64 -mmacosx-version-min=${CMAKE_OSX_DEPLOYMENT_TARGET} -sysroot=${CMAKE_OSX_SYSROOT} ")
endif()
set(prefix_path "${install_location}")

#merge in default/user-specified CMAKE flags
set(ldflags "${CMAKE_SHARED_LINKER_FLAGS} ${ldflags}")
set(cppflags "${CMAKE_CXX_FLAGS} ${cppflags}")
set(cxxflags "${CMAKE_CXX_FLAGS} ${cxxflags}")
set(cflags "${CMAKE_C_FLAGS} ${cflags}")

string(STRIP "${ldflags}" ldflags)
string(STRIP "${cppflags}" cppflags)
string(STRIP "${cxxflags}" cxxflags)
string(STRIP "${cflags}"  cflags)

#-----------------------------------------------------------------------------
## Top-level options. These must be limited to a commonly used subset with
## default values being used for building ParaView binaries.
#option(PV_COMMAND_LINE_TOOLS_ONLY
#  "Turn this ON to build command lines tools alone." OFF)

if (WIN32)
  if ( (NOT "${CMAKE_GENERATOR}" MATCHES "^NMake.*$") OR
       (NOT "${CMAKE_GENERATOR}" MATCHES "^Visual Studio.*$"))
  # not use VS environment. We need to be pointed to nmake and devenv paths
  # since they are needed when building tools (qt, python, etc.)
    find_program(DEVENV_PATH devenv)
    find_program(NMAKE_PATH nmake)
    mark_as_advanced(DEVENV_PATH NMAKE_PATH)
  endif()

  # Freetype uses its own build system on win32...
  find_program(FTJAM_EXECUTABLE jam)
  mark_as_advanced(FTJAM_EXECUTABLE)
  if (freetype_ENABLED)
    if (NOT FTJAM_EXECUTABLE)
      message(FATAL_ERROR "FreeType's 'jam' build tool not found! Download it from http://sourceforge.net/projects/freetype/files/ftjam/2.5.2/ and set FTJAM_EXECUTABLE appropriately.")
    endif()
  endif()
endif()

###-------------------------------------------------------------------------------------------------------
include(ParaViewModules)
include(dep_versions)

###-------------------------------------------------------------------------------------------------------
# Projects to be built
set (CM_PROJECTS_ALL)

if(NOT WIN32)
  # Check if there's a Fortran compiler. If there is, CMAKE_Fortran_COMPILER
  # will be set. Note that as of CMake 2.8.11 that CMAKE_Fortran_COMPILER
  # isn't passed to the external projects so setting it to something else
  # will give unexpected results. This is CMake Mantis issue 14,231.
    include(CheckFortran)
endif()

include(mpi)
include(qt)
include(boost)
include(jpeg)
include(zlib)
include(szip)
include(hdf4)
include(hdf5)
include(cdf)
include(shared)
include(ghost)
include(python)
include(paraview)

if(NOT APPLE AND NOT UNIX)
    include(freetype)
    include(fontconfig)
    include(png)
endif()

#turn on ghost
set(ENABLE_ghost true)
if(BUILD_PARAVIEW)
    set(ENABLE_paraview true)
endif()

process_dependencies()


##############################
### This is a development hack for making QT Creator show the files I need. Ignore if you are not me.
#if(QTCREATOR)
#    INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR}/install/include)
#    INCLUDE_DIRECTORIES(${CMAKE_CURRENT_LIST_DIR}/shared/CDFreader/src)
#    find_package(Qt4 REQUIRED)
#    include(${QT_USE_FILE})
#    add_library(CDFreader STATIC ${QT_LIBRARIES} ${cdfReader})


#set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${PROJECT_SOURCE_DIR}/Utilities/cxxtest)
#find_package(CxxTest)
#if(CXXTEST_FOUND)
#    find_package(Qt4 REQUIRED)
#    include(${QT_USE_FILE})
#    message(STATUS "cxxtest Found!")

#    set(CDF_LIBRARIES ${CMAKE_CURRENT_BINARY_DIR}/install/lib/${CMAKE_STATIC_LIBRARY_PREFIX}cdf${CMAKE_STATIC_LIBRARY_SUFFIX})
#    include_directories(BEFORE ${CMAKE_CURRENT_BINARY_DIR}/install/include)

#    set(CXXTEST_USE_PYTHON TRUE)
#    include_directories(${CXXTEST_INCLUDE_DIR})
#    enable_testing()

#    CXXTEST_ADD_TEST(testCDFreader TestCDFreader.C "${CMAKE_CURRENT_SOURCE_DIR}/shared/CDFreader/test/TestCDFreader.cxxTest")
#    target_link_libraries(testCDFreader CDFreader ${QT_LIBRARIES} ${CDF_LIBRARIES})

#    #move test file
#    set(DataFile "${CMAKE_CURRENT_SOURCE_DIR}/test/data/STA_L1_MAGB_RTN-20100101T000000Z-20100102T000000Z.cdf")
#    add_custom_command(TARGET testCDFreader PRE_BUILD
#                       COMMAND ${CMAKE_COMMAND} -E
#                       copy ${DataFile} $<TARGET_FILE_DIR:testCDFreader>)


#    add_executable(generateStatusText ${CMAKE_CURRENT_SOURCE_DIR}/shared/CDFreader/test/generateStatusCodes.cxx)
#    target_link_libraries(generateStatusText CDFreader ${QT_LIBRARIES} ${CDF_LIBRARIES})

#else()
#    message ( STATUS "Could NOT find CxxTest - unit testing not available" )
#endif()

#endif()

#############################

