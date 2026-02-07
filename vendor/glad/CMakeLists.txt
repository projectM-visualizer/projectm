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

# build glad objects
add_library(glad_obj
        OBJECT
        ${SOURCES}
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

set_target_properties(glad PROPERTIES
        POSITION_INDEPENDENT_CODE ON
        )

# platform link dependencies
if (UNIX AND NOT APPLE)
    target_link_libraries(glad PUBLIC dl)
endif()
