#include "MilkdropShader.hpp"

#include <algorithm>

MilkdropShader::MilkdropShader(ShaderType type)
    : m_type(type)
{
}


void MilkdropShader::LoadCode(std::string presetShaderCode)
{
    m_presetShaderCode = std::move(presetShaderCode);

    std::string program = m_presetShaderCode;
    PreprocessPresetShader(program);

}

void MilkdropShader::LoadTextures(TextureManager& textureManager)
{
    // Add Milkdrop built-in textures
    m_shader.SetUniformTexture("main", textureManager.getTexture("main", GL_REPEAT, GL_LINEAR));
    m_shader.SetUniformTexture("main", textureManager->getTexture("main", GL_REPEAT, GL_LINEAR));
    m_shader.SetUniformTexture("fc_main", textureManager->getTexture("main", GL_CLAMP_TO_EDGE, GL_LINEAR));
    m_shader.SetUniformTexture("pc_main", textureManager->getTexture("main", GL_CLAMP_TO_EDGE, GL_NEAREST));
    m_shader.SetUniformTexture("fw_main", textureManager->getTexture("main", GL_REPEAT, GL_LINEAR));
    m_shader.SetUniformTexture("pw_main", textureManager->getTexture("main", GL_REPEAT, GL_NEAREST));

    m_shader.SetUniformTexture("noise_lq", textureManager->getTexture("noise_lq", GL_REPEAT, GL_LINEAR));
    m_shader.SetUniformTexture("noise_lq_lite", textureManager->getTexture("noise_lq_lite", GL_REPEAT, GL_LINEAR));
    m_shader.SetUniformTexture("noise_mq", textureManager->getTexture("noise_mq", GL_REPEAT, GL_LINEAR));
    m_shader.SetUniformTexture("noise_hq",  textureManager->getTexture("noise_hq", GL_REPEAT, GL_LINEAR));
    m_shader.SetUniformTexture("noisevol_lq", textureManager->getTexture("noisevol_lq", GL_REPEAT, GL_LINEAR));
    m_shader.SetUniformTexture("noisevol_hq", textureManager->getTexture("noisevol_hq", GL_REPEAT, GL_LINEAR));

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

}

void MilkdropShader::GetReferencedSamplers(const std::string& program)
{
    // set up texture samplers for all samplers references in the shader program
    auto found = program.find("sampler_", 0);
    while (found != std::string::npos)
    {
        found += 8;
        size_t end = program.find_first_of(" ;,\n\r)", found);

        if (end != std::string::npos)
        {
            std::string sampler = program.substr((int) found, (int) end - found);
            std::string lowerCaseName(sampler);
            std::transform(lowerCaseName.begin(), lowerCaseName.end(), lowerCaseName.begin(), tolower);

            m_samplerNames.push_back(lowerCaseName);
        }

        found = program.find("sampler_", found);
    }


    found = program.find("GetBlur3");

}
