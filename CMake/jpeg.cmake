add_external_project_or_use_system(
    jpeg
  CONFIGURE_COMMAND <SOURCE_DIR>/configure
                    --prefix=<INSTALL_DIR>
                    --disable-shared
                    --enable-static

  BUILD_COMMAND ${CMAKE_MAKE_PROGRAM}
  BUILD_IN_SOURCE 1
)
