#include "MilkdropShader.hpp"

#include "PresetState.hpp"

#include "Renderer/StaticGlShaders.hpp"

#include <GLSLGenerator.h>
#include <HLSLParser.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>

#include <algorithm>
#include <regex>
#include <set>

static auto floatRand = []() { return static_cast<float>(rand() % 7381) / 7380.0f; };

MilkdropShader::MilkdropShader(ShaderType type)
    : m_type(type)
    , m_randValues({floatRand(), floatRand(), floatRand(), floatRand()})
{
    unsigned int index = 0;
    do
    {
        for (int i = 0; i < 4; i++)
        {
            float m_randTranslation_mult = 1;
            float rot_mult = 0.9f * powf(index / 8.0f, 3.2f);
            m_randTranslation[index].x = (floatRand() * 2 - 1) * m_randTranslation_mult;
            m_randTranslation[index].y = (floatRand() * 2 - 1) * m_randTranslation_mult;
            m_randTranslation[index].z = (floatRand() * 2 - 1) * m_randTranslation_mult;
            m_randRotationCenters[index].x = floatRand() * 6.28f;
            m_randRotationCenters[index].y = floatRand() * 6.28f;
            m_randRotationCenters[index].z = floatRand() * 6.28f;
            m_randRotationSpeeds[index].x = (floatRand() * 2 - 1) * rot_mult;
            m_randRotationSpeeds[index].y = (floatRand() * 2 - 1) * rot_mult;
            m_randRotationSpeeds[index].z = (floatRand() * 2 - 1) * rot_mult;
            index++;
        }
    } while (index < sizeof(m_randTranslation) / sizeof(m_randTranslation[0]));
}

void MilkdropShader::LoadCode(std::string presetShaderCode)
{
    m_presetShaderCode = std::move(presetShaderCode);

    std::string program = m_presetShaderCode;
    GetReferencedSamplers(program);
    PreprocessPresetShader(program);
    TranspileHLSLShader(program);
}

void MilkdropShader::LoadTextures(TextureManager& textureManager)
{
    // Add Milkdrop built-in textures
    m_shader.SetUniformTexture("main", textureManager.getTexture("main", GL_REPEAT, GL_LINEAR));
    m_shader.SetUniformTexture("main", textureManager.getTexture("main", GL_REPEAT, GL_LINEAR));
    m_shader.SetUniformTexture("fc_main", textureManager.getTexture("main", GL_CLAMP_TO_EDGE, GL_LINEAR));
    m_shader.SetUniformTexture("pc_main", textureManager.getTexture("main", GL_CLAMP_TO_EDGE, GL_NEAREST));
    m_shader.SetUniformTexture("fw_main", textureManager.getTexture("main", GL_REPEAT, GL_LINEAR));
    m_shader.SetUniformTexture("pw_main", textureManager.getTexture("main", GL_REPEAT, GL_NEAREST));

    m_shader.SetUniformTexture("noise_lq", textureManager.getTexture("noise_lq", GL_REPEAT, GL_LINEAR));
    m_shader.SetUniformTexture("noise_lq_lite", textureManager.getTexture("noise_lq_lite", GL_REPEAT, GL_LINEAR));
    m_shader.SetUniformTexture("noise_mq", textureManager.getTexture("noise_mq", GL_REPEAT, GL_LINEAR));
    m_shader.SetUniformTexture("noise_hq", textureManager.getTexture("noise_hq", GL_REPEAT, GL_LINEAR));
    m_shader.SetUniformTexture("noisevol_lq", textureManager.getTexture("noisevol_lq", GL_REPEAT, GL_LINEAR));
    m_shader.SetUniformTexture("noisevol_hq", textureManager.getTexture("noisevol_hq", GL_REPEAT, GL_LINEAR));
}

