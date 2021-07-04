if("${CMAKE_HOST_SYSTEM_NAME}" STREQUAL "Windows")
    execute_process(COMMAND "${CMAKE_COMMAND}" --build "$ENV{GITHUB_WORKSPACE}/cmake-build"
            --config $ENV{BUILD_TYPE}

            RESULT_VARIABLE result
            )
elseif("${CMAKE_HOST_SYSTEM_NAME}" STREQUAL "Linux")
    execute_process(COMMAND "${CMAKE_COMMAND}" --build "$ENV{GITHUB_WORKSPACE}/cmake-build"
            -- -j 3

            RESULT_VARIABLE result
            )
elseif("${CMAKE_HOST_SYSTEM_NAME}" STREQUAL "Darwin")
    execute_process(COMMAND "${CMAKE_COMMAND}" --build "$ENV{GITHUB_WORKSPACE}/cmake-build"
            --config $ENV{BUILD_TYPE}
            -- -j 5

            RESULT_VARIABLE result
            )
endif()

if(NOT result EQUAL 0)
    message(FATAL_ERROR "CMake returned bad exit status")
endif()
