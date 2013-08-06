cmake_minimum_required (VERSION 2.8)
PROJECT (ZLIB_EXAMPLES)

#-----------------------------------------------------------------------------
# Setup include Directories
#-----------------------------------------------------------------------------
INCLUDE_DIRECTORIES (${ZLIB_SOURCE_DIR})

#-----------------------------------------------------------------------------
# Add Tests
#-----------------------------------------------------------------------------

#-- Adding test for example
ADD_EXECUTABLE(example ../example.c)
TARGET_NAMING (example)
IF (WIN32)
  IF (MSVC)
    IF (NOT BUILD_SHARED_LIBS)
      SET_TARGET_PROPERTIES (example
          PROPERTIES
              LINK_FLAGS "/NODEFAULTLIB:MSVCRT"
      ) 
    ENDIF (NOT BUILD_SHARED_LIBS)
  ENDIF (MSVC)
ENDIF (WIN32)
TARGET_LINK_LIBRARIES(example ${ZLIB_LIB_TARGET})

ADD_TEST (NAME example COMMAND $<TARGET_FILE:example>)

#-- Adding test for minigzip
ADD_EXECUTABLE(minigzip ../minigzip.c)
TARGET_NAMING (minigzip)
IF (WIN32)
  IF (MSVC)
    IF (NOT BUILD_SHARED_LIBS)
      SET_TARGET_PROPERTIES (minigzip
          PROPERTIES
              LINK_FLAGS "/NODEFAULTLIB:MSVCRT"
      ) 
    ENDIF (NOT BUILD_SHARED_LIBS)
  ENDIF (MSVC)
ENDIF (WIN32)
TARGET_LINK_LIBRARIES(minigzip ${ZLIB_LIB_TARGET})

#ADD_TEST (NAME minigzip COMMAND $<TARGET_FILE:minigzip>)


IF (HAVE_OFF64_T)
  #-- Adding test for example64
  ADD_EXECUTABLE(example64 ../example.c)
  TARGET_NAMING (example64)
  IF (WIN32)
    IF (MSVC)
      IF (NOT BUILD_SHARED_LIBS)
        SET_TARGET_PROPERTIES (example64
            PROPERTIES
                LINK_FLAGS "/NODEFAULTLIB:MSVCRT"
        ) 
      ENDIF (NOT BUILD_SHARED_LIBS)
    ENDIF (MSVC)
  ENDIF (WIN32)
  SET_TARGET_PROPERTIES (example64 PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64")
  TARGET_LINK_LIBRARIES(example64 ${ZLIB_LIB_TARGET})

  ADD_TEST (NAME example64 COMMAND $<TARGET_FILE:example64>)

  #-- Adding test for minigzip64
  ADD_EXECUTABLE(minigzip64 ../minigzip.c)
  TARGET_NAMING (minigzip64)
  IF (WIN32)
    IF (MSVC)
      IF (NOT BUILD_SHARED_LIBS)
        SET_TARGET_PROPERTIES (minigzip64
            PROPERTIES
                LINK_FLAGS "/NODEFAULTLIB:MSVCRT"
        ) 
      ENDIF (NOT BUILD_SHARED_LIBS)
    ENDIF (MSVC)
  ENDIF (WIN32)
  SET_TARGET_PROPERTIES (minigzip64 PROPERTIES COMPILE_FLAGS "-D_FILE_OFFSET_BITS=64")
  TARGET_LINK_LIBRARIES(minigzip64 ${ZLIB_LIB_TARGET})

#  ADD_TEST (NAME minigzip64 COMMAND $<TARGET_FILE:minigzip64>)
ENDIF (HAVE_OFF64_T)
