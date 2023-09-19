# Tests for various platform features and generates a header with the appropriate defines,
# similar to the one created by autotools.

include(CheckSymbolExists)
include(CheckCXXSymbolExists)
include(CheckFunctionExists)
include(CheckIncludeFileCXX)
include(CheckEnumValueExists)
include(CheckIncludeFiles)
include(EnableCFlagsIfSupported)

add_compile_definitions(
        $<$<CONFIG:DEBUG>:DEBUG>
        )

if(NOT MSVC)
    enable_cflags_if_supported(
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

check_function_exists(aligned_alloc HAVE_ALIGNED_ALLOC)
check_include_file_cxx("dlfcn.h" HAVE_DLFCN_H)
check_include_file_cxx("fts.h" HAVE_FTS_H)
check_include_file_cxx("GL/gl.h" HAVE_GL_GL_H)
check_include_file_cxx("inttypes.h" HAVE_INTTYPES_H)
check_include_file_cxx("memory.h" HAVE_MEMORY_H)
check_function_exists(posix_memalign HAVE_POSIX_MEMALIGN)
check_enum_value_exists("PTHREAD_PRIO_INHERIT" "pthread.h" HAVE_PTHREAD_PRIO_INHERIT)
check_include_file_cxx("stdint.h" HAVE_STDINT_H)
check_include_file_cxx("stdlib.h" HAVE_STDLIB_H)
check_include_file_cxx("strings.h" HAVE_STRINGS_H)
check_include_file_cxx("string.h" HAVE_STRING_H)
check_include_file_cxx("sys/stat.h" HAVE_SYS_STAT_H)
check_include_file_cxx("sys/types.h" HAVE_SYS_TYPES_H)
check_include_file_cxx("unistd.h" HAVE_UNISTD_H)
check_include_file_cxx("windows.h" HAVE_WINDOWS_H)

set(_std_c_headers
        # C89/C90
        assert.h
        ctype.h
        errno.h
        float.h
        limits.h
        locale.h
        math.h
        setjmp.h
        signal.h
        stdarg.h
        stddef.h
        stdio.h
        stdlib.h
        string.h
        time.h
        # C95/NA1
        iso646.h
        wctype.h
        # C99
        complex.h
        fenv.h
        inttypes.h
        stdbool.h
        stdint.h
        tgmath.h
        )
check_include_files("${_std_c_headers}" STDC_HEADERS LANGUAGE C)
unset(_std_c_headers)

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
