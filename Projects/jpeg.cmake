

add_external_project(
    jpeg

    PATCH_COMMAND  ${CMAKE_COMMAND} -E make_directory <INSTALL_DIR>/bin &&
                   ${CMAKE_COMMAND} -E make_directory <INSTALL_DIR>/man/man1 &&
                   ${CMAKE_COMMAND} -E copy_if_different
                   ${CMAKE_CURRENT_LIST_DIR}/../depends/jpeg_patch/config.guess
                   <SOURCE_DIR>/config.guess &&
                   ${CMAKE_COMMAND} -E copy_if_different
                   ${CMAKE_CURRENT_LIST_DIR}/../depends/jpeg_patch/config.sub
                   <SOURCE_DIR>/config.sub

    CONFIGURE_COMMAND   <SOURCE_DIR>/configure
                                    --enable-static
                                    --enable-shared
                                    --prefix=<INSTALL_DIR>



)

