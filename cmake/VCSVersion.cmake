set(PROJECTM_VCS_VERSION "Unknown" CACHE STRING "projectM version control revision number, e.g. Git commit hash")

# Currently only supporting Git.
find_package(Git)

if(Git_FOUND)
    execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse HEAD
            OUTPUT_VARIABLE _git_ref
            ERROR_QUIET
            OUTPUT_STRIP_TRAILING_WHITESPACE
            )

    if(NOT _git_ref STREQUAL "")
        set(PROJECTM_VCS_VERSION "${_git_ref}" CACHE STRING "projectM version control revision number, e.g. Git commit hash" FORCE)
        message(STATUS "Git revision: ${_git_ref}")
    endif()
endif()
