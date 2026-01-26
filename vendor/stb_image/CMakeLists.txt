add_library(stb_image OBJECT
        ${CMAKE_CURRENT_SOURCE_DIR}/image_DXT.c
        ${CMAKE_CURRENT_SOURCE_DIR}/image_DXT.h
        ${CMAKE_CURRENT_SOURCE_DIR}/pkm_helper.h
        ${CMAKE_CURRENT_SOURCE_DIR}/pvr_helper.h
        ${CMAKE_CURRENT_SOURCE_DIR}/stb_image.h
        ${CMAKE_CURRENT_SOURCE_DIR}/stb_image.c
        ${CMAKE_CURRENT_SOURCE_DIR}/stb_image_write.h
        ${CMAKE_CURRENT_SOURCE_DIR}/stbi_DDS.h
        ${CMAKE_CURRENT_SOURCE_DIR}/stbi_DDS_c.h
        ${CMAKE_CURRENT_SOURCE_DIR}/stbi_ext.h
        ${CMAKE_CURRENT_SOURCE_DIR}/stbi_ext_c.h
        ${CMAKE_CURRENT_SOURCE_DIR}/stbi_pkm.h
        ${CMAKE_CURRENT_SOURCE_DIR}/stbi_pkm_c.h
        ${CMAKE_CURRENT_SOURCE_DIR}/stbi_pvr.h
        ${CMAKE_CURRENT_SOURCE_DIR}/stbi_pvr_c.h
        ${CMAKE_CURRENT_SOURCE_DIR}/stbi_qoi.h
        ${CMAKE_CURRENT_SOURCE_DIR}/stbi_qoi_c.h
        ${CMAKE_CURRENT_SOURCE_DIR}/stbi_qoi_write.h
        ${CMAKE_CURRENT_SOURCE_DIR}/wfETC.c
        ${CMAKE_CURRENT_SOURCE_DIR}/wfETC.h
        )

target_include_directories(stb_image
        PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}"
        )

set_target_properties(stb_image PROPERTIES
        FOLDER vendor/stb_image
        )
