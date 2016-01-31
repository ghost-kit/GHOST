    if(WIN32)

        message(FATAL_ERROR "Windows Not yet supported... comming soon!")


    elseif(APPLE)


        set(CDF_OS macosx)
        set(CDF_ENV x86_64)

         set(UCOPTIONS "\"-fPIC\"")

    elseif(UNIX)
        #for now, we are going to assume this is LINUX, not other flavors
        set(CDF_OS linux)

        if(64bit_build)
            set(CDF_ENV gnu64)
        else()
            set(CDF_ENV gnu32)
        endif()

    set(UCOPTIONS "\"${cflags}\"")

    endif()



    set(BUILD_COMMAND "${CMAKE_MAKE_PROGRAM} CC=${CMAKE_C_COMPILER} CXX=${CMAKE_CXX_COMPILER} OS=${CDF_OS} ENV=${CDF_ENV} SHARED=no CURSES=no FORTRAN=no UCOPTIONS=${UCOPTIONS} all")
    message("BC: ${BUILD_COMMAND}")


add_external_project(
    cdf

    CONFIGURE_COMMAND echo "Configuring CDF"


    BUILD_COMMAND ${BUILD_COMMAND}

    INSTALL_COMMAND make INSTALLDIR=${install_location} install

    BUILD_IN_SOURCE 1
)

