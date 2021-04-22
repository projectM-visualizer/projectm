# First try to use PKG_CONFIG to find libvisual.
find_package(PkgConfig QUIET)
if(PKG_CONFIG_FOUND)
    pkg_check_modules(LIBVISUAL libvisual-0.4 QUIET)

    if(LIBVISUAL_INCLUDEDIR)
        # Retrieve the plug-in install directory
        pkg_get_variable(LIBVISUAL_PLUGINSBASEDIR libvisual-0.4 pluginsbasedir)
    endif()
endif()

if(NOT LIBVISUAL_INCLUDEDIR OR NOT LIBVISUAL_LIBRARIES)
    find_path(LIBVISUAL_INCLUDEDIR
            libvisual/libvisual.h
            PATH_SUFFIXES libvisual-0.4
            )

    find_library(LIBVISUAL_LIBRARIES
            visual-0.4
            )

    # Use the default path.
    set(LIBVISUAL_PLUGINSBASEDIR "lib/libvisual-0.4")
endif()

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(libvisual
        REQUIRED_VARS LIBVISUAL_INCLUDEDIR LIBVISUAL_LIBRARIES
        VERSION_VAR LIBVISUAL_VERSION
        )

if(LIBVISUAL_FOUND AND NOT TARGET libvisual::libvisual)
    add_library(libvisual::libvisual INTERFACE IMPORTED)

    set_target_properties(libvisual::libvisual PROPERTIES
            INTERFACE_LINK_LIBRARIES "${LIBVISUAL_LIBRARIES}"
            INTERFACE_LINK_DIRECTORIES "${LIBVISUAL_LIBRARY_DIRS}"
            INTERFACE_INCLUDE_DIRECTORIES "${LIBVISUAL_INCLUDEDIR}"
            INTERFACE_COMPILE_OPTIONS "${LIBVISUAL_CFLAGS}"
            )
endif()
