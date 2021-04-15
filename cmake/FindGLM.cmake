# The GLM library is header-only, so we only need to find the glm/glm.hpp header.
find_path(GLM_INCLUDE_DIR
        glm/glm.hpp
        )

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(GLM DEFAULT_MSG GLM_INCLUDE_DIR)

if(GLM_FOUND AND NOT TARGET GLM::GLM)
    add_library(GLM::GLM INTERFACE)

    target_include_directories(GLM::GLM
            INTERFACE
            ${GLM_INCLUDE_DIR}
            )
endif()
