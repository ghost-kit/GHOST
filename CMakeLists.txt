cmake_minimum_required(VERSION 2.8)
PROJECT("DateTime")

add_library(
            DateTime
            STATIC
            DateTime.C
            )

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

find_package(CxxTest)
if(CXXTEST_FOUND)
  set(CXXTEST_USE_PYTHON TRUE)
  include_directories(${CXXTEST_INCLUDE_DIR})
  enable_testing()
  CXXTEST_ADD_TEST(testDateTime testErrorQueue.C "${CMAKE_CURRENT_SOURCE_DIR}/tests/testDateTime.cxxTest")
  target_link_libraries(testDateTime DateTime )
else ()
  message ( STATUS "Could NOT find CxxTest - unit testing not available" )
endif ()
