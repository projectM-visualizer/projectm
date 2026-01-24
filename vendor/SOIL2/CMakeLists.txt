set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_library(SOIL2 OBJECT
        ${CMAKE_CURRENT_SOURCE_DIR}/src/common/common.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/common/common.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/SOIL2/image_DXT.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/SOIL2/image_DXT.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/SOIL2/image_helper.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/SOIL2/image_helper.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/SOIL2/pkm_helper.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/SOIL2/pvr_helper.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/SOIL2/SOIL2.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/SOIL2/SOIL2.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/SOIL2/stb_image.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/SOIL2/stb_image_write.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/SOIL2/stbi_DDS.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/SOIL2/stbi_DDS_c.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/SOIL2/stbi_ext.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/SOIL2/stbi_ext_c.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/SOIL2/stbi_pkm.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/SOIL2/stbi_pkm_c.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/SOIL2/stbi_pvr.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/SOIL2/stbi_pvr_c.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/SOIL2/stbi_qoi.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/SOIL2/stbi_qoi_c.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/SOIL2/stbi_qoi_write.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/SOIL2/wfETC.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/SOIL2/wfETC.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/SOIL2/SOIL2_gl_bridge.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/SOIL2/SOIL2_gl_bridge.h
        )

target_include_directories(SOIL2
        PUBLIC
        "${PROJECT_SOURCE_DIR}/vendor/glad/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
        )

if (ANDROID)
    target_compile_definitions(SOIL2
            PRIVATE
            SOIL_PLATFORM_ANDROID=1
            )
elseif (APPLE)
    target_compile_definitions(SOIL2
            PRIVATE
            SOIL_PLATFORM_OSX=1
            )
elseif (WIN32)
    target_compile_definitions(SOIL2
            PRIVATE
            SOIL_PLATFORM_WIN32=1
            )
else()
    target_compile_definitions(SOIL2
            PRIVATE
            SOIL_X11_PLATFORM=1
            )
endif()

if (USE_GLES)
    target_compile_definitions(SOIL2
            PUBLIC
            SOIL_GLES2
            )
    target_link_libraries(SOIL2
            PUBLIC
            ${CMAKE_DL_LIBS}
            )
endif()

if(NOT TARGET OpenGL::EGL)
    target_compile_definitions(SOIL2
            PRIVATE
            SOIL_NO_EGL
            )
endif()

set_target_properties(SOIL2 PROPERTIES
        FOLDER vendor/SOIL2
        )

target_link_libraries(SOIL2
        PRIVATE
        glad
        )
