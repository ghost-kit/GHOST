add_external_project(
    hdf4
    DEPENDS jpeg zlib

#     CMAKE_ARGS
#         -DMACOSX_VERSION_MIN=${CMAKE_OSX_DEPLOYMENT_TARGET}
#         -DPREFIX=<INSTALL_DIR>
#         -DHDF4_ENABLE_JPEG_LIB_SUPPORT=ON
#         -DHDF4_ENABLE_LARGE_FILE=ON
#         -DJPEG_DIR=<INSTALL_DIR>/lib
#         -DJPEG_INCLUDE_DIR=<INSTALL_DIR>/include
#         -DHDF4_ENABLE_EXTERNAL_SUPPORT=ON
#     	-DHDF4_USE_FORTRAN=OFF

   CONFIGURE_COMMAND #(From HDF4 4.2.9)

       <SOURCE_DIR>/configure
       CXX=${CMAKE_CXX_COMPILER}
       CC=${CMAKE_C_COMPILER}
       CFLAGS=-I${install_location}/include
       CXXFLAGS=-I${install_location}/include
       LDFLAGS=-L${install_location}/lib
       --prefix=<INSTALL_DIR>

       --with-sysroot=${CMAKE_OSX_SYSROOT}
       --enable-static
       --enable-production
       --disable-fortran
       
      )
