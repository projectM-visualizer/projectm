# Helper script to add the SDL2 CMake target and version variable as introduced in SDL 2.0.12.
# Also fixes a wrong include path provided by the SDL2 config script.


# Proper CMake target support was added in SDL 2.0.12, create one
# Need to search again to find the full path of libSDL2
if(NOT TARGET SDL2::SDL2)
    # Remove -lSDL2 as that is handled by CMake, note the space at the end so it does not replace e.g. -lSDL2main
    # This may require "libdir" being set (from above)
    string(REPLACE "-lSDL2 " "" SDL2_EXTRA_LINK_FLAGS " -lSDL2 ")
    string(STRIP "${SDL2_EXTRA_LINK_FLAGS}" SDL2_EXTRA_LINK_FLAGS)
    string(REPLACE "-lSDL2 " "" SDL2_EXTRA_LINK_FLAGS_STATIC " -Wl,--no-undefined -lm -ldl  -lasound -lm -ldl -lpthread -lpulse-simple -lpulse  -lX11 -lXext -lXcursor -lXinerama -lXi -lXrandr -lXss -lXxf86vm -lpthread -lrt ")
    string(STRIP "${SDL2_EXTRA_LINK_FLAGS_STATIC}" SDL2_EXTRA_LINK_FLAGS_STATIC)

    find_library(SDL2_LIBRARY SDL2)
    if(NOT SDL2_LIBRARY)
        message(FATAL_ERROR "Could not determine the location of the SDL2 library.")
    endif()

    add_library(SDL2::SDL2 SHARED IMPORTED)
    set_target_properties(SDL2::SDL2 PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${SDL2_INCLUDE_DIRS}"
            IMPORTED_LINK_INTERFACE_LANGUAGES "C"
            IMPORTED_LOCATION "${SDL2_LIBRARY}"
            INTERFACE_LINK_LIBRARIES "${SDL2_EXTRA_LINK_FLAGS}")

    find_library(SDL2MAIN_LIBRARY SDL2main)
    if(NOT SDL2MAIN_LIBRARY)
        message(FATAL_ERROR "Could not determine the location of the SDL2main library.")
    endif()

    add_library(SDL2::SDL2main STATIC IMPORTED)
    set_target_properties(SDL2::SDL2main PROPERTIES
            IMPORTED_LINK_INTERFACE_LANGUAGES "C"
            IMPORTED_LOCATION "${SDL2MAIN_LIBRARY}")

    # Retrieve the version from the SDL2_version.h header
    if(SDL2_INCLUDE_DIRS AND EXISTS "${SDL2_INCLUDE_DIRS}/SDL_version.h")
        file(STRINGS "${SDL2_INCLUDE_DIRS}/SDL_version.h" SDL_VERSION_MAJOR_LINE REGEX "^#define[ \t]+SDL_MAJOR_VERSION[ \t]+[0-9]+$")
        file(STRINGS "${SDL2_INCLUDE_DIRS}/SDL_version.h" SDL_VERSION_MINOR_LINE REGEX "^#define[ \t]+SDL_MINOR_VERSION[ \t]+[0-9]+$")
        file(STRINGS "${SDL2_INCLUDE_DIRS}/SDL_version.h" SDL_VERSION_PATCH_LINE REGEX "^#define[ \t]+SDL_PATCHLEVEL[ \t]+[0-9]+$")
        string(REGEX REPLACE "^#define[ \t]+SDL_MAJOR_VERSION[ \t]+([0-9]+)$" "\\1" SDL_VERSION_MAJOR "${SDL_VERSION_MAJOR_LINE}")
        string(REGEX REPLACE "^#define[ \t]+SDL_MINOR_VERSION[ \t]+([0-9]+)$" "\\1" SDL_VERSION_MINOR "${SDL_VERSION_MINOR_LINE}")
        string(REGEX REPLACE "^#define[ \t]+SDL_PATCHLEVEL[ \t]+([0-9]+)$" "\\1" SDL_VERSION_PATCH "${SDL_VERSION_PATCH_LINE}")
        set(SDL2_VERSION ${SDL_VERSION_MAJOR}.${SDL_VERSION_MINOR}.${SDL_VERSION_PATCH})
        unset(SDL_VERSION_MAJOR_LINE)
        unset(SDL_VERSION_MINOR_LINE)
        unset(SDL_VERSION_PATCH_LINE)
        unset(SDL_VERSION_MAJOR)
        unset(SDL_VERSION_MINOR)
        unset(SDL_VERSION_PATCH)
    endif()

endif()

# Temporary fix to deal with wrong include dir set by SDL2's CMake configuration.
get_target_property(_SDL2_INCLUDE_DIR SDL2::SDL2 INTERFACE_INCLUDE_DIRECTORIES)
if(_SDL2_INCLUDE_DIR MATCHES "(.+)/SDL2\$")
    # Check if SDL2::SDL2 is aliased to SDL2::SDL2-static (will be the case for static-only builds)
    get_target_property(_SDL2_ALIASED_TARGET SDL2::SDL2 ALIASED_TARGET)
    if(_SDL2_ALIASED_TARGET)
        set(_sdl2_target ${_SDL2_ALIASED_TARGET})
    else()
        set(_sdl2_target SDL2::SDL2)
    endif()

    message(STATUS "SDL2 include dir contains \"SDL2\" subdir (SDL bug #4004) - fixing to \"${CMAKE_MATCH_1}\".")
    set_target_properties(${_sdl2_target} PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_MATCH_1}"
            )
endif()

if(SDL2_VERSION AND SDL2_VERSION VERSION_LESS "2.0.5")
    message(FATAL_ERROR "SDL2 libraries were found, but have version ${SDL2_VERSION}. At least version 2.0.5 is required.")
endif()

