add_external_project(
  fontconfig
  DEPENDS freetype fixFreetype libxml2
  BUILD_IN_SOURCE 1
  CONFIGURE_COMMAND <SOURCE_DIR>/configure
                    --prefix=<INSTALL_DIR>
                    --disable-docs
                    --enable-libxml2
                    --enable-static=no
                    
  PROCESS_ENVIRONMENT
                    LIBXML2_CFLAGS -I<INSTALL_DIR>/include/libxml2 
                    LIBXML2_LIBS -lxml2
		    CFLAGS -I<INSTALL_DIR>/include/freetype
		    LIBS -L<INSTALL_DIR>/lib/libfreetype.so
)
