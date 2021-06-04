# First try to use PKG_CONFIG to find JACK.
find_package(PkgConfig QUIET)
if(PKG_CONFIG_FOUND)
    pkg_check_modules(JACK jack QUIET)
endif()

if(NOT JACK_INCLUDEDIR OR NOT JACK_LIBRARIES)
    find_path(JACK_INCLUDEDIR
            jack/jack.h
            )

    find_library(JACK_LIBRARIES
            jack
            )
endif()

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(JACK
        REQUIRED_VARS JACK_LIBRARIES JACK_INCLUDEDIR
        VERSION_VAR JACK_VERSION
        )

if(JACK_FOUND AND NOT TARGET JACK::JACK)
    add_library(JACK::JACK INTERFACE IMPORTED)

    set_target_properties(JACK::JACK PROPERTIES
            INTERFACE_LINK_LIBRARIES "${JACK_LIBRARIES}"
            INTERFACE_LINK_DIRECTORIES "${JACK_LIBRARY_DIRS}"
            INTERFACE_INCLUDE_DIRECTORIES "${JACK_INCLUDEDIR}"
            INTERFACE_COMPILE_OPTIONS "${JACK_CFLAGS}"
            )
endif()
