# Check if compiler really supports C++17. If not, require boost::filesystem.
# We also provide an option in the main CMakeLists.txt to force the use of boost.

set(_old_CMAKE_CXX_STANDARD ${CMAKE_CXX_STANDARD})
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
set(CMAKE_CXX_STANDARD ${_old_CMAKE_CXX_STANDARD})
unset(_old_CMAKE_CXX_STANDARD)

if(NOT ENABLE_BOOST_FILESYSTEM AND STD_FILESYSTEM_EXISTS)
    set(CMAKE_CXX_STANDARD 17)

    set(PROJECTM_FILESYSTEM_NAMESPACE std)
    set(PROJECTM_FILESYSTEM_INCLUDE <filesystem>)
    set(PROJECTM_FILESYSTEM_USE_BOOST FALSE)
else()
    find_package(Boost REQUIRED COMPONENTS Filesystem)

    set(PROJECTM_FILESYSTEM_NAMESPACE boost)
    set(PROJECTM_FILESYSTEM_INCLUDE <boost/filesystem.hpp>)
    set(PROJECTM_FILESYSTEM_USE_BOOST TRUE)

    set(PROJECTM_FILESYSTEM_LIBRARY Boost::filesystem)
    set(ENABLE_BOOST_FILESYSTEM ON CACHE BOOL "Compiler does not support std::filesystem, reverting to boost::filesystem." FORCE)
endif()
