cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

# Run as custom command in script mode if any shader file has changed.
# Recreates the BuiltInTransitionsResources.hpp file accordingly.

set(STATIC_SHADER_CONTENTS "")
foreach(shader_file IN LISTS SHADER_FILES)
    cmake_path(GET shader_file FILENAME _shader_name)
    cmake_path(GET shader_file EXTENSION _shader_type)
    cmake_path(REMOVE_EXTENSION _shader_name)
    file(READ "${shader_file}" _shader_contents)

    string(REGEX REPLACE "Glsl([0-9]+)" "" _accessor_name ${_shader_name})

    string(APPEND STATIC_SHADER_CONTENTS "static std::string k${_shader_name} = R\"(\n${_shader_contents})\";\n\n")

endforeach()

configure_file(BuiltInTransitionsResources.hpp.in ${OUTPUT_DIR}/BuiltInTransitionsResources.hpp @ONLY)
