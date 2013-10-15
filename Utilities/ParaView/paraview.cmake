set (extra_cmake_args)
if (manta_ENABLED)
  list (APPEND extra_cmake_args
    -DMANTA_BUILD:PATH=${SuperBuild_BINARY_DIR}/manta/src/manta-build)
endif()
if(PV_NIGHTLY_SUFFIX)
  list (APPEND extra_cmake_args
    -DPV_NIGHTLY_SUFFIX:STRING=${PV_NIGHTLY_SUFFIX})
endif()

if (APPLE)
  # We are having issues building mpi4py with Python 2.6 on Mac OSX. Hence,
  # disable it for now.
  list (APPEND extra_cmake_args
        -DPARAVIEW_USE_SYSTEM_MPI4PY:BOOL=ON)
endif()

set (PARAVIEW_INSTALL_DEVELOPMENT_FILES FALSE)
if (paraviewsdk_ENABLED)
  set (PARAVIEW_INSTALL_DEVELOPMENT_FILES TRUE)
endif()

add_external_project(paraview
  DEPENDS_OPTIONAL
    boost ffmpeg hdf5 libxml3 manta matplotlib mpi numpy png python qt visitbridge zlib silo cgns
    mesa osmesa nektarreader
    
  DEPENDS
  	boost hdf5 mpi python qt

  CMAKE_ARGS
    -DBUILD_SHARED_LIBS:BOOL=ON
    -DBUILD_TESTING:BOOL=OFF
    -DPARAVIEW_BUILD_PLUGIN_CoProcessingScriptGenerator:BOOL=ON
    -DPARAVIEW_BUILD_QT_GUI:BOOL=ON
    -DPARAVIEW_ENABLE_PYTHON:BOOL=ON
    -DPARAVIEW_USE_MPI:BOOL=ON
    -DVTK_USE_SYSTEM_HDF5:BOOL=ON
    -DPARAVIEW_INSTALL_DEVELOPMENT_FILES:BOOL=${PARAVIEW_INSTALL_DEVELOPMENT_FILES}

    # Web documentation
    -DPARAVIEW_BUILD_WEB_DOCUMENTATION:BOOL=${PARAVIEW_BUILD_WEB_DOCUMENTATION}

    # specify the apple app install prefix. No harm in specifying it for all
    # platforms.
    -DMACOSX_APP_INSTALL_PREFIX:PATH=<INSTALL_DIR>/Applications

  ${extra_cmake_args}
)
