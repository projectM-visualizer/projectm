#include <projectM.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <boost/algorithm/string.hpp>
#include <regex>
#include <sys/stat.h>
#include "../libprojectM/Renderer/hlslparser/src/HLSLParser.h"
#include "../libprojectM/Renderer/hlslparser/src/GLSLGenerator.h"

std::string warp_prefix("warp_");
std::string comp_prefix("comp_");

// TODO don't duplicate these values from ShaderEngine.cpp
#ifdef USE_GLES
#define GLSL_VERSION "300 es"
    #define SHADER_VERSION  M4::GLSLGenerator::Version_300_ES
#else
#define GLSL_VERSION "330"
#define SHADER_VERSION  M4::GLSLGenerator::Version_330
#endif
std::string getShaderVersionString()
{
    return GLSL_VERSION;
}
M4::GLSLGenerator::Version getShaderVersionEnum()
{
    return SHADER_VERSION;
}



#define starts_with(s,f)  boost::algorithm::starts_with(s,f)
#define trim(s) boost::algorithm::trim(s)
#define replace_all(s,f,r) boost::algorithm::replace_all(s,f,r)
#define replace_all_copy(s,f,r) boost::algorithm::replace_all_copy(s,f,r)

std::string TextureIncludes =
"uniform float4 texsize_pw_main;\n"
"uniform float4 texsize_pc_main;\n"
"uniform float4 texsize_noisevol_lq;\n"
"uniform float4 texsize_noisevol_hq;\n"
"uniform float4 texsize_noise_mq;\n"
"uniform float4 texsize_noise_lq_lite;\n"
"uniform float4 texsize_noise_lq;\n"
"uniform float4 texsize_noise_hq;\n"
"uniform float4 texsize_main;\n"
"uniform float4 texsize_fw_main;\n"
"uniform float4 texsize_fc_main;\n"
"uniform float4 texsize_blur2;\n"
"uniform float4 texsize_blur1;\n"
"uniform sampler2D sampler_pw_main;\n"
"uniform sampler2D sampler_pc_main;\n"
"uniform sampler3D sampler_noisevol_lq;\n"
"uniform sampler3D sampler_noisevol_hq;\n"
"uniform sampler2D sampler_noise_mq;\n"
"uniform sampler2D sampler_noise_lq_lite;\n"
"uniform sampler2D sampler_noise_lq;\n"
"uniform sampler2D sampler_noise_hq;\n"
"uniform sampler2D sampler_main;\n"
"uniform sampler2D sampler_fw_main;\n"
"uniform sampler2D sampler_fc_main;\n"
"uniform sampler2D sampler_blur2;\n"
"uniform sampler2D sampler_blur1;\n\n";

// see ShaderEngine::PresetShaderIncludes
// replace with #define with declarations of variables/functions because I don't want substitution in the source
std::string PresetShaderIncludes =
"float  M_PI;\n"
"float  M_PI_2;\n"
"float  M_INV_PI_2;\n"

"uniform float4   rand_frame;		// random float4, updated each frame\n"
"uniform float4   rand_preset;   // random float4, updated once per *preset*\n"
"uniform float4   _c0;  // .xy: multiplier to use on UV's to paste an image fullscreen, *aspect-aware*; .zw = inverse.\n"
"uniform float4   _c1, _c2, _c3, _c4;\n"
"uniform float4   _c5;  //.xy = scale,bias for reading blur1; .zw = scale,bias for reading blur2; \n"
"uniform float4   _c6;  //.xy = scale,bias for reading blur3; .zw = blur1_min,blur1_max\n"
"uniform float4   _c7;  // .xy ~= float2(1024,768); .zw ~= float2(1/1024.0, 1/768.0)\n"
"uniform float4   _c8;  // .xyzw ~= 0.5 + 0.5*cos(time * float4(~0.3, ~1.3, ~5, ~20))\n"
"uniform float4   _c9;  // .xyzw ~= same, but using sin()\n"
"uniform float4   _c10;  // .xyzw ~= 0.5 + 0.5*cos(time * float4(~0.005, ~0.008, ~0.013, ~0.022))\n"
"uniform float4   _c11;  // .xyzw ~= same, but using sin()\n"
"uniform float4   _c12;  // .xyz = mip info for main image (.x=#across, .y=#down, .z=avg); .w = unused\n"
"uniform float4   _c13;  //.xy = blur2_min,blur2_max; .zw = blur3_min, blur3_max.\n"
"uniform float4   _qa;  // q vars bank 1 [q1-q4]\n"
"uniform float4   _qb;  // q vars bank 2 [q5-q8]\n"
"uniform float4   _qc;  // q vars ...\n"
"uniform float4   _qd;  // q vars\n"
"uniform float4   _qe;  // q vars\n"
"uniform float4   _qf;  // q vars\n"
"uniform float4   _qg;  // q vars\n"
"uniform float4   _qh;  // q vars bank 8 [q29-q32]\n"

