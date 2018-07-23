#ifndef HLSLTRANSLATOR_CPP_
#define HLSLTRANSLATOR_CPP_

#include <memory>
#include <fstream>
#include <iostream>

#include "HLSLTranslator.hpp"

using namespace M4;


std::unique_ptr<std::string> HLSLTranslator::parse(const std::string & shaderType, const char *fileName, const std::string &fullSource) {
    // alloc
    GLSLGenerator generator;
    Allocator allocator;

    HLSLTree tree( &allocator );
    HLSLParser parser(&allocator, &tree);

    // preprocess define macros
    std::string sourcePreprocessed;
    parser.ApplyPreprocessor(fileName, fullSource.c_str(), fullSource.size(), sourcePreprocessed);

    // parse
    if( !parser.Parse(fileName, sourcePreprocessed.c_str(), sourcePreprocessed.size()) ) {
        std::cerr << "Failed to parse HLSL (" << shaderType << ") shader" << std::endl;

#ifndef DUMP_SHADERS_ON_ERROR
        std::cerr << "Source: " << std::endl << sourcePreprocessed << std::endl;
#else
        std::ofstream out("/tmp/shader_" + shaderType + ".txt");
            out << fullSource;
            out.close();

        std::ofstream out2("/tmp/shader_preproc_" + shaderType + ".txt");
            out2 << sourcePreprocessed;
            out2.close();
#endif
        return nullptr;
    }

    // generate GLSL
    if (!generator.Generate(&tree, GLSLGenerator::Target_FragmentShader, GLSLGenerator::Version_140, "PS")) {
        std::cerr << "Failed to transpile HLSL (" << shaderType << ") shader to GLSL" << std::endl;
#ifndef DUMP_SHADERS_ON_ERROR
        std::cerr << "Source: " << std::endl << sourcePreprocessed << std::endl;
#else
        std::ofstream out2("/tmp/shader_preproc_" + shaderType + ".txt");
            out2 << sourcePreprocessed;
            out2.close();
#endif
        return nullptr;
    }

    return std::make_unique<std::string>(generator.GetResult());
}


#endif
