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
        )

target_include_directories(SOIL2
        PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
        )

target_link_libraries(SOIL2
        PUBLIC
        ${PROJECTM_OPENGL_LIBRARIES}
        )

if(USE_GLES)
    target_compile_definitions(SOIL2
            PRIVATE
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
else()
    target_link_libraries(SOIL2
            PUBLIC
            OpenGL::EGL
            )
endif()

set_target_properties(SOIL2 PROPERTIES
        FOLDER vendor/SOIL2
        )
