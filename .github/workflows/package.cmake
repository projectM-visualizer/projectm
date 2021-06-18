if("${CMAKE_HOST_SYSTEM_NAME}" STREQUAL "Windows")
    execute_process(COMMAND "${CMAKE_CPACK_COMMAND}"
            -G ZIP
            --config "$ENV{GITHUB_WORKSPACE}/cmake-build/CPackConfig.cmake"
            -B "$ENV{GITHUB_WORKSPACE}/package"

            RESULT_VARIABLE result
            )
else("${CMAKE_HOST_SYSTEM_NAME}" STREQUAL "Linux")
    execute_process(COMMAND "${CMAKE_CPACK_COMMAND}"
            -G TGZ
            --config "$ENV{GITHUB_WORKSPACE}/cmake-build/CPackConfig.cmake"
            -B "$ENV{GITHUB_WORKSPACE}/package"

            RESULT_VARIABLE result
            )
endif()

if(NOT result EQUAL 0)
    message(FATAL_ERROR "CPack returned bad exit status")
endif()
