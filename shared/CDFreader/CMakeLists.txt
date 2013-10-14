PROJECT(CDFreader)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8.9 FATAL_ERROR)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_LIST_DIR}/cmake )

INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR}/src)

set(cdfDataReaderFiles
      ${CMAKE_CURRENT_SOURCE_DIR}/src/CDFreader.cxx
      ${CMAKE_CURRENT_SOURCE_DIR}/src/CDFvariable.cxx
      ${CMAKE_CURRENT_SOURCE_DIR}/src/CDFattribute.cxx
)

    add_library(cdfReader STATIC ${cdfDataReaderFiles})

set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${PROJECT_SOURCE_DIR}/../../Utilities/cxxtest)
find_package(CxxTest)
if(CXXTEST_FOUND)
    find_package(Qt4 REQUIRED)
    include(${QT_USE_FILE})
    message(STATUS "cxxtest Found!")

    set(CDF_USE_STATIC_LIBS ON)
    find_package(CDF REQUIRED)

    set(CXXTEST_USE_PYTHON TRUE)
    include_directories(${CXXTEST_INCLUDE_DIR})
    enable_testing()

    CXXTEST_ADD_TEST(testCDFreader TestCDFreader.C "${CMAKE_CURRENT_SOURCE_DIR}/test/TestCDFreader.cxxTest")
    target_link_libraries(testCDFreader ${CMAKE_CURRENT_BINARY_DIR}/libcdfReader.a ${QT_LIBRARIES} ${CDF_LIBRARIES})

    #move test file
    set(DataFile "${CMAKE_CURRENT_SOURCE_DIR}/../../test/data/STA_L1_MAGB_RTN-20100101T000000Z-20100102T000000Z.cdf")
    add_custom_command(TARGET testCDFreader PRE_BUILD
                       COMMAND ${CMAKE_COMMAND} -E
                       copy ${DataFile} $<TARGET_FILE_DIR:testCDFreader>)


else()
    message ( STATUS "Could NOT find CxxTest - unit testing not available" )
endif()
