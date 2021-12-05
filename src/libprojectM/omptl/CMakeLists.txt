add_library(omptl INTERFACE)

target_sources(omptl
        omptl
        omptl_algorithm
        omptl_algorithm_par.h
        omptl_algorithm_ser.h
        omptl_numeric
        omptl_numeric_extentions.h
        omptl_numeric_extentions_par.h
        omptl_numeric_extentions_ser.h
        omptl_numeric_par.h
        omptl_numeric_ser.h
        omptl_tools.h
        )

# MSVC standard library doesn't define WORD_BIT (from xopen_lim.h)
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        target_compile_definitions(omptl
                INTERFACE
                -DWORD_BIT=64
                )
    else()
        target_compile_definitions(omptl
                INTERFACE
                -DWORD_BIT=32
                )
    endif()
endif()
