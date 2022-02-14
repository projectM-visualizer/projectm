message(STATUS "Using host CMake version: ${CMAKE_VERSION}")

if("${CMAKE_HOST_SYSTEM_NAME}" STREQUAL "Windows")
    # On Windows, using vcpkg to install and build is the best practice.
    set(VCPKG "$ENV{VCPKG_INSTALLATION_ROOT}/vcpkg.exe")
    execute_process(COMMAND "${VCPKG}" --triplet=x64-windows install glew sdl2

            RESULT_VARIABLE result
            )
elseif("${CMAKE_HOST_SYSTEM_NAME}" STREQUAL "Linux")
    # On Ubuntu, installing the required dev packages is sufficient
    message(STATUS "Updating apt package sources")
    execute_process(COMMAND sudo apt-get update
            COMMAND sudo apt-get -f install

            RESULT_VARIABLE result
            )

    if(NOT result EQUAL 0)
        message(FATAL_ERROR "Could not update apt package lists")
    endif()

    execute_process(COMMAND sudo apt-get install
            libgl1-mesa-dev
            mesa-common-dev
            libsdl2-dev
            libglm-dev
            llvm-dev

            RESULT_VARIABLE result
            )

elseif("${CMAKE_HOST_SYSTEM_NAME}" STREQUAL "Darwin")
    # macOS uses Homebrew to install additional software packages.
    execute_process(COMMAND brew update
            COMMAND brew install sdl2

            RESULT_VARIABLE result
            )
endif()

if(NOT result EQUAL 0)
    message(FATAL_ERROR "A command returned bad exit status")
endif()
