set(SOURCES "")

if (USE_GLES)
    # GLES2 / EGL
    list(APPEND SOURCES
            include/glad/gles2.h
            include/glad/egl.h
            src/gles2.c
            src/egl.c
    )
else()
    # Desktop OpenGL
    list(APPEND SOURCES
            include/glad/gl.h
            src/gl.c
    )

    if (WIN32)
        list(APPEND SOURCES
                include/glad/wgl.h
                src/wgl.c
        )
    elseif(APPLE)
        # macOS uses CGL. GLAD provides no separate loader file
    else()
        list(APPEND SOURCES
                include/glad/glx.h
                src/glx.c
        )
    endif()
endif()

# Give all global glad symbols a projectM-specific prefix, see cmake/VendoredSymbols.cmake. The names
# are read from the GLAD_API_CALL declarations of the generated headers.
include(VendoredSymbols)
file(GLOB _glad_headers "${CMAKE_CURRENT_SOURCE_DIR}/include/glad/*.h")
projectm_vendored_symbols_from_declarations(_glad_symbols "^GLAD_API_CALL " ${_glad_headers})
projectm_write_vendored_symbol_header(_glad_force_include
        "${CMAKE_CURRENT_BINARY_DIR}/glad_symbol_prefix.h"
        glad
        ${_glad_symbols}
        )

# The option force-including the header, for all targets using the glad headers.
set(PROJECTM_GLAD_FORCE_INCLUDE "${_glad_force_include}" CACHE INTERNAL "Compile option force-including the glad symbol prefix header")

# build glad objects
add_library(glad_obj
        OBJECT
        ${SOURCES}
        )

target_compile_options(glad_obj
        PRIVATE
        ${PROJECTM_GLAD_FORCE_INCLUDE}
        )

target_include_directories(glad_obj
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        )

set_target_properties(glad_obj PROPERTIES
        POSITION_INDEPENDENT_CODE ON
        )

# wrap the objects in a static library
add_library(glad STATIC $<TARGET_OBJECTS:glad_obj>)

# propagate include dirs via library target
target_include_directories(glad
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        )

# Targets linking glad need the same prefixed symbol names.
target_compile_options(glad
        PUBLIC
        ${PROJECTM_GLAD_FORCE_INCLUDE}
        )

target_compile_definitions(glad_obj
        PRIVATE
        GLAD_GLX_NO_X11
        )

set_target_properties(glad PROPERTIES
        POSITION_INDEPENDENT_CODE ON
        )

# platform link dependencies
if (UNIX AND NOT APPLE)
    target_link_libraries(glad PUBLIC dl)
endif()
