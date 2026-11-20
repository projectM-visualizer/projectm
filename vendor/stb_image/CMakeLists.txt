include(VendoredSymbols)

if(ENABLE_SYSTEM_STB_IMAGE)
    # Build the implementation from an external stb_image.h, e.g. from vcpkg's stb port. It doesn't
    # have the DDS, PVR, PKM and QOI readers of the vendored (SOIL2) copy.
    find_path(STB_IMAGE_INCLUDE_DIR stb_image.h PATH_SUFFIXES stb REQUIRED)
    message(STATUS "Using external stb_image.h from ${STB_IMAGE_INCLUDE_DIR}")

    add_library(stb_image OBJECT
            ${CMAKE_CURRENT_SOURCE_DIR}/stb_image_system.c
            )

    target_include_directories(stb_image
            PUBLIC
            "${STB_IMAGE_INCLUDE_DIR}"
            )

    # Prefix the public stb_image API, which is declared with STBIDEF.
    projectm_vendored_symbols_from_declarations(_stb_symbols "^[ 	]*STBIDEF[ 	]" "${STB_IMAGE_INCLUDE_DIR}/stb_image.h")
    projectm_write_vendored_symbol_header(_stb_force_include
            "${CMAKE_CURRENT_BINARY_DIR}/stb_image_symbol_prefix.h"
            stb_image
            ${_stb_symbols}
            )

    # PUBLIC, so the code calling stb_image (TextureManager) uses the same prefixed names.
    target_compile_options(stb_image
            PUBLIC
            ${_stb_force_include}
            )

    set_target_properties(stb_image PROPERTIES
            FOLDER vendor/stb_image
            )

    return()
endif()

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

# Give all global stb_image symbols a projectM-specific prefix, see cmake/VendoredSymbols.cmake: the public
# API declared with STBIDEF, plus the functions of the SOIL2 additions, which are declared without it.
projectm_vendored_symbols_from_declarations(_stb_symbols "^[ 	]*STBIDEF[ 	]" "${CMAKE_CURRENT_SOURCE_DIR}/stb_image.h")
list(APPEND _stb_symbols
        # image_DXT.c
        LSE_master_colors_max_min compress_DDS_alpha_block compress_DDS_color_block compute_color_line_STDEV
        convert_bit_range convert_image_to_DXT1 convert_image_to_DXT5 rgb_888_from_565 rgb_to_565 save_image_as_DDS
        # stbi_DDS_c.h
        stbi__dds_info_from_callbacks stbi__dds_info_from_file stbi__dds_info_from_memory stbi__dds_info_from_path
        stbi__dds_load_from_callbacks stbi__dds_load_from_file stbi__dds_load_from_memory stbi__dds_load_from_path
        stbi__dds_test_callbacks stbi__dds_test_file stbi__dds_test_filename stbi__dds_test_memory
        stbi_convert_bit_range stbi_rgb_888_from_565 stbi_decode_DXT1_block stbi_decode_DXT23_alpha_block
        stbi_decode_DXT45_alpha_block stbi_decode_DXT_color_block
        # stbi_pkm_c.h
        stbi__pkm_info_from_callbacks stbi__pkm_info_from_file stbi__pkm_info_from_memory stbi__pkm_info_from_path
        stbi__pkm_load_from_callbacks stbi__pkm_load_from_file stbi__pkm_load_from_memory stbi__pkm_load_from_path
        stbi__pkm_test_callbacks stbi__pkm_test_file stbi__pkm_test_filename stbi__pkm_test_memory
        # stbi_pvr_c.h
        stbi__pvr_info_from_callbacks stbi__pvr_info_from_file stbi__pvr_info_from_memory stbi__pvr_info_from_path
        stbi__pvr_load_from_callbacks stbi__pvr_load_from_file stbi__pvr_load_from_memory stbi__pvr_load_from_path
        stbi__pvr_test_callbacks stbi__pvr_test_file stbi__pvr_test_filename stbi__pvr_test_memory
        # stbi_qoi_c.h
        stbi__qoi_info_from_callbacks stbi__qoi_info_from_file stbi__qoi_info_from_memory stbi__qoi_info_from_path
        stbi__qoi_load_from_callbacks stbi__qoi_load_from_file stbi__qoi_load_from_memory stbi__qoi_load_from_path
        stbi__qoi_test_callbacks stbi__qoi_test_file stbi__qoi_test_filename stbi__qoi_test_memory
        # stbi_ext_c.h
        stbi_test stbi_test_from_callbacks stbi_test_from_file stbi_test_from_memory util_number_is_power_2
        # wfETC.c
        wfETC1_Color3IdxLUT wfETC1_DecodeBlock wfETC1_DecodeImage wfETC1_ReadColor4 wfETC1_ReadColor53
        wfETC1_ReadPixel wfETC_ClampColor wfETC_IntensityTables
        )
projectm_write_vendored_symbol_header(_stb_force_include
        "${CMAKE_CURRENT_BINARY_DIR}/stb_image_symbol_prefix.h"
        stb_image
        ${_stb_symbols}
        )

# PUBLIC, so the code calling stb_image (TextureManager) uses the same prefixed names.
target_compile_options(stb_image
        PUBLIC
        ${_stb_force_include}
        )

set_target_properties(stb_image PROPERTIES
        FOLDER vendor/stb_image
        )
