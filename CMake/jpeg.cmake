add_external_project_or_use_system(
    jpeg

    PATCH_COMMAND
          ${CMAKE_COMMAND} -E make_directory "${install_location}/bin" &&
          ${CMAKE_COMMAND} -E make_directory "${install_location}/man/man1"

  CONFIGURE_COMMAND <SOURCE_DIR>/configure
                    --prefix=<INSTALL_DIR>
                    --disable-shared
                    --mandir=<INSTALL_DIR>/man
                    --bindir=<INSTALL_DIR/bin

  INSTALL_COMMAND make install
)
