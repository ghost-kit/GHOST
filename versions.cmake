# This maintains the links for all sources used by this superbuild.
# Simply update this file to change the revision.
# One can use different revision on different platforms.
# e.g.
# if (UNIX)
#   ..
# else (APPLE)
#   ..
# endif()


add_revision(hdf4
    URL "http://www.hdfgroup.org/ftp/HDF/HDF_Current/src/hdf-4.2.9.tar.gz"
    URL_MD5 b49f1fd65e4857c6f6d7da151b96f32a)


add_revision(jpeg
        URL "http://www.hdfgroup.org/ftp/lib-external/jpeg/src/jpegsrc.v6b.tar.gz"
        URL_MD5 83992a9466af7536da30efe6b51d4064)

add_revision(cdf
    URL "http://cdaweb.gsfc.nasa.gov/pub/software/cdf/dist/cdf34_1/unix/cdf34_1-dist-all.tar.gz"
    URL_MD5 85918abf003b8fa5da42244e68fee8f7)

add_revision(ghost
    SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/ghost-kit)

add_revision(lfm
    SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/ghost-kit/lfm)

add_revision(io
    SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/dep-libs/io)

add_revision(DateTime
    SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/dep-libs/DateTime)

add_revision(cxform
    SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/dep-libs/cxform)

add_revision(CDFreader
    SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/dep-libs/CDFreader)

add_revision(zlib
  URL "http://www.paraview.org/files/dependencies/zlib-1.2.7.tar.gz"
  URL_MD5 60df6a37c56e7c1366cca812414f7b85)
# NOTE: if updating zlib version, fix patch in zlib.cmake


add_revision(png
  URL "http://paraview.org/files/dependencies/libpng-1.4.8.tar.gz"
  URL_MD5 49c6e05be5fa88ed815945d7ca7d4aa9)

add_revision(freetype
  URL "http://paraview.org/files/dependencies/freetype-2.4.8.tar.gz"
  URL_MD5 "5d82aaa9a4abc0ebbd592783208d9c76")

add_revision(szip
  URL "http://paraview.org/files/dependencies/szip-2.1.tar.gz"
  URL_MD5 902f831bcefb69c6b635374424acbead)

add_revision(hdf5
  URL "http://paraview.org/files/dependencies/hdf5-1.8.9.tar.gz"
  URL_MD5 d1266bb7416ef089400a15cc7c963218)

add_revision(silo
  URL "http://paraview.org/files/dependencies/silo-4.8-bsd.tar.gz"
  URL_MD5 d864e383f25b5b047b98aa2d5562d379)

add_revision(cgns
  URL "http://www.paraview.org/files/dependencies/cgnslib_3.1.3-4.tar.gz"
  URL_MD5 442bba32b576f3429cbd086af43fd4ae)

add_revision(ffmpeg
  URL "http://paraview.org/files/dependencies/ffmpeg-0.6.5.tar.gz"
  URL_MD5 451054dae3b3d33a86c2c48bd12d56e7)

add_revision(libxml2
  URL "http://paraview.org/files/dependencies/libxml2-2.7.8.tar.gz"
  URL_MD5 8127a65e8c3b08856093099b52599c86)

add_revision(fontconfig
  URL "http://paraview.org/files/dependencies/fontconfig-2.8.0.tar.gz"
  URL_MD5 77e15a92006ddc2adbb06f840d591c0e)

add_revision(qt
  URL "http://www.paraview.org/files/dependencies/qt-everywhere-opensource-src-4.8.2.tar.gz"
  URL_MD5 3c1146ddf56247e16782f96910a8423b)

if (WIN32 OR (CROSS_BUILD_STAGE STREQUAL "CROSS"))
  add_revision(python
    URL "http://www.paraview.org/files/dependencies/Python-2.7.3.tgz"
    URL_MD5 "2cf641732ac23b18d139be077bd906cd")
else()
  add_revision(python
    URL "http://paraview.org/files/dependencies/Python-2.7.2.tgz"
    URL_MD5 "0ddfe265f1b3d0a8c2459f5bf66894c7")