void MilkdropShader::LoadVariables(const PresetState& presetState)
{
    // These are the inputs: http://www.geisswerks.com/milkdrop/milkdrop_preset_authoring.html#3f6

    auto floatTime = static_cast<float>(presetState.presetTime);
    auto timeSincePresetStartWrapped = floatTime - static_cast<int>(floatTime / 10000.0) * 10000;
    auto mipX = logf(static_cast<float>(presetState.viewportWidth)) / logf(2.0f);
    auto mipY = logf(static_cast<float>(presetState.viewportWidth)) / logf(2.0f);
    auto mipAvg = 0.5f * (mipX + mipY);

    m_shader.SetUniformFloat4("rand_frame", {(rand() % 100) * .01,
                                             (rand() % 100) * .01,
                                             (rand() % 100) * .01,
                                             (rand() % 100) * .01});
    m_shader.SetUniformFloat4("rand_preset", {m_randValues[0],
                                              m_randValues[1],
                                              m_randValues[2],
                                              m_randValues[3]});

    m_shader.SetUniformFloat4("_c0", {presetState.aspectX,
                                      presetState.aspectY,
                                      1.0f / presetState.aspectX,
                                      1.0f / presetState.aspectY});
    m_shader.SetUniformFloat4("_c1", {0.0,
                                      0.0,
                                      0.0,
                                      0.0});
    m_shader.SetUniformFloat4("_c2", {timeSincePresetStartWrapped,
                                      presetState.fps,
                                      presetState.frame,
                                      presetState.progress});
    m_shader.SetUniformFloat4("_c3", {presetState.audioData.bass / 100,
                                      presetState.audioData.mid / 100,
                                      presetState.audioData.treb / 100,
                                      presetState.audioData.vol / 100});
    m_shader.SetUniformFloat4("_c4", {presetState.audioData.bassAtt / 100,
                                      presetState.audioData.midAtt / 100,
                                      presetState.audioData.trebAtt / 100,
                                      presetState.audioData.volAtt / 100});
    m_shader.SetUniformFloat4("_c5", {presetState.blur1Max - presetState.blur1Min,
                                      presetState.blur1Min,
                                      presetState.blur2Max - presetState.blur2Min,
                                      presetState.blur2Min});
    m_shader.SetUniformFloat4("_c6", {presetState.blur3Max - presetState.blur3Min,
                                      presetState.blur3Min,
                                      presetState.blur1Min,
                                      presetState.blur1Max});
    m_shader.SetUniformFloat4("_c7", {presetState.viewportWidth,
                                      presetState.viewportHeight,
                                      1 / static_cast<float>(presetState.viewportWidth),
                                      1 / static_cast<float>(presetState.viewportHeight)});

    m_shader.SetUniformFloat4("_c8", {0.5f + 0.5f * cosf(floatTime * 0.329f + 1.2f),
                                      0.5f + 0.5f * cosf(floatTime * 1.293f + 3.9f),
                                      0.5f + 0.5f * cosf(floatTime * 5.070f + 2.5f),
                                      0.5f + 0.5f * cosf(floatTime * 20.051f + 5.4f)});

    m_shader.SetUniformFloat4("_c9", {0.5f + 0.5f * sinf(floatTime * 0.329f + 1.2f),
                                      0.5f + 0.5f * sinf(floatTime * 1.293f + 3.9f),
                                      0.5f + 0.5f * sinf(floatTime * 5.070f + 2.5f),
                                      0.5f + 0.5f * sinf(floatTime * 20.051f + 5.4f)});

    m_shader.SetUniformFloat4("_c10", {0.5f + 0.5f * cosf(floatTime * 0.0050f + 2.7f),
                                       0.5f + 0.5f * cosf(floatTime * 0.0085f + 5.3f),
                                       0.5f + 0.5f * cosf(floatTime * 0.0133f + 4.5f),
                                       0.5f + 0.5f * cosf(floatTime * 0.0217f + 3.8f)});

    m_shader.SetUniformFloat4("_c11", {0.5f + 0.5f * sinf(floatTime * 0.0050f + 2.7f),
                                       0.5f + 0.5f * sinf(floatTime * 0.0085f + 5.3f),
                                       0.5f + 0.5f * sinf(floatTime * 0.0133f + 4.5f),
                                       0.5f + 0.5f * sinf(floatTime * 0.0217f + 3.8f)});

    m_shader.SetUniformFloat4("_c12", {mipX,
                                       mipY,
                                       mipAvg,
                                       0});
    m_shader.SetUniformFloat4("_c13", {presetState.blur2Min,
                                       presetState.blur2Max,
                                       presetState.blur3Min,
                                       presetState.blur3Max});


    std::array<glm::mat4, 24> tempMatrices{};

    // write matrices
    for (int i = 0; i < 20; i++)
    {
        glm::mat4 const rotationX = glm::rotate(glm::mat4(1.0f), m_randRotationCenters[i].x + m_randRotationSpeeds[i].x * floatTime, glm::vec3(1.0f, 0.0f, 0.0f));
        glm::mat4 const rotationY = glm::rotate(glm::mat4(1.0f), m_randRotationCenters[i].y + m_randRotationSpeeds[i].y * floatTime, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 const rotationZ = glm::rotate(glm::mat4(1.0f), m_randRotationCenters[i].z + m_randRotationSpeeds[i].z * floatTime, glm::vec3(0.0f, 0.0f, 1.0f));

        glm::mat4 const randomTranslation = glm::translate(glm::mat4(1.0f), glm::vec3(m_randTranslation[i].x, m_randTranslation[i].y, m_randTranslation[i].z));

        tempMatrices[i] = randomTranslation * rotationX;
        tempMatrices[i] = rotationZ * tempMatrices[i];
        tempMatrices[i] = rotationY * tempMatrices[i];
    }

    // the last 4 are totally random, each frame
    for (int i = 20; i < 24; i++)
    {
        glm::mat4 const rotationX = glm::rotate(glm::mat4(1.0f), floatRand() * 6.28f, glm::vec3(1.0f, 0.0f, 0.0f));
        glm::mat4 const rotationY = glm::rotate(glm::mat4(1.0f), floatRand() * 6.28f, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 const rotationZ = glm::rotate(glm::mat4(1.0f), floatRand() * 6.28f, glm::vec3(0.0f, 0.0f, 1.0f));

        glm::mat4 const randomTranslation = glm::translate(glm::mat4(1.0f), glm::vec3(floatRand(), floatRand(), floatRand()));

        tempMatrices[i] = randomTranslation * rotationX;
        tempMatrices[i] = rotationZ * tempMatrices[i];
        tempMatrices[i] = rotationY * tempMatrices[i];
    }

    m_shader.SetUniformMat3x4("rot_s1", tempMatrices[0]);
    m_shader.SetUniformMat3x4("rot_s2", tempMatrices[1]);
    m_shader.SetUniformMat3x4("rot_s3", tempMatrices[2]);
    m_shader.SetUniformMat3x4("rot_s4", tempMatrices[3]);
    m_shader.SetUniformMat3x4("rot_d1", tempMatrices[4]);
    m_shader.SetUniformMat3x4("rot_d2", tempMatrices[5]);
    m_shader.SetUniformMat3x4("rot_d3", tempMatrices[6]);
    m_shader.SetUniformMat3x4("rot_d4", tempMatrices[7]);
    m_shader.SetUniformMat3x4("rot_f1", tempMatrices[8]);
    m_shader.SetUniformMat3x4("rot_f2", tempMatrices[9]);
    m_shader.SetUniformMat3x4("rot_f3", tempMatrices[10]);
    m_shader.SetUniformMat3x4("rot_f4", tempMatrices[11]);
    m_shader.SetUniformMat3x4("rot_vf1", tempMatrices[12]);
    m_shader.SetUniformMat3x4("rot_vf2", tempMatrices[13]);
    m_shader.SetUniformMat3x4("rot_vf3", tempMatrices[14]);
    m_shader.SetUniformMat3x4("rot_vf4", tempMatrices[15]);
    m_shader.SetUniformMat3x4("rot_uf1", tempMatrices[16]);
    m_shader.SetUniformMat3x4("rot_uf2", tempMatrices[17]);
    m_shader.SetUniformMat3x4("rot_uf3", tempMatrices[18]);
    m_shader.SetUniformMat3x4("rot_uf4", tempMatrices[19]);
    m_shader.SetUniformMat3x4("rot_rand1", tempMatrices[20]);
    m_shader.SetUniformMat3x4("rot_rand2", tempMatrices[21]);
    m_shader.SetUniformMat3x4("rot_rand3", tempMatrices[22]);
    m_shader.SetUniformMat3x4("rot_rand4", tempMatrices[23]);

    // set program uniform "_q[a-h]" values (_qa.x, _qa.y, _qa.z, _qa.w, _qb.x, _qb.y ... ) alias q[1-32]
    for (int i = 0; i < QVarCount; i += 4)
    {
        std::string varName = "_q";
        varName.push_back('a' + i / 4);
        m_shader.SetUniformFloat4(varName.c_str(), {presetState.frameQVariables[i],
                                                    presetState.frameQVariables[i + 1],
                                                    presetState.frameQVariables[i + 2],
                                                    presetState.frameQVariables[i + 3]});
    }
}

void MilkdropShader::PreprocessPresetShader(std::string& program)
{

    if (program.length() <= 0)
    {
        throw ShaderException("Preset shader is declared, but empty.");
    }

    size_t found;

    // replace shader_body with entry point function
    found = program.find("shader_body");
    if (found != std::string::npos)
    {
#ifdef MILKDROP_PRESET_DEBUG
        std::cerr << "[MilkdropShader] First 'shader_body' found at: " << int(found) << std::endl;
#endif

        if (m_type == ShaderType::WarpShader)
        {
            program.replace(int(found), 11, "void PS(float4 _vDiffuse : COLOR, float4 _uv : TEXCOORD0, float2 _rad_ang : TEXCOORD1, out float4 _return_value : COLOR)\n");
        }
        else
        {
            program.replace(int(found), 11, "void PS(float4 _vDiffuse : COLOR, float2 _uv : TEXCOORD0, float2 _rad_ang : TEXCOORD1, out float4 _return_value : COLOR)\n");
        }
    }
    else
    {
        throw ShaderException("Preset shader is missing \"shader_body\" entry point.");
    }

    // replace the "{" immediately following shader_body with some variable declarations
    found = program.find('{', found);
    if (found != std::string::npos)
    {
#ifdef MILKDROP_PRESET_DEBUG
        std::cerr << "[MilkdropShader] First '{' found at: " << int(found) << std::endl;
#endif
        const char* progMain =
            "{\n"
            "float3 ret = 0;\n";
        program.replace(int(found), 1, progMain);
    }
    else
    {
        throw ShaderException("Preset shader has no opening braces.");
    }

    // replace "}" with return statement (this can probably be optimized for the GLSL conversion...)
    found = program.rfind('}');
    if (found != std::string::npos)
    {
#ifdef MILKDROP_PRESET_DEBUG
        std::cerr << "[MilkdropShader] Last '}' found at: " << int(found) << std::endl;
#endif
        program.replace(int(found), 1, "_return_value = float4(ret.xyz, 1.0);\n"
                                       "}\n");
    }
    else
    {
        throw ShaderException("Preset shader has no closing brace.");
    }

    // Find matching closing brace and cut off excess text after shader's main function
    int bracesOpen = 1;
    size_t pos = found + 1;
    for (; pos < program.length() && bracesOpen > 0; ++pos)
    {
        switch (program.at(pos))
        {
            case '/':
                // Skip comments until EoL to prevent false counting
                if (pos < program.length() - 1 && program.at(pos + 1) == '/')
                {
                    for (; pos < program.length(); ++pos)
                    {
                        if (program.at(pos) == '\n')
                        {
                            break;
                        }
                    }
                }
                continue;

            case '{':
                bracesOpen++;
                continue;

            case '}':
                bracesOpen--;
        }
    }

    if (pos < program.length() - 1)
    {
        program.resize(pos);
    }

    std::string fullSource; //!< Full shader source before translation, includes all uniforms etc.

    // First copy the generic "header" into the shader. Includes uniforms and some defines
    // to unwrap the packed 4-element uniforms into single values.
    fullSource.append(StaticGlShaders::Get()->GetPresetShaderHeader());

    if (m_type == ShaderType::WarpShader)
    {
        fullSource.append("#define rad _rad_ang.x\n"
                          "#define ang _rad_ang.y\n"
                          "#define uv _uv.xy\n"
                          "#define uv_orig _uv.zw\n");
    }
    else
    {
        fullSource.append("#define rad _rad_ang.x\n"
                          "#define ang _rad_ang.y\n"
                          "#define uv _uv.xy\n"
                          "#define uv_orig _uv.xy\n"
                          "#define hue_shader _vDiffuse.xyz\n");
    }

    fullSource.append(program);

    program = fullSource;
}

void MilkdropShader::GetReferencedSamplers(const std::string& program)
{
    // set up texture samplers for all samplers references in the shader program
    auto found = program.find("sampler_", 0);
    while (found != std::string::npos)
    {
        found += 8;
        size_t const end = program.find_first_of(" ;,\n\r)", found);

        if (end != std::string::npos)
        {
            std::string const sampler = program.substr(static_cast<int>(found), static_cast<int>(end - found));
            std::string lowerCaseName(sampler);
            std::transform(lowerCaseName.begin(), lowerCaseName.end(), lowerCaseName.begin(), tolower);

            m_samplerNames.push_back(lowerCaseName);
        }

        found = program.find("sampler_", found);
    }

    if (program.find("GetBlur3") != std::string::npos)
    {
        m_maxBlurLevelRequired = BlurTexture::BlurLevel::Blur3;
        m_samplerNames.emplace_back("blur1");
        m_samplerNames.emplace_back("blur2");
        m_samplerNames.emplace_back("blur3");
    }
    else if (program.find("GetBlur2") != std::string::npos)
    {
        m_maxBlurLevelRequired = BlurTexture::BlurLevel::Blur2;
        m_samplerNames.emplace_back("blur1");
        m_samplerNames.emplace_back("blur2");
    }
    else if (program.find("GetBlur1") != std::string::npos)
    {
        m_maxBlurLevelRequired = BlurTexture::BlurLevel::Blur1;
        m_samplerNames.emplace_back("blur1");
    }
    else
    {
        m_maxBlurLevelRequired = BlurTexture::BlurLevel::None;
    }
}

void MilkdropShader::TranspileHLSLShader(std::string& program)
{
    std::string shaderTypeString = "composite";
    if (m_type == ShaderType::WarpShader)
    {
        shaderTypeString = "warp";
    }

    M4::GLSLGenerator generator;
    M4::Allocator allocator;

    M4::HLSLTree tree(&allocator);
    M4::HLSLParser parser(&allocator, &tree);

    // Preprocess define macros
    std::string sourcePreprocessed;
    if (!parser.ApplyPreprocessor("", program.c_str(), program.size(), sourcePreprocessed))
    {
        throw ShaderException("Error translating HLSL " + shaderTypeString + " shader: Preprocessing failed.\nSource:\n" + program);
    }

    // Remove previous shader declarations
    std::smatch matches;
    while (std::regex_search(sourcePreprocessed, matches, std::regex("sampler(2D|3D|)(\\s+|\\().*")))
    {
        sourcePreprocessed.replace(matches.position(), matches.length(), "");
    }

    // Remove previous texsize declarations
    while (std::regex_search(sourcePreprocessed, matches, std::regex("float4\\s+texsize_.*")))
    {
        sourcePreprocessed.replace(matches.position(), matches.length(), "");
    }

    // Declare samplers
    std::set<std::string> textureSizes;
    for (const auto& sampler : m_shader.GetTextures())
    {
        Texture* texture = sampler.second.first;

        if (texture->type == GL_TEXTURE_3D)
        {
            sourcePreprocessed.insert(0, "uniform sampler3D sampler_" + sampler.first + ";\n");
        }
        else
        {
            sourcePreprocessed.insert(0, "uniform sampler2D sampler_" + sampler.first + ";\n");
        }

        textureSizes.insert(sampler.first);
        textureSizes.insert(texture->name);
    }

    // Declare texsizes
    for (const auto& textureSize : textureSizes)
    {
        sourcePreprocessed.insert(0, "uniform float4 texsize_" + textureSize + ";\n");
    }


    // Transpile from HLSL (aka preset shader aka DirectX shader) to GLSL (aka OpenGL shader lang)
    // First, parse HLSL into a tree
    if (!parser.Parse("", sourcePreprocessed.c_str(), sourcePreprocessed.size()))
    {
        throw ShaderException("Error translating HLSL " + shaderTypeString + " shader: HLSL parsing failed.\nSource:\n" + sourcePreprocessed);
    }

    // Then generate GLSL from the resulting parser tree
    if (!generator.Generate(&tree, M4::GLSLGenerator::Target_FragmentShader,
                            StaticGlShaders::Get()->GetGlslGeneratorVersion(),
                            "PS"))
    {
        throw ShaderException("Error translating HLSL " + shaderTypeString + " shader: GLSL generating failes.\nSource:\n" + sourcePreprocessed);
    }

    // Now we have GLSL source for the preset shader program (hopefully it's valid!)
    // Compile the preset shader fragment shader with the standard vertex shader and cross our fingers.
    if (m_type == ShaderType::WarpShader)
    {
        m_shader.CompileProgram(StaticGlShaders::Get()->GetPresetWarpVertexShader(), generator.GetResult());
    }
    else
    {
        m_shader.CompileProgram(StaticGlShaders::Get()->GetPresetCompVertexShader(), generator.GetResult());
    }
}