"// note: in general, don't use the current time w/the *dynamic* rotations!\n"
"uniform float4x3 rot_s1;  // four random, static rotations.  randomized @ preset load time.\n"
"uniform float4x3 rot_s2;  // minor translation component (<1).\n"
"uniform float4x3 rot_s3;\n"
"uniform float4x3 rot_s4;\n"

"uniform float4x3 rot_d1;  // four random, slowly changing rotations.\n"
"uniform float4x3 rot_d2;  \n"
"uniform float4x3 rot_d3;\n"
"uniform float4x3 rot_d4;\n"
"uniform float4x3 rot_f1;  // faster-changing.\n"
"uniform float4x3 rot_f2;\n"
"uniform float4x3 rot_f3;\n"
"uniform float4x3 rot_f4;\n"
"uniform float4x3 rot_vf1;  // very-fast-changing.\n"
"uniform float4x3 rot_vf2;\n"
"uniform float4x3 rot_vf3;\n"
"uniform float4x3 rot_vf4;\n"
"uniform float4x3 rot_uf1;  // ultra-fast-changing.\n"
"uniform float4x3 rot_uf2;\n"
"uniform float4x3 rot_uf3;\n"
"uniform float4x3 rot_uf4;\n"

"uniform float4x3 rot_rand1; // random every frame\n"
"uniform float4x3 rot_rand2;\n"
"uniform float4x3 rot_rand3;\n"
"uniform float4x3 rot_rand4;\n"

"float time;\n"
"float fps;\n"
"float frame;\n"
"float progress;\n"
"float bass;\n"
"float mid;\n"
"float treb;\n"
"float vol;\n"
"float bass_att;\n"
"float mid_att;\n"
"float treb_att;\n"
"float vol_att;\n"
"float q1;\n"
"float q2;\n"
"float q3;\n"
"float q4;\n"
"float q5;\n"
"float q6;\n"
"float q7;\n"
"float q8;\n"
"float q9;\n"
"float q10;\n"
"float q11;\n"
"float q12;\n"
"float q13;\n"
"float q14;\n"
"float q15;\n"
"float q16;\n"
"float q17;\n"
"float q18;\n"
"float q19;\n"
"float q20;\n"
"float q21;\n"
"float q22;\n"
"float q23;\n"
"float q24;\n"
"float q25;\n"
"float q26;\n"
"float q27;\n"
"float q28;\n"
"float q29;\n"
"float q30;\n"
"float q31;\n"
"float q32;\n"

"float4 aspect;\n"
"float4 texsize;\n"
"float4 roam_cos;\n"
"float4 roam_sin;\n"
"float4 slow_roam_cos;\n"
"float4 slow_roam_sin;\n"
"float mip_x;\n"
"float mip_y;\n"
"float mip_xy;\n"
"float mip_avg;\n"
"float blur1_min;\n"
"float blur1_max;\n"
"float blur2_min;\n"
"float blur2_max;\n"
"float blur3_min;\n"
"float blur3_max;\n"

"float sampler_FC_main;\n"
"float sampler_PC_main;\n"
"float sampler_FW_main;\n"
"float sampler_PW_main;\n"

