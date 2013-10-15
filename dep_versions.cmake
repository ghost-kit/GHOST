# This maintains the links for all sources used by GHOST.
# Simply update this file to change the revision.
# One can use different revision on different platforms.
# e.g.
# if (UNIX)
#   ..
# else (APPLE)
#   ..
# endif()

add_revision(qt
  URL "http://www.paraview.org/files/dependencies/qt-everywhere-opensource-src-4.8.2.tar.gz"
  URL_MD5 3c1146ddf56247e16782f96910a8423b)

add_revision(boost
  URL "http://www.paraview.org/files/dependencies/boost_1_50_0.tar.gz"
  URL_MD5 dbc07ab0254df3dda6300fd737b3f264)

add_revision(zlib
  URL "http://www.paraview.org/files/dependencies/zlib-1.2.7.tar.gz"
  URL_MD5 60df6a37c56e7c1366cca812414f7b85)
# NOTE: if updating zlib version, fix patch in zlib.cmake

if (UNIX)
  add_revision(mpi
    URL "http://paraview.org/files/dependencies/mpich2-1.4.1p1.tar.gz"
    URL_MD5 b470666749bcb4a0449a072a18e2c204)
elseif (WIN32)
  add_revision(mpi
    URL "http://www.paraview.org/files/dependencies/openmpi-1.4.4.tar.gz"
    URL_MD5 7253c2a43445fbce2bf4f1dfbac113ad)
endif()

add_revision(szip
  URL "http://paraview.org/files/dependencies/szip-2.1.tar.gz"
  URL_MD5 902f831bcefb69c6b635374424acbead)

add_revision(hdf5
  URL "http://paraview.org/files/dependencies/hdf5-1.8.9.tar.gz"
  URL_MD5 d1266bb7416ef089400a15cc7c963218)

add_revision(hdf4
    URL "http://www.hdfgroup.org/ftp/HDF/HDF_Current/src/hdf-4.2.9.tar.gz"
    URL_MD5 b49f1fd65e4857c6f6d7da151b96f32a)


add_revision(jpeg
        URL "http://www.hdfgroup.org/ftp/lib-external/jpeg/src/jpegsrc.v6b.tar.gz"
        URL_MD5 83992a9466af7536da30efe6b51d4064)

add_revision(cdf
    URL "http://cdaweb.gsfc.nasa.gov/pub/software/cdf/dist/cdf34_1/unix/cdf34_1-dist-all.tar.gz"
    URL_MD5 85918abf003b8fa5da42244e68fee8f7)

add_revision(shared
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/shared)

add_revision(ghost
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/ghost-kit)
