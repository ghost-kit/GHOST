project(cxformFilter)
cmake_minimum_required(VERSION 2.8.9 FATAL_ERROR)


#Configure needed librarires
set(LIBS ${PREFIX}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}cxform${CMAKE_STATIC_LIBRARY_SUFFIX}
         ${PREFIX}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}boost_date_time${CMAKE_STATIC_LIBRARY_SUFFIX}
         ${PREFIX}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}ltrDateTime${CMAKE_STATIC_LIBRARY_SUFFIX}
    )