"float3 GetMain(in float2 uv) {return float3(0,0,0);}\n"
"float3 GetPixel(in float2 uv) {return float3(0,0,0);}\n"
"float3 GetBlur1(in float2 uv) {return float3(0,0,0);}\n"
"float3 GetBlur2(in float2 uv) {return float3(0,0,0);}\n"
"float3 GetBlur3(in float2 uv) {return float3(0,0,0);}\n"
// NOTE float lum = lum() in preset only woks because lum is a macro, convet lum() -> LUM(), and then back
"float LUM(float3 x) { return 0; }\n"
// some presets call tex3D() on sampler2D which causes an error
"float3 tex3D( sampler2D s, float3 v) { return float3(0,0,0); }"
;


const std::string addl_declarations(
    "float rad;\n"
    "float ang;\n"
    "float2 uv;\n"
    "float2 uv_orig;\n"
    "float3 hue_shader;\n"
    "float3 ret;\n"
    );


std::string hlsl2glsl(const std::string &source, const std::string &shaderTypeString, const std::string &hlsl)
{
    std::string program = hlsl;

    M4::Allocator allocator;
    M4::HLSLTree tree( &allocator );
    M4::HLSLParser parser(&allocator, &tree);

    if (program.find("#define") != std::string::npos || program.find("#if") != std::string::npos)
    {
        std::cerr << "WARN converting file " << source << std::endl;
        std::cerr << "h2g does not preseve #define, #if and other preprocessor commands" << std::endl;
    }

    // replace shader_body with entry point function
    size_t found = program.find("shader_body");
    if (found == std::string::npos)
    {
        std::cerr << "ERROR converting file " << source << std::endl;
        std::cerr << "shader_body not found" << std::endl;
        exit(1);
    }
    program.replace(int(found), 11, "void PS(float4 _vDiffuse : COLOR, float4 _uv : TEXCOORD0, float2 _rad_ang : TEXCOORD1, out float4 _return_value : COLOR)\n");


    // We mostly _do_not_ want to preprocess macros because we are transpiling to generate preset source not to compile
    // however, by doing some minimal preprocessing here we can get more presets to convert
    program.insert(0,
            "#define tex2d tex2D\n"
            "#define tex3d tex3D\n"
            "#define lum(x) LUM(x)\n"           // fix lum = lum()
            );

    std::string sourcePreprocessed;
    if (!parser.ApplyPreprocessor(source.c_str(), program.c_str(), program.size(), sourcePreprocessed))
    {
        std::cerr << "ERROR converting file " << source << std::endl;
        std::cerr << "Failed to preprocess HLSL(step1) " << shaderTypeString << " shader" << std::endl;
        std::cerr << program << std::endl;
        exit(1);
    }

    // UNDO the mangling of sampler2D and sampler3D
    std::smatch matches;
    while(std::regex_search(sourcePreprocessed, matches, std::regex("sampler(2D|3D|)\\([^\\)]*\\)")))
    {
        std::string sampler = sourcePreprocessed.substr(matches.position(), matches.length());
        std::string new_declaration = sampler;
        replace_all(new_declaration,"("," ");
        replace_all(new_declaration,")","");
        sourcePreprocessed.replace(matches.position(), matches.length(), new_declaration);
    }
//    replace_all(sourcePreprocessed, "sampler2D", "uniform sampler2D ");
//    replace_all(sourcePreprocessed, "sampler3D", "uniform sampler3D ");

//
//    // Remove previous texsize declarations
//    while(std::regex_search(sourcePreprocessed, matches, std::regex("float4\\s+texsize_.*"))) {
//        sourcePreprocessed.replace(matches.position(), matches.length(), "");
//    }

    std::string START_SOURCE = "float SHADER_START;";
    std::string END_SOURCE = "float SHADER_END;";

    std::string fullsource;
    fullsource.append(TextureIncludes);
    fullsource.append(PresetShaderIncludes);
    fullsource.append(addl_declarations);
    fullsource.append("\n");
    fullsource.append(START_SOURCE);
    fullsource.append("\n");
    fullsource.append(sourcePreprocessed);
    fullsource.append("\n");
    fullsource.append(END_SOURCE);
    fullsource.append("\n");

    if( !parser.Parse("input", fullsource.c_str(), fullsource.size()) )
    {
        std::cerr << "ERROR converting file " << source << std::endl;
        std::cerr << "Failed to parse HLSL (step2) " << shaderTypeString << " shader" << std::endl;
        std::cerr << fullsource << std::endl;
        exit(1);
    }

    M4::GLSLGenerator generator;

    if (!generator.Generate(&tree, M4::GLSLGenerator::Target_FragmentShader, getShaderVersionEnum(), "PS"))
    {
        std::cerr << "ERROR converting file " << source << std::endl;
        std::cerr << "Failed to transpile HLSL(step3) " << getShaderVersionString() << " shader to GLSL" << std::endl;
        std::cerr << fullsource << std::endl;
        exit(1);
    }

    std::string result;
    std::stringstream ss(generator.GetResult());
    bool started=false;
    std::string line;
    while (std::getline(ss, line,'\n'))
    {
        if (!started)
        {
            if (starts_with(line, START_SOURCE))
                started = true;
        }
        else if (starts_with(line, END_SOURCE))
            break;
        else if (starts_with(line, "void PS("))
            result.append("shader_body {\n");
        else
        {
            replace_all(line, "LUM(", "lum(");
            result.append(line + "\n");
        }
    }
    return result;
}


