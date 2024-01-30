# Tests for various platform features and generates a header with the appropriate defines.
include(EnableCFlagsIfSupported)

add_compile_definitions(
        $<$<CONFIG:DEBUG>:DEBUG>
        )

if(NOT MSVC)
    enable_cflags_if_supported(
            -Wpedantic
            -Wextra
            -Wall
            -Wchar-subscripts
            -Wformat-security
            -Wpointer-arith
            -Wshadow
            -Wsign-compare
            -Wtype-limits
    )
else()
    enable_cflags_if_supported(
            /EHsc   # Exception handling support
            /GR     # RTTI, for dynamic_cast
            /W4
    )
endif()

# Adds fallback support to boost if std::filesystem is unavailable.
include(FilesystemSupport)

# Create global configuration header
file(MAKE_DIRECTORY "${PROJECTM_BINARY_DIR}/include")
configure_file(config.h.cmake.in "${PROJECTM_BINARY_DIR}/include/config.h")
include_directories("${PROJECTM_BINARY_DIR}/include")

# Force-include the file in all targets
if(MSVC)
    add_definitions(/FI"config.h")
else()
    # GCC or Clang
    add_definitions(-include config.h)
endif()
