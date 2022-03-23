execute_process(COMMAND "${CMAKE_CTEST_COMMAND}" --build "$ENV{GITHUB_WORKSPACE}/cmake-build"
        --build-config $ENV{BUILD_TYPE}
        -V
        WORKING_DIRECTORY "$ENV{GITHUB_WORKSPACE}/cmake-build"

        RESULT_VARIABLE result
        )

if(NOT result EQUAL 0)
    message(FATAL_ERROR "CTest returned bad exit status")
endif()
