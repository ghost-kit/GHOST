# This maintains the links for all sources used by GHOST.
# Simply update this file to change the revision.
# One can use different revision on different platforms.
# e.g.
# if (UNIX)
#   ..
# else (APPLE)
#   ..
# endif()


if (WIN32 OR (CROSS_BUILD_STAGE STREQUAL "CROSS"))
  add_revision(python
    URL "http://www.paraview.org/files/dependencies/Python-2.7.3.tgz"
    URL_MD5 "2cf641732ac23b18d139be077bd906cd")
else()
  add_revision(python
    URL "http://paraview.org/files/dependencies/Python-2.7.2.tgz"
    URL_MD5 "0ddfe265f1b3d0a8c2459f5bf66894c7")
endif()

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

# ----------------------------------------------------------------------------
# You choose to download ParaView source form GIT or other URL/FILE tarball
option(ParaView_FROM_GIT "If enabled then the repository is fetched from git" ON)

if (ParaView_FROM_GIT)
  # Download PV from GIT
  add_revision(paraview
    GIT_REPOSITORY git://paraview.org/ParaView.git
    GIT_TAG "v4.0.1")
else()
  # Variables to hold the URL and MD5 (optional)
  set (ParaView_URL "http://www.paraview.org/files/v4.0/ParaView-v4.0.1-source.tgz" CACHE
    STRING "Specify the url for ParaView tarball")
  set (ParaView_URL_MD5 "6a300744eaf32676a3a7e1b42eb642c7" CACHE
    STRING "MD5 of the ParaView tarball")

  # Get the length of the URL specified.
  if("${ParaView_URL}" STREQUAL "")
    # No URL specified raise error.
    message (FATAL_ERROR "ParaView_URL should have a valid URL or FilePath to a ParaView tarball")
  else()
    # Download PV from source specified in URL
    add_revision(paraview
      URL ${ParaView_URL}
      URL_MD5 ${ParaView_URL_MD5})
  endif()
endif()