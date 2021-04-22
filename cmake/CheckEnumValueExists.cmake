include_guard()

include(CheckCSourceCompiles)

macro(check_enum_value_exists enum_value include variable)
    if(NOT DEFINED ${variable})
        message(STATUS "Looking for enum value ${enum_value}")
        set(_CMAKE_REQUIRED_QUIET_tmp "${CMAKE_REQUIRED_QUIET}")
        set(CMAKE_REQUIRED_QUIET ON)
        check_c_source_compiles("
#include <${include}>
int main() {
  int tmp = ${enum_value};
  return 0;
}
"
                ${variable}
                )
        if(${variable})
            message(STATUS "Looking for enum value ${enum_value} - found")
        else()
            message(STATUS "Looking for enum value ${enum_value} - not found")
        endif()
        set(CMAKE_REQUIRED_QUIET "${_CMAKE_REQUIRED_QUIET_tmp}")
    endif()
endmacro()
