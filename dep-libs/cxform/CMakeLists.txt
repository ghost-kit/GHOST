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

# Test Configuration
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${PROJECT_SOURCE_DIR}/../../Utilities/cxxtest)
find_package(CxxTest)
if(CXXTEST_FOUND)
    message(STATUS "cxxtest Found!")

    include_directories(BEFORE ${CMAKE_CURRENT_BINARY_DIR}/install/include)

    set(CXXTEST_USE_PYTHON TRUE)
    include_directories(${CXXTEST_INCLUDE_DIR})
    enable_testing()

    CXXTEST_ADD_TEST(testCppXform TestCppXform.C "${CMAKE_CURRENT_SOURCE_DIR}/test/TestCppXform.cxxTest")
    target_link_libraries(testCppXform cppForm  ${cxLIBS})

    #move test file
    set(DataFile "${CMAKE_CURRENT_SOURCE_DIR}/../../data-files/cdf_data/STA_L1_MAGB_RTN-20100101T000000Z-20100102T000000Z.cdf")
    add_custom_command(TARGET testCDFreader PRE_BUILD
                       COMMAND ${CMAKE_COMMAND} -E
                       copy ${DataFile} $<TARGET_FILE_DIR:testCDFreader>)


else()
    message ( STATUS "Could NOT find CxxTest - unit testing not available" )
endif()





INSTALL(FILES
        ${PROJECT_SOURCE_DIR}/cxform.h
	${PROJECT_SOURCE_DIR}/cppxform.h
        DESTINATION include/)

INSTALL(FILES
            ${PROJECT_BINARY_DIR}/${CMAKE_STATIC_LIBRARY_PREFIX}cxform${CMAKE_STATIC_LIBRARY_SUFFIX}
            ${PROJECT_BINARY_DIR}/${CMAKE_STATIC_LIBRARY_PREFIX}cppForm${CMAKE_STATIC_LIBRARY_SUFFIX}
        DESTINATION lib/)
