cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

# Run as custom command in script mode if any shader file has changed.
# Recreates the MilkdropStaticShaders.hpp/.cpp files accordingly.

set(STATIC_SHADER_CONTENTS "")
set(STATIC_SHADER_ACCESSOR_DECLARATIONS "")
set(STATIC_SHADER_ACCESSOR_DEFINITIONS "")
foreach(shader_file IN LISTS SHADER_FILES)
    cmake_path(GET shader_file FILENAME _shader_name)
    cmake_path(GET shader_file EXTENSION _shader_type)
    cmake_path(REMOVE_EXTENSION _shader_name)
    file(READ "${shader_file}" _shader_contents)

    string(REGEX REPLACE "Glsl([0-9]+)" "" _accessor_name ${_shader_name})

    string(APPEND STATIC_SHADER_CONTENTS "static std::string k${_shader_name} = R\"(\n${_shader_contents})\";\n\n")
    string(APPEND STATIC_SHADER_ACCESSOR_DECLARATIONS "    std::string Get${_accessor_name}();\n")

    if(_shader_type STREQUAL ".inc")
        # Includes don't get the header prepended.
        string(APPEND STATIC_SHADER_ACCESSOR_DEFINITIONS "DECLARE_SHADER_ACCESSOR_NO_HEADER(${_accessor_name});\n")
    else()
        string(APPEND STATIC_SHADER_ACCESSOR_DEFINITIONS "DECLARE_SHADER_ACCESSOR(${_accessor_name});\n")
    endif()
endforeach()

configure_file(MilkdropStaticShaders.hpp.in ${OUTPUT_DIR}/MilkdropStaticShaders.hpp @ONLY)
configure_file(MilkdropStaticShaders.cpp.in ${OUTPUT_DIR}/MilkdropStaticShaders.cpp @ONLY)
