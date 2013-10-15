if (APPLE)
  message(STATUS "Using system python. Pick correct python based on your deployment target")
add_external_project_or_use_system(python)
set(USE_SYSTEM_python TRUE CACHE BOOL "" FORCE)
# FIXME: We automate the picking of the correct python version.

else()

set(libtype "--enable-shared")
if (CROSS_BUILD_STAGE STREQUAL "TOOLS")
  set(libtype "--enable-static --disable-shared")
endif()

add_external_project_or_use_system(python
  DEPENDS zlib png
  CONFIGURE_COMMAND <SOURCE_DIR>/configure
                    --prefix=<INSTALL_DIR>
                    --enable-unicode
                    ${libtype}
  )
set (pv_python_executable "${install_location}/bin/python" CACHE INTERNAL "" FORCE)
endif()