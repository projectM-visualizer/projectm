add_library(omptl INTERFACE)

target_sources(omptl
        INTERFACE
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

target_include_directories(omptl
        INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/..
        )
