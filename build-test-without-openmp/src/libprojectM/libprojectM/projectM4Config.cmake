set(projectM4_VERSION 4.1.0)


####### Expanded from @PACKAGE_INIT@ by configure_package_config_file() #######
####### Any changes to this file will be overwritten by the next CMake run ####
####### The input file was projectM4Config.cmake.in                            ########

get_filename_component(PACKAGE_PREFIX_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../" ABSOLUTE)

macro(set_and_check _var _file)
  set(${_var} "${_file}")
  if(NOT EXISTS "${_file}")
    message(FATAL_ERROR "File or directory ${_file} referenced by variable ${_var} does not exist !")
  endif()
endmacro()

macro(check_required_components _NAME)
  foreach(comp ${${_NAME}_FIND_COMPONENTS})
    if(NOT ${_NAME}_${comp}_FOUND)
      if(${_NAME}_FIND_REQUIRED_${comp})
        set(${_NAME}_FOUND FALSE)
      endif()
    endif()
  endforeach()
endmacro()

####################################################################################

include(CMakeFindDependencyMacro)

if(NOT "OFF") # ENABLE_EMSCRIPTEN
    if("OFF") # ENABLE_GLES
        list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}")
        find_dependency(OpenGL COMPONENTS GLES3)
    else()
        find_dependency(OpenGL)
    endif()
endif()
if("OFF") # ENABLE_BOOST_FILESYSTEM
    if(POLICY CMP0167)
        cmake_policy(SET CMP0167 NEW)
    endif()

    find_dependency(Boost CONFIG NO_MODULE COMPONENTS Filesystem)
endif()
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
     find_dependency(GLEW)
endif()

set(_projectM4_FIND_PARTS_REQUIRED)
if(projectM4_FIND_REQUIRED)
    set(_projectM4_FIND_PARTS_REQUIRED REQUIRED)
endif()
set(_projectM4_FIND_PARTS_QUIET)
if(projectM4_FIND_QUIETLY)
    set(_projectM4_FIND_PARTS_QUIET QUIET)
endif()

include("${CMAKE_CURRENT_LIST_DIR}/projectM4Targets.cmake")

if(projectM4_FIND_COMPONENTS)
    foreach(component ${projectM4_FIND_COMPONENTS})
        find_package(projectM4${component}
            ${_projectM4_FIND_PARTS_REQUIRED}
            ${_projectM4_FIND_PARTS_QUIET}
            )

        if(NOT projectM4${component}_FOUND)
            if (projectM4_FIND_REQUIRED_${component})
                set(_projectM4_NOTFOUND_MESSAGE "${_libprojectM_NOTFOUND_MESSAGE}Failed to find libprojectM component \"${component}\" config file\n")
            elseif(NOT projectM4_FIND_QUIETLY)
                message(WARNING "Failed to find projectM4 component \"${component}\" config file")
            endif()
        endif()
    endforeach()
endif()

if (_projectM4_NOTFOUND_MESSAGE)
    set(projectM4_NOT_FOUND_MESSAGE "${_libprojectM_NOTFOUND_MESSAGE}")
    set(projectM4_FOUND False)
endif()
