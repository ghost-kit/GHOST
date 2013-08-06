# This maintains the links for all sources used by GHOST.
# Simply update this file to change the revision.
# One can use different revision on different platforms.
# e.g.
# if (UNIX)
#   ..
# else (APPLE)
#   ..
# endif()

add_revision(zlib
  URL "http://www.paraview.org/files/dependencies/zlib-1.2.7.tar.gz"
  URL_MD5 60df6a37c56e7c1366cca812414f7b85)
# NOTE: if updating zlib version, fix patch in zlib.cmake

add_revision(szip
  URL "http://paraview.org/files/dependencies/szip-2.1.tar.gz"
  URL_MD5 902f831bcefb69c6b635374424acbead)

add_revision(hdf5
  URL "http://paraview.org/files/dependencies/hdf5-1.8.9.tar.gz"
  URL_MD5 d1266bb7416ef089400a15cc7c963218)

add_revision(jpeg
    URL "http://www.hdfgroup.org/ftp/HDF/HDF_Current/src/hdf-4.2.9.tar.gz"
    URL_MD5 6af240405f2095e2718f303e7fba1d26)
