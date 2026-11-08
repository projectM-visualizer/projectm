add_library(stb_image OBJECT
        ${CMAKE_CURRENT_SOURCE_DIR}/image_DXT.c
        ${CMAKE_CURRENT_SOURCE_DIR}/image_DXT.h
        ${CMAKE_CURRENT_SOURCE_DIR}/stb_image.h
        ${CMAKE_CURRENT_SOURCE_DIR}/stb_image.c
        ${CMAKE_CURRENT_SOURCE_DIR}/stbi_DDS.h
        ${CMAKE_CURRENT_SOURCE_DIR}/stbi_DDS_c.h
        ${CMAKE_CURRENT_SOURCE_DIR}/stbi_ext.h
        ${CMAKE_CURRENT_SOURCE_DIR}/stbi_ext_c.h
        )

target_include_directories(stb_image
        PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}"
        )

# projectM only loads textures with the extensions listed in TextureManager, so the PVR, PKM and QOI
# readers SOIL2 added to stb_image are never needed. Their sources were removed, see #1046.
target_compile_definitions(stb_image
        PUBLIC
        STBI_NO_PVR
        STBI_NO_PKM
        STBI_NO_QOI
        )

set_target_properties(stb_image PROPERTIES
        FOLDER vendor/stb_image
        )