endif()

add_revision(numpy
  URL "http://paraview.org/files/dependencies/numpy-1.6.2.tar.gz"
  URL_MD5 95ed6c9dcc94af1fc1642ea2a33c1bba)

add_revision(matplotlib
  URL "http://paraview.org/files/dependencies/matplotlib-1.1.1_notests.tar.gz"
  URL_MD5 30ee59119599331bf1f3b6e838fee9a8)

add_revision(boost
  URL "http://www.paraview.org/files/dependencies/boost_1_50_0.tar.gz"
  URL_MD5 dbc07ab0254df3dda6300fd737b3f264)

add_revision(manta
  URL "http://paraview.org/files/dependencies/manta-r2439.tar.gz"
  URL_MD5 fbf4107fe2f6d7e8a5ae3dda71805bdc)

if (UNIX)
  add_revision(mpi
    URL "http://paraview.org/files/dependencies/mpich2-1.4.1p1.tar.gz"
    URL_MD5 b470666749bcb4a0449a072a18e2c204)
elseif (WIN32)
  add_revision(mpi
    URL "http://www.paraview.org/files/dependencies/openmpi-1.4.4.tar.gz"
    URL_MD5 7253c2a43445fbce2bf4f1dfbac113ad)
endif()

if (CROSS_BUILD_STAGE STREQUAL "CROSS")
  add_revision(mesa
    URL "http://www.paraview.org/files/dependencies/MesaLib-7.6.1.tar.gz"
    URL_MD5 e80fabad2e3eb7990adae773d6aeacba)
else()
  add_revision(mesa
    URL "http://paraview.org/files/dependencies/MesaLib-7.11.2.tar.gz"
    URL_MD5 b9e84efee3931c0acbccd1bb5a860554)
endif()

# We stick with 7.11.2 for Mesa version for now. Newer mesa doesn't seem to
# build correctly with certain older compilers (e.g. on neser).
add_revision(osmesa
    URL "http://paraview.org/files/dependencies/MesaLib-7.11.2.tar.gz"
    URL_MD5 b9e84efee3931c0acbccd1bb5a860554)

if (TRUST_SVN_CERTIFICATES_AUTOMATICALLY)
  add_revision(diy
     SVN_REPOSITORY https://svn.mcs.anl.gov/repos/diy/trunk
     SVN_REVISION -r178
     SVN_TRUST_CERT 1)
else()
  add_revision(diy
     SVN_REPOSITORY https://svn.mcs.anl.gov/repos/diy/trunk
     SVN_REVISION -r178)
endif()

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
  #set (ParaView_URL_MD5 "6a65c4b03bd82393197f1311e6d9c750" CACHE
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

add_revision(qhull
    GIT_REPOSITORY git://github.com/gzagaris/gxzagas-qhull.git
    GIT_TAG master)

add_revision(genericio
    GIT_REPOSITORY git://kwsource.kitwarein.com/genericio/genericio.git
    GIT_TAG master)

#------------------------------------------------------------------------------
# Optional Plugins. Doesn't affect ParaView binaries at all even if missing
# or disabled.
#------------------------------------------------------------------------------

add_revision(cosmotools
    GIT_REPOSITORY git://public.kitware.com/cosmotools.git
    GIT_TAG master)

add_revision(acusolve
  GIT_REPOSITORY git://kwsource.kitwarein.com/paraview/acusolvereaderplugin.git
  GIT_TAG master)

add_revision(vistrails
  GIT_REPOSITORY git://kwsource.kitwarein.com/paraview/vistrails.git
  GIT_TAG master)

#add_revision(mili_plugin
# URL ${CMAKE_CURRENT_SOURCE_DIR}/Externals/mili)

add_revision(portfwd
  URL "http://www.paraview.org/files/dependencies/portfwd-0.29.tar.gz"
  URL_MD5 93161c91e12b0d67ca52dc13708a2f2f)

add_revision(lapack
  URL "http://paraview.org/files/dependencies/lapack-3.4.2.tgz"
  URL_MD5 61bf1a8a4469d4bdb7604f5897179478)
