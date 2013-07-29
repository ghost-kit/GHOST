cmake_minimum_required(VERSION 2.8)
PROJECT("DateTime")

if(USE_FPIC)
   set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -fPIC)
endif(USE_FPIC)


add_library(
            DateTime
            STATIC
            DateTime.C
            )

INCLUDE_DIRECTORIES(${DateTime_SOURCE_DIR})

find_package(CxxTest)
if(CXXTEST_FOUND)
  set(CXXTEST_USE_PYTHON TRUE)
  include_directories(${CXXTEST_INCLUDE_DIR})
  enable_testing()
  CXXTEST_ADD_TEST(testDateTime testErrorQueue.C "${DateTime_SOURCE_DIR}/tests/testDateTime.cxxTest")
  target_link_libraries(testDateTime DateTime )
else ()
  message ( STATUS "Could NOT find CxxTest - unit testing not available" )
endif ()
