
add_external_project_or_use_system(
    jpeg

    PATCH_COMMAND
          ${CMAKE_COMMAND} -E make_directory "${install_location}/bin" &&
          ${CMAKE_COMMAND} -E make_directory "${install_location}/man/man1"


    INSTALL_COMMAND echo "Installed"

)