void output_program(std::ostream &output, const std::string &prefix, const std::string &program)
{
    size_t line_count = 0;
    std::stringstream ss(program);
    std::string line;
    while (std::getline(ss, line,'\n'))
    {
        output << prefix << ++line_count << "=`" << line << std::endl;
    }
    output.flush();
}


int main(int argc, char *argv[])
{
    std::string source = "-";
    std::istream *pinput = &std::cin;
    std::ostream *poutput = &std::cout;

    if (argc >= 2)
    {
        source = argv[1];
        pinput = new std::ifstream(argv[1]);
    }

    if (argc >= 3)
    {
        std::string outputfile = argv[2];

        struct stat s = {0};
        if (0 == stat(argv[2],&s))
        {
            // todo handle case where input file has directory
            if (s.st_mode & S_IFDIR)
                outputfile += "/" + source;
        }
        poutput = new std::ofstream(outputfile);
    }

    std::istream &input = *pinput;
    std::ostream &output = *poutput;

    std::string line;
    std::string warp;
    std::string comp;
    while (std::getline(input, line))
    {
        if (starts_with(line,warp_prefix) || starts_with(line,comp_prefix))
        {
            size_t eq = line.find('=');
            if (eq == std::string::npos)
                continue;
            std::string rhs = line.substr(eq+1);
            trim(rhs);
            if (rhs.length()>0 && rhs[0] == '`')
                rhs = rhs.substr(1);
            if (starts_with(line,warp_prefix))
                warp += rhs + "\n";
            else
                comp += rhs + "\n";
        }
        else
        {
            output << line << std::endl;
        }
    }
    output.flush();

    trim(warp);
    std::string warp_glsl;
    if (warp.length() > 0)
    {
        warp_glsl = hlsl2glsl(source, "warp", warp);
        output << warp_prefix << "lang=glsl_" << replace_all_copy(getShaderVersionString(), " ", "_") << std::endl;
        output_program(output, warp_prefix, warp_glsl);
    }

    trim(comp);
    std::string comp_glsl;
    if (comp.length() > 0)
    {
        comp_glsl = hlsl2glsl(source, "comp", comp);
        output << comp_prefix << "lang=glsl_" << replace_all_copy(getShaderVersionString(), " ", "_") << std::endl;
        output_program(output, comp_prefix, comp_glsl);
    }

//    std::cerr << "\n - WARP HLSL - \n\n";
//    std::cerr << warp;
//    std::cerr << "\n - WARP GLSL - \n\n";
//    std::cerr << warp_glsl;
//    std::cerr << "\n - COMP HLSL - \n\n";
//    std::cerr << comp;
//    std::cerr << "\n - COMP GLSL - \n\n";
//    std::cerr << comp_glsl;
//    std::cerr.flush();

    exit(0);
}