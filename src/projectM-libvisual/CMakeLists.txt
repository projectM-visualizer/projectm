if(NOT ENABLE_LIBVISUAL)
    return()
endif()


add_library(projectM_libvisual SHARED
        actor_projectM.cpp
        ConfigFile.cpp
        ConfigFile.h
        lvtoprojectM.h
        )

target_compile_definitions(projectM_libvisual
        PRIVATE
        PROJECTM_PREFIX="${CMAKE_INSTALL_PREFIX}"
        )

target_link_libraries(projectM_libvisual
        PUBLIC
        projectM_shared
        libvisual::libvisual
        ${CMAKE_DL_LIBS}
        )

install(TARGETS projectM_libvisual
        LIBRARY DESTINATION "${PROJECTM_LIBVISUAL_DIR}"
        RUNTIME DESTINATION "${PROJECTM_LIBVISUAL_DIR}"
        ARCHIVE DESTINATION "${PROJECTM_LIBVISUAL_DIR}"
        COMPONENT libvisual
        )
