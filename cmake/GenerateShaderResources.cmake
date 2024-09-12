macro(GENERATE_SHADER_RESOURCES _output_file)
    string(REPLACE ";" "\\;" SHADER_FILES_ARG "${ARGN}")
    add_custom_command(OUTPUT
            "${_output_file}"

            COMMAND ${CMAKE_COMMAND}

            ARGS
            -D "SHADER_FILES=${SHADER_FILES_ARG}"
            -D "OUTPUT_FILE=${_output_file}"
            -P "${PROJECTM_SOURCE_DIR}/cmake/GenerateShaderResourcesScript.cmake"

            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            DEPENDS # Watch scripts and shader files for changes
            ${PROJECTM_SOURCE_DIR}/cmake/ShaderResources.hpp.in
            ${PROJECTM_SOURCE_DIR}/cmake/GenerateShaderResources.cmake
            ${PROJECTM_SOURCE_DIR}/cmake/GenerateShaderResourcesScript.cmake
            ${ARGN}
            )
endmacro()
