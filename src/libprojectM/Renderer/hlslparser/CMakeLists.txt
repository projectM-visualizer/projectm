add_library(hlslparser STATIC
        src/CodeWriter.cpp
        src/CodeWriter.h
        src/Engine.cpp
        src/Engine.h
        src/GLSLGenerator.cpp
        src/GLSLGenerator.h
        src/HLSLParser.cpp
        src/HLSLParser.h
        src/HLSLTokenizer.cpp
        src/HLSLTokenizer.h
        src/HLSLTree.cpp
        src/HLSLTree.h
        )

target_include_directories(hlslparser
        PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
        )
