cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

option(ENABLE_BOOST_FILESYSTEM "Force the use of boost::filesystem, even if the compiler supports C++17." OFF)

if(ENABLE_BOOST_FILESYSTEM)
    list(APPEND VCPKG_MANIFEST_FEATURES boost-filesystem)
endif()

project(projectM_CXX_Test
        LANGUAGES C CXX
        )

find_package(projectM4 REQUIRED)

add_executable(projectM4_CXX_Test
        main.cpp
        )

target_link_libraries(projectM4_CXX_Test
        libprojectM::projectM
        )
