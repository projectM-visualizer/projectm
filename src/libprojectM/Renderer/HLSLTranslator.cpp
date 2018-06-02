#ifndef HLSLTRANSLATOR_CPP_
#define HLSLTRANSLATOR_CPP_

#include "HLSLTranslator.hpp"

using namespace M4;

// http://www.geisswerks.com/milkdrop/milkdrop_preset_authoring.html#3f6
std::string HLSLShaderTemplate = ""
"    static const float  M_PI = 3.14159265359;\n"
"    static const float  M_PI_2 = 6.28318530718;\n"
"    static const float  M_INV_PI_2 = 0.159154943091895;\n"

"    float q1;\n"
"    float q2;\n"
"    float q3;\n"
"    float q4;\n"
"    float q5;\n"
"    float q6;\n"
"    float q7;\n"
"    float q8;\n"
"    float q9;\n"
"    float q10;\n"
"    float q11;\n"
"    float q12;\n"
"    float q13;\n"
"    float q14;\n"
"    float q15;\n"
"    float q16;\n"
"    float q17;\n"
"    float q18;\n"
"    float q19;\n"
"    float q20;\n"
"    float q21;\n"
"    float q22;\n"
"    float q23;\n"
"    float q24;\n"
"    float q25;\n"
"    float q26;\n"
"    float q27;\n"
"    float q28;\n"
"    float q29;\n"
"    float q30;\n"
"    float q31;\n"
"    float q32;\n"

//"    #define tex2d tex2D\n"
//"    #define tex3d tex3D\n"
//
//"    #define sampler sampler2D\n"

// get the color of a pixel rendered by the preset
"    uniform sampler2D sampler_main;\n"

"    uniform sampler2D sampler_fw_main;\n"
"    uniform sampler2D sampler_pw_main;\n"
"    uniform sampler2D sampler_fc_main;\n"
"    uniform sampler2D sampler_pc_main;\n"

"    uniform sampler2D sampler_noise_lq;\n"
"    uniform sampler2D sampler_noise_lq_lite;\n"
"    uniform sampler2D sampler_noise_mq;\n"
"    uniform sampler2D sampler_noise_hq;\n"
"    uniform sampler2D sampler_noise_perlin;\n"
"    uniform sampler3D sampler_noisevol_lq;\n"
"    uniform sampler3D sampler_noisevol_hq;\n"

"    uniform sampler2D sampler_blur1;\n"
"    uniform sampler2D sampler_blur2;\n"
"    uniform sampler2D sampler_blur3;\n"

"    float4 texsize_noise_lq;\n"
"    float4 texsize_noise_mq;\n"
"    float4 texsize_noise_hq;\n"
"    float4 texsize_noise_perlin;\n"
"    float4 texsize_noise_lq_lite;\n"
"    float4 texsize_noisevol_hq;\n"

"    float blur1_min;\n"
"    float blur1_max;\n"
"    float blur2_min;\n"
"    float blur2_max;\n"
"    float blur3_min;\n"
"    float blur3_max;\n"

"    float4 slow_roam_cos;\n"
"    float4 roam_cos;\n"
"    float4 slow_roam_sin;\n"
"    float4 roam_sin;\n"

"    float time;\n"
"    float4 rand_preset;\n"
"    float4 rand_frame;\n"
"    float  progress;\n"
"    float  frame;\n"
"    float  fps;\n"
"    float  bass;\n"
"    float  mid;\n"
"    float  treb;\n"
"    float  vol;\n"
"    float  bass_att;\n"
"    float  mid_att;\n"
"    float  treb_att;\n"
"    float  vol_att;\n"
"    float4 texsize;\n"
"    float4 aspect;\n"

//"    #define getrad sqrt((uv.x-0.5)*2*(uv.x-0.5)*2+(uv.y-0.5)*2*(uv.y-0.5)*2)*.7071067\n"
//"    #define getang atan2(((uv.y-0.5)*2),((uv.x-0.5)*2))\n"
"    float3 getrad(float2 uv) { return sqrt((uv.x-0.5)*2*(uv.x-0.5)*2+(uv.y-0.5)*2*(uv.y-0.5)*2)*.7071067; };\n"
"    float3 getang(float2 uv) { return atan2(((uv.y-0.5)*2),((uv.x-0.5)*2)); };\n"

//"    #define GetMain(uv) (tex2D(sampler_main,uv).xyz)\n"
//"    #define GetPixel(uv) (tex2D(sampler_main,uv).xyz)\n"
"    float3 GetMain(float2 uv) { return tex2D(sampler_main,uv).xyz; }\n"
"    float3 GetPixel(float2 uv) { return tex2D(sampler_main,uv).xyz; }\n"
"    float lum(float3 x) { return dot(x, float3(0.32,0.49,0.29)); }\n"

"    float3 GetBlur1(float2 uv) {return tex2D(sampler_blur1,uv).xyz*blur1_max+blur1_min;}\n"
"    float3 GetBlur2(float2 uv) {return tex2D(sampler_blur2,uv).xyz*blur2_max+blur2_min;}\n"
"    float3 GetBlur3(float2 uv) {return tex2D(sampler_blur3,uv).xyz*blur3_max+blur3_min;}\n"


"    float max( float a, float b ) {\n"
"        return (a > b ? a : b);\n"
"    }\n"

"    float3 max(float3 a, float4 b) {\n"
"        return float3( a.x > b.x ? a.x : b.x,\n"
"                      a.y > b.y ? a.y : b.y,\n"
"                      a.z > b.z ? a.z : b.z );\n"
"    }\n"

"    float3 max(float3 a, float3 b) {\n"
"        return float3( a.x > b.x ? a.x : b.x,\n"
"                      a.y > b.y ? a.y : b.y,\n"
"                      a.z > b.z ? a.z : b.z );\n"
"    }\n"

"    struct outtype {float4 color : COLOR;};\n";


std::unique_ptr<std::string> HLSLTranslator::parse(GLenum shaderType, const char *fileName, std::string &source) {
    // alloc
    GLSLGenerator generator;
    Allocator allocator;
    
    // prepend our Cg/HLSL template to the actual program source
    std::string fullSource;
    fullSource.append(HLSLShaderTemplate);
    fullSource.append(source);
    
    // parse
    HLSLParser parser(&allocator, fileName, fullSource.c_str(), fullSource.size());
    HLSLTree tree( &allocator );
    if( !parser.Parse(&tree) ) {
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
    if (!generator.Generate(&tree, GLSLGenerator::Target(shaderType), GLSLGenerator::Version_150, "projectm")) {
        fprintf(stderr, "Failed to transpile HLSL shader to GLSL\n");
        return nullptr;
    }
    
    return std::make_unique<std::string>(generator.GetResult());
}


#endif
