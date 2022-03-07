if("${CMAKE_HOST_SYSTEM_NAME}" STREQUAL "Windows")
    execute_process(COMMAND "${CMAKE_COMMAND}"
            -G "Visual Studio 17 2022"
            -A "X64"
            -S "$ENV{GITHUB_WORKSPACE}"
            -B "$ENV{GITHUB_WORKSPACE}/cmake-build"
            -DTARGET_TRIPLET=x64-windows
            -DCMAKE_VERBOSE_MAKEFILE=YES
            "-DCMAKE_INSTALL_PREFIX=$ENV{GITHUB_WORKSPACE}/cmake-install"
            "-DCMAKE_TOOLCHAIN_FILE=$ENV{VCPKG_INSTALLATION_ROOT}/scripts/buildsystems/vcpkg.cmake"

            RESULT_VARIABLE result
            )
elseif("${CMAKE_HOST_SYSTEM_NAME}" STREQUAL "Linux")
    execute_process(COMMAND "${CMAKE_COMMAND}"
            -G "Unix Makefiles"
            -S "$ENV{GITHUB_WORKSPACE}"
            -B "$ENV{GITHUB_WORKSPACE}/cmake-build"
            -DCMAKE_VERBOSE_MAKEFILE=YES
            -DCMAKE_BUILD_TYPE=$ENV{BUILD_TYPE}
            "-DCMAKE_INSTALL_PREFIX=$ENV{GITHUB_WORKSPACE}/cmake-install"

            RESULT_VARIABLE result
            )
elseif("${CMAKE_HOST_SYSTEM_NAME}" STREQUAL "Darwin")
    execute_process(COMMAND "${CMAKE_COMMAND}"
            -G "Unix Makefiles"
            -S "$ENV{GITHUB_WORKSPACE}"
            -B "$ENV{GITHUB_WORKSPACE}/cmake-build"
            -DCMAKE_VERBOSE_MAKEFILE=YES
            "-DCMAKE_INSTALL_PREFIX=$ENV{GITHUB_WORKSPACE}/cmake-install"

            RESULT_VARIABLE result
            )
endif()

if(NOT result EQUAL 0)
    message(FATAL_ERROR "CMake returned bad exit status")
endif()
