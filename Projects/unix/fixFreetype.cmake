add_external_project(
    fixFreetype
    DEPENDS freetype

   CONFIGURE_COMMAND 
   echo "No Configure Command"	 

   BUILD_COMMAND
   echo "No Build"

   INSTALL_COMMAND
   ln -s <INSTALL_DIR>/include/freetype2/freetype <INSTALL_DIR>/include/freetype

      )
