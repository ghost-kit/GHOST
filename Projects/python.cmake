if (APPLE)
  message(FATAL_ERROR "ABORT")
endif()

set(libtype "--enable-shared")
if (CROSS_BUILD_STAGE STREQUAL "TOOLS")
  set(libtype "--enable-static --disable-shared")
endif()

add_external_project_or_use_system(python
  DEPENDS zlib png
  CONFIGURE_COMMAND <SOURCE_DIR>/configure
	    	    CXX=${CMAKE_CXX_COMPILER}
		    --prefix=<INSTALL_DIR>
                    --enable-unicode
                    --enable-shared
		    #${libtype}
  )
set (pv_python_executable "${install_location}/bin/python" CACHE INTERNAL "" FORCE)
