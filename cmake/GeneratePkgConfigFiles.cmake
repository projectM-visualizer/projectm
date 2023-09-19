macro(GENERATE_PKG_CONFIG_FILES target package_name)

    if(UNIX)
        macro(set_pkg_config_path varname path)
            if(IS_ABSOLUTE "${path}")
                set(${varname} "${path}")
            else()
                set(${varname} "\${prefix}/${path}")
            endif()
        endmacro()

        set(PKGCONFIG_PACKAGE_NAME "${package_name}")
        set(PKGCONFIG_PREFIX "${CMAKE_INSTALL_PREFIX}")
        set_pkg_config_path(PKGCONFIG_LIB_DIR "${PROJECTM_LIB_DIR}")
        set_pkg_config_path(PKGCONFIG_INCLUDE_DIR "${PROJECTM_INCLUDE_DIR}")
        set_pkg_config_path(PKGCONFIG_DATADIR_PATH "${PROJECTM_DATADIR_PATH}")

        # Get exported target defines
        get_target_property(_exported_defines ${target} INTERFACE_COMPILE_DEFINITIONS)
        if(_exported_defines)
            foreach(_define ${_exported_defines})
                set(PKGCONFIG_FLAGS "${PKGCONFIG_FLAGS} -D${_define}")
            endforeach()
        endif()

        # Get name for "-l:<name>" linker flags
        get_target_property(_lib_name ${target} OUTPUT_NAME)

        # Using different package name for debug and release, as pkg-config doesn't support
        # multi-config packages such as CMake provides. It's a mess.
        set(PKGCONFIG_PROJECTM_LIBRARY "${_lib_name}")
        set(PKGCONFIG_PACKAGE_REQUIREMENTS "${PKGCONFIG_PACKAGE_REQUIREMENTS_ALL} ${PKGCONFIG_PACKAGE_REQUIREMENTS_RELEASE}")
        configure_file(${PROJECTM_SOURCE_DIR}/cmake/pkgconfig-file.in "${CMAKE_CURRENT_BINARY_DIR}/${package_name}.pc" @ONLY)
        install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${package_name}.pc"
                CONFIGURATIONS Release;RelWithDebInfo;MinSizeRel
                DESTINATION "${PROJECTM_LIB_DIR}/pkgconfig"
                COMPONENT Devel
                )

        set(PKGCONFIG_PROJECTM_LIBRARY "${_lib_name}${CMAKE_DEBUG_POSTFIX}")
        set(PKGCONFIG_PACKAGE_REQUIREMENTS "${PKGCONFIG_PACKAGE_REQUIREMENTS_ALL} ${PKGCONFIG_PACKAGE_REQUIREMENTS_DEBUG}")
        configure_file(${PROJECTM_SOURCE_DIR}/cmake/pkgconfig-file.in "${CMAKE_CURRENT_BINARY_DIR}/${package_name}-debug.pc" @ONLY)
        install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${package_name}-debug.pc"
                CONFIGURATIONS Debug
                DESTINATION "${PROJECTM_LIB_DIR}/pkgconfig"
                COMPONENT Devel
                )

        unset(_lib_name)
        unset(_output_name)
        unset(_exported_defines)
    endif()

endmacro()