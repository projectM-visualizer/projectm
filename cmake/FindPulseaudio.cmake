# First try to use PKG_CONFIG to find Pulseaudio.
find_package(PkgConfig QUIET)
if(PKG_CONFIG_FOUND)
    pkg_check_modules(PULSEAUDIO libpulse QUIET)
endif()

if(NOT PULSEAUDIO_INCLUDEDIR OR NOT PULSEAUDIO_LIBRARIES)
    find_path(PULSEAUDIO_INCLUDEDIR
            pulse/pulseaudio.h
            )

    find_library(PULSEAUDIO_LIBRARIES
            pulse
            )

    if(PULSEAUDIO_INCLUDEDIR AND EXISTS "${PULSEAUDIO_INCLUDEDIR}/pulse/version.h")
        file(STRINGS "${PULSEAUDIO_INCLUDEDIR}/pulse/version.h" pulseaudio_version_str
                REGEX "pa_get_headers_version\(\)"
                )
        if(pulseaudio_version_str AND "${pulseaudio_version_str}" MATCHES "\\(\"([0-9.]+)\"\\)")
            set(PULSEAUDIO_VERSION "${CMAKE_MATCH_1}")
        endif()
    endif()

    set(PULSEAUDIO_CFLAGS "-D_REENTRANT")
endif()

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(Pulseaudio
        REQUIRED_VARS PULSEAUDIO_LIBRARIES PULSEAUDIO_INCLUDEDIR
        VERSION_VAR PULSEAUDIO_VERSION
        )

if(Pulseaudio_FOUND AND NOT TARGET Pulseaudio::Pulseaudio)
    add_library(Pulseaudio::Pulseaudio INTERFACE IMPORTED)

    set_target_properties(Pulseaudio::Pulseaudio PROPERTIES
            INTERFACE_LINK_LIBRARIES "${PULSEAUDIO_LIBRARIES}"
            INTERFACE_LINK_DIRECTORIES "${PULSEAUDIO_LIBRARY_DIRS}"
            INTERFACE_INCLUDE_DIRECTORIES "${PULSEAUDIO_INCLUDEDIR}"
            INTERFACE_COMPILE_OPTIONS "${PULSEAUDIO_CFLAGS}"
            )
endif()
