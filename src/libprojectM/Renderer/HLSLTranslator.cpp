#ifndef HLSLTRANSLATOR_CPP_
#define HLSLTRANSLATOR_CPP_

#include "HLSLTranslator.hpp"

using namespace M4;

std::unique_ptr<std::string> HLSLTranslator::parse(GLenum shaderType, const char *fileName, const char *source, size_t size) {
    // alloc
    GLSLGenerator generator;
    Allocator allocator;
    HLSLParser parser(&allocator, fileName, source, size);
    HLSLTree tree( &allocator );
    
    // parse
    if( !parser.Parse( &tree ) ) {
        fprintf(stderr, "Failed to parse HLSL shader\n");
        return nullptr;
    }
    
    // figure out what sort of shader we are generating
    GLSLGenerator::Target target;
    switch (shaderType) {
        case GL_VERTEX_SHADER:
            target = GLSLGenerator::Target_VertexShader;
            break;
        case GL_FRAGMENT_SHADER:
            target = GLSLGenerator::Target_FragmentShader;
            break;
        default:
            fprintf(stderr, "Unsupported shader type\n");
            return nullptr;
    }

    // generate GLSL
    if (!generator.Generate( &tree, GLSLGenerator::Target(shaderType), GLSLGenerator::Version_140, "shader_body" )) {
        fprintf(stderr, "Failed to transpile HLSL shader to GLSL\n");
        return nullptr;
    }
    
    return std::make_unique<std::string>(generator.GetResult());
}


#endif
