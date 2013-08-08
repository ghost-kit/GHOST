cmake_minimum_required(VERSION 2.8)
project (libjpeg)

if (DEFINED CMAKE_TOOLCHAIN_FILE)
	message("Include Cross Platform cmake: " ${CROSS_PLATFORM_CMAKE})
	get_filename_component(COMPILER_NAME ${CMAKE_C_COMPILER} NAME)
	message( "CMAKE_C_COMPILER: ${COMPILER_NAME}")
	string(REGEX REPLACE "(.*)-gcc" "\\1" target_platform "${COMPILER_NAME}")
	message( "target_platform: ${target_platform}")
	set(target_platform_preconfigure_options "CC=${target_platform}-gcc;AR=${target_platform}-ar;LD=${target_platform}-ld;RANLIB=${target_platform}-ranlib;NM=${target_platform}-nm;AS=${target_platform}-as;OBJDUMP=${target_platform}-objdump;OBJCOPY=${target_platform}-objcopy;STRIP=${target_platform}-strip;STRINGS=${target_platform}-strings;SIZE=${target_platform}-size")
endif(DEFINED CMAKE_TOOLCHAIN_FILE)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set(TOOL_LIBJPEG_SOURCE jpegsrc.v6b.tar.gz)
set(TOOL_LIBJPEG_DIR jpeg-6b/)
set(CONFIG_ADD_OPTIONS "--prefix;${CMAKE_INSTALL_PREFIX};--enable-static;--disable-shared")
	if(DEFINED CMAKE_TOOLCHAIN_FILE)
		add_custom_command(
			OUTPUT BUILD_LIBJPEG
			COMMAND wget -qO- http://downloads.sourceforge.net/project/libjpeg/libjpeg/6b/${TOOL_LIBJPEG_SOURCE} | tar -zxvf - && cd ${CMAKE_BINARY_DIR}/${TOOL_LIBJPEG_DIR} && cp ${CMAKE_CURRENT_SOURCE_DIR}/libtool-2.4.2-config/* .  && ${target_platform_preconfigure_options} ./configure --host=${target_platform} ${CONFIG_ADD_OPTIONS} && make -j2 && mkdir -p ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} ${CMAKE_LIBRARY_OUTPUT_DIRECTORY} ${CMAKE_BINARY_DIR}/man/man1 ${CMAKE_BINARY_DIR}/include && make install #&& mkdir -p ${CMAKE_LIBRARY_OUTPUT_DIRECTORY} && cp ${CMAKE_BINARY_DIR}/${TOOL_LIBJPEG_DIR}/.libs/libjpeg.so ${CMAKE_BINARY_DIR}/${TOOL_LIBJPEG_DIR}/libjpeg* ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
			WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
			COMMENT "Build libjpeg"
		)
	else(DEFINED CMAKE_TOOLCHAIN_FILE)
		add_custom_command(
			OUTPUT BUILD_LIBJPEG
			COMMAND wget -qO- http://downloads.sourceforge.net/project/libjpeg/libjpeg/6b/${TOOL_LIBJPEG_SOURCE} | tar -zxvf - && cd ${CMAKE_BINARY_DIR}/${TOOL_LIBJPEG_DIR} && cp ${CMAKE_CURRENT_SOURCE_DIR}/libtool-2.4.2-config/* . && ./configure ${CONFIG_ADD_OPTIONS} && make -j2 && mkdir -p ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} ${CMAKE_LIBRARY_OUTPUT_DIRECTORY} ${CMAKE_BINARY_DIR}/man/man1 ${CMAKE_BINARY_DIR}/include && make install #&& mkdir -p ${CMAKE_LIBRARY_OUTPUT_DIRECTORY} && cp ${CMAKE_BINARY_DIR}/${TOOL_LIBJPEG_DIR}/.libs/libjpeg.so ${CMAKE_BINARY_DIR}/${TOOL_LIBJPEG_DIR}/libjpeg* ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
			WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
			COMMENT "Build libjpeg" 
		)
	endif(DEFINED CMAKE_TOOLCHAIN_FILE)
	add_custom_target(build_libjpeg ALL DEPENDS BUILD_LIBJPEG )
