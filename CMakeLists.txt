cmake_minimum_required(VERSION 2.8.9)
project(GHOST_Shared)

#Options for building
option(BUILD_IO "Build the IO package" OFF )
option(BUILD_DATETIME "Build the DateTime package" OFF )
option(BUILD_CXFORM "Builds the CXFORM package" OFF )
option(USE_FPIC "Use the -fPIC compiler flag to fix various issues" OFF )

if(BUILD_IO)
    option(IO_USE_APP "Use A++ and/or P++ for array storage" OFF)
    option(IO_USE_MPI "Build parallel I/O (Warning: leave this off, it's untested)" OFF)
endif(BUILD_IO)

message(STATUS "Building selected libraries")

if(USE_FPIC)
    set(USE_FPIC true CACHE INTERNAL "")
endif()

### IO LIBRARY ###
if(BUILD_IO)
    message(STATUS "BUILDING IO")
    add_subdirectory(${PROJECT_SOURCE_DIR}/io)
    INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/io)
    LINK_DIRECTORIES(${PROJECT_BINARY_DIR}/io)

    set(IO_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/io)
    set(IO_LIBRARY_DIR ${PROJECT_BINARY_DIR}/io)
    set(IO_LIBRARIES io)

    ##DEBUG##
    message(STATUS "IO include: ${IO_INCLUDE_DIR}")
    message(STATUS "IO libraries: ${IO_LIBRARY_DIR}")
endif()


### DATETIME LIBRARY ###
if(BUILD_DATETIME)
    message(STATUS "BUILDING DATETIME")
    add_subdirectory(${PROJECT_SOURCE_DIR}/DateTime)
    INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/DateTime)
    LINK_DIRECTORIES(${PROJECT_BINARY_DIR}/DateTime)

    set(DATETIME_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/DateTime)
    set(DATETIME_LIBRARY_DIR ${PROJECT_BINARY_DIR}/DateTime)
    set(DATETIME_LIBRARIES DateTime)

    ##DEBUG##
    message(STATUS "DateTime include: ${DATETIME_INCLUDE_DIR}")
    message(STATUS "DateTime include: ${DATETIME_LIBRARY_DIR}")
endif()

### CXFORM LIBRARY ###
if(BUILD_CXFORM)
    message(STATUS "BUILDING CXFORM")
    add_subdirectory(${PROJECT_SOURCE_DIR}/cxform)
    INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/cxform)
    LINK_DIRECTORIES(${PROJECT_BINARY_DIR}/cxform)

    set(CXFORM_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/cxform)
    set(CXFORM_LIBRARY_DIR ${PROJECT_BINARY_DIR}/cxform)
    set(CXFORM_LIBRARIES cxform)

    ##DEBUG##
    message(STATUS "cxform include: ${CXFORM_INCLUDE_DIR}")
    message(STATUS "cxform libraries: ${CXFORM_LIBRARY_DIR}")
endif()

set(SHARED_INCLUDE_DIR ${IO_INCLUDE_DIR} ${DATETIME_INCLUDE_DIR} ${CXFORM_INCLUDE_DIR} CACHE INTERNAL "")
set(SHARED_LIBRARY_DIR ${IO_LIBRARY_DIR} ${DATETIME_LIBRARY_DIR} ${CXFORM_LIBRARY_DIR} CACHE INTERNAL "")
set(SHARED_LIBRARIES ${IO_LIBRARIES} ${DATETIME_LIBRARIES} ${CXFORM_LIBRARIES} CACHE INTERNAL "")

