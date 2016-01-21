add_external_project(
  freetype
  DEPENDS zlib
 
  CMAKE_ARGS
	-DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
	-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
	-DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
	-DBUILD_SHARED_LIBS:BOOL=true


#  CONFIGURE_COMMAND <SOURCE_DIR>/configure
#                    --prefix=<INSTALL_DIR>
#                    --enable-static=no
#                     --with-sysroot=<INSTALL_DIR>
) 
