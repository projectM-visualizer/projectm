# Find and use llvm-config to determine the proper library and include locations.

find_program(LLVM_CONFIG_COMMAND
        NAMES llvm-config llvm-config.exe
        )

if(LLVM_CONFIG_COMMAND)
    execute_process(COMMAND
            ${LLVM_CONFIG_COMMAND} --version
            OUTPUT_VARIABLE LLVM_VERSION
            )
    string(STRIP "${LLVM_VERSION}" LLVM_VERSION)

    execute_process(COMMAND
            ${LLVM_CONFIG_COMMAND} --includedir
            OUTPUT_VARIABLE LLVM_INCLUDE_DIR
            )
    string(STRIP "${LLVM_INCLUDE_DIR}" LLVM_INCLUDE_DIR)

    execute_process(COMMAND
            ${LLVM_CONFIG_COMMAND} --libdir
            OUTPUT_VARIABLE LLVM_LIB_DIR
            )
    string(STRIP "${LLVM_LIB_DIR}" LLVM_LIB_DIR)

    execute_process(COMMAND
            ${LLVM_CONFIG_COMMAND} --libs
            OUTPUT_VARIABLE LLVM_LIBRARIES
            )
    string(STRIP "${LLVM_LIBRARIES}" LLVM_LIBRARIES)

    execute_process(COMMAND
            ${LLVM_CONFIG_COMMAND} --ldflags
            OUTPUT_VARIABLE LLVM_LDFLAGS
            )
    string(STRIP "${LLVM_LDFLAGS}" LLVM_LDFLAGS)
endif()

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(LLVM
        REQUIRED_VARS LLVM_CONFIG_COMMAND LLVM_LIBRARIES LLVM_LIB_DIR LLVM_INCLUDE_DIR LLVM_LDFLAGS
        VERSION_VAR LLVM_VERSION
        )

if(LLVM_FOUND AND NOT TARGET LLVM::LLVM)
    add_library(LLVM::LLVM INTERFACE IMPORTED)

    set_target_properties(LLVM::LLVM PROPERTIES
            INTERFACE_LINK_LIBRARIES "${LLVM_LIBRARIES}"
            INTERFACE_LINK_DIRECTORIES "${LLVM_LIB_DIR}"
            INTERFACE_INCLUDE_DIRECTORIES "${LLVM_INCLUDE_DIR}"
            INTERFACE_LINK_OPTIONS "${LLVM_LDFLAGS}"
            )
endif()
