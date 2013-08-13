if (APPLE)
  set (SHARED_LIBRARY_PREFIX "lib")
  set (SHARED_LIBRARY_SUFFIX ".dylib")
  set (PLUGIN_DIR "lib")
elseif (UNIX)
  set (SHARED_LIBRARY_PREFIX "lib")
  set (SHARED_LIBRARY_SUFFIX ".so")
  set (PLUGIN_DIR "lib")
elseif (WIN32)
  set (SHARED_LIBRARY_PREFIX "")
  set (SHARED_LIBRARY_SUFFIX ".dll")
  set (PLUGIN_DIR "bin")
endif()

if (APPLE)
  execute_process(
      COMMAND ${CMAKE_CURRENT_LIST_DIR}/Utilities/ParaView/fixup_plugin.py
            # The directory containing the plugin dylibs or the plugin itself.
            ${LIB_DIR}/${SHARED_LIBRARY_PREFIX}GhostKit${SHARED_LIBRARY_SUFFIX}
            # names to replace (in order)
            "${PARAVIEW_BINARY_DIR}/lib/=@executable_path/../Libraries/"
            "${QT_LIBRARY_DIR}/Qt=@executable_path/../Frameworks/Qt"
            "${QT_LIBRARY_DIR}/=@executable_path/../Libraries/"
            "libhdf5.7.3.0.dylib=@executable_path/../Libraries/libhdf5.1.8.9.dylib"
            "libhdf5_hl.7.3.0.dylib=@executable_path/../Libraries/libhdf5.1.8.9.dylib"
            )
endif()
