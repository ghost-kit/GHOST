CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT("cxform")

add_library(
            cxform
            STATIC
            cxform-auto.c
            cxform-manual.c
            )

add_library(
	    cppForm
	    STATIC
	    cppxform.cxx
	    cxform-auto.c
	    cxform-manual.c
	   )


if(NOT PREFIX)
    #This just sets the prefix for Joshua Murphy's work development machine.
    #set(PREFIX /Volumes/Data/Development/GHOST-Kit/build/install)
    set(PREFIX /Users/jjm390/Developer/GHOST/build/install)

endif()



include_directories(BEFORE ${PREFIX}/include)

set(cxLIBS ${PREFIX}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}ltrDateTime${CMAKE_STATIC_LIBRARY_SUFFIX})

add_executable(testCppForm cppformTest.cxx)
target_link_libraries(testCppForm ${cxLIBS} cppForm )


INSTALL(FILES
        ${PROJECT_SOURCE_DIR}/cxform.h
	${PROJECT_SOURCE_DIR}/cppxform.h
        DESTINATION include/)

INSTALL(FILES
            ${PROJECT_BINARY_DIR}/${CMAKE_STATIC_LIBRARY_PREFIX}cxform${CMAKE_STATIC_LIBRARY_SUFFIX}
            ${PROJECT_BINARY_DIR}/${CMAKE_STATIC_LIBRARY_PREFIX}cppForm${CMAKE_STATIC_LIBRARY_SUFFIX}
        DESTINATION lib/)
