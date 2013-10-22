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

execute_process(COMMAND ${CMAKE_COMMAND} -E remove_directory ${TMP_DIR}/${PROJECT_NAME})
execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${TMP_DIR}/${PROJECT_NAME})
execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${BINARY_DIR}/lib/${SHARED_LIBRARY_PREFIX}${TARGET_NAME}${SHARED_LIBRARY_SUFFIX} ${TMP_DIR}/${PROJECT_NAME})

message("DIR: ${PARAVIEW_BINARY_DIR}")

if (APPLE)
    message("FIXING UP THE PLUGIN")
    message("Command:  ${PV_SUPERBUILD_LIST_DIR}/Projects/apple/fixup_plugin.py")
    message("PV_BINARY_DIR: ${PARAVIEW_BINARY_DIR}")

  execute_process(
      COMMAND ${PV_SUPERBUILD_LIST_DIR}/Projects/apple/fixup_plugin.py
            # The directory containing the plugin dylibs or the plugin itself.
            ${TMP_DIR}/${PROJECT_NAME}/${SHARED_LIBRARY_PREFIX}${TARGET_NAME}${SHARED_LIBRARY_SUFFIX}
            # names to replace (in order)
            "${PARAVIEW_BINARY_DIR}/lib/=@executable_path/../Libraries/"
            "${PARAVIEW_BINARY_DIR}/lib/libavformat.dylib=@executable_path/../Libraries/libavformat.52.64.2.dylib"
            "${PARAVIEW_BINARY_DIR}/lib/libavcodec.dylib=@executable_path/../Libraries/libavcodec.52.72.2.dylib"
            "${PARAVIEW_BINARY_DIR}/lib/libavutil.dylib=@executable_path/../Libraries/libavutil.50.15.1.dylib"
            "${PARAVIEW_BINARY_DIR}/lib/libswscale.dylib=@executable_path/../Libraries/libswscale.0.11.0.dylib"
            "${PARAVIEW_BINARY_DIR}/lib/=@executable_path/../Libraries/"
            "${QT_LIBRARY_DIR}/Qt=@executable_path/../Frameworks/Qt"
            "${QT_LIBRARY_DIR}/=@executable_path/../Libraries/"
            "libhdf5.7.3.0.dylib=@executable_path/../Libraries/libhdf5.1.8.9.dylib"
            "libhdf5_hl.7.3.0.dylib=@executable_path/../Libraries/libhdf5.1.8.9.dylib"
            )
    message("FIXUP THEORETICALLY COMPLETE")
endif()

execute_process(
  COMMAND ${CMAKE_COMMAND} -E tar cvzf ${bundle_name} ${PROJECT_NAME}
  WORKING_DIRECTORY ${TMP_DIR})
