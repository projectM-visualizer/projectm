# Check if compiler really support C++17. If not, require boost::filesystem.
# We also provide an option to force the use of boost.

option(ENABLE_BOOST_FILESYSTEM "Force the use of boost::filesystem, even if the compiler supports C++17." OFF)

set(CMAKE_CXX_STANDARD 17)
include(CheckSourceCompiles)
check_source_compiles(CXX
        "#include <filesystem>
        int main(){
           std::filesystem::path p;
           return 0;
        }"
        STD_FILESYSTEM_EXISTS
        )
set(CMAKE_CXX_STANDARD 14)

if(NOT ENABLE_BOOST_FILESYSTEM AND STD_FILESYSTEM_EXISTS)
    message(STATUS "Building playlist library using C++17 and std::filesystem.")

    set_target_properties(projectM_playlist_main PROPERTIES
            CXX_STANDARD 17
            )

    target_compile_definitions(projectM_playlist_main
            PRIVATE
            FS_NAMESPACE=std
            FS_INCLUDE=<filesystem>
            )
else()
    message(STATUS "Compiler does not support std::filesystem, reverting to boost::filesystem.")
    find_package(Boost REQUIRED COMPONENTS Filesystem)

    target_compile_definitions(projectM_playlist_main
            PRIVATE
            FS_NAMESPACE=boost
            FS_INCLUDE=<boost/filesystem.hpp>
            )

    target_link_libraries(projectM_playlist_main
            PUBLIC
            Boost::filesystem
            )

    target_link_libraries(projectM_playlist
            PUBLIC
            Boost::filesystem
            )
endif()
