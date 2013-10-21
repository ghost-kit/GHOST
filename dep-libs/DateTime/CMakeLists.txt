cmake_minimum_required(VERSION 2.8)

PROJECT("DateTime")

if(USE_FPIC)
   set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -fPIC)
endif(USE_FPIC)

add_library(
            ltrDateTime
            STATIC
            DateTime.C
            )

if(BUILD_TEST)
    INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR})
    set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${PROJECT_SOURCE_DIR}/../../Utilities/cxxtest)
    find_package(CxxTest)
    if(CXXTEST_FOUND)
        message(STATUS "cxxtest Found!")
        set(CXXTEST_USE_PYTHON TRUE)
        include_directories(${CXXTEST_INCLUDE_DIR})
        enable_testing()
        CXXTEST_ADD_TEST(testDateTime testErrorQueue.C "${PROJECT_SOURCE_DIR}/tests/testDateTime.cxxTest")
        target_link_libraries(testDateTime ltrDateTime )
    else()
        message( STATUS "Could NOT find CxxTest - unit testing not available" )
    endif()
endif()


INSTALL(FILES
        ${PROJECT_BINARY_DIR}/${CMAKE_STATIC_LIBRARY_PREFIX}ltrDateTime${CMAKE_STATIC_LIBRARY_SUFFIX}
        DESTINATION lib/)

INSTALL(FILES
        ${PROJECT_SOURCE_DIR}/ltrDateTime.h
        ${PROJECT_SOURCE_DIR}/ltrDateTime_const.h
        DESTINATION include/)
