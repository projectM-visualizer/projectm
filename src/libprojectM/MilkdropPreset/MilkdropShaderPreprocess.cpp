#include "MilkdropShaderPreprocess.hpp"

#include "Utils.hpp"

#include <MilkdropStaticShaders.hpp>

#include <Logging.hpp>
#include <Renderer/Shader.hpp>

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <string>

namespace libprojectM {
namespace MilkdropPreset {

using libprojectM::MilkdropPreset::MilkdropStaticShaders;

std::string AssembleApplyPreprocessorInput(MilkdropShader::ShaderType type,
                                           const std::string& presetShaderBody)
{
    // Byte-for-byte extraction of the pure-text assembly that used to live inline in
    // MilkdropShader::PreprocessPresetShader. Do NOT alter any transformation, ordering,
    // whitespace, header or #define content — external tooling and the runtime rely on
    // producing identical bytes.
    std::string program = presetShaderBody;

    std::string shaderTypeString = "composite";
    if (type == MilkdropShader::ShaderType::WarpShader)
    {
        shaderTypeString = "warp";
    }

    if (program.length() <= 0)
    {
        throw Renderer::ShaderException("[MilkdropShader] Preset " + shaderTypeString + " shader is declared, but empty.");
    }

    size_t found;

    // Find "sampler_state" overrides and remove them first, as they're not supported by GLSL.
    // The logic isn't totally fool-proof, but should work in general.
    // Use a comment-stripped copy for searching so commented-out sampler_state blocks are skipped.
    // StripComments preserves string length, so positions map 1:1 to the original.
    std::string stripped = Utils::StripComments(program);
    found = stripped.find("sampler_state");
    while (found != std::string::npos)
    {
        // Now go backwards and find the assignment
        found = stripped.rfind('=', found);
        auto startPos = found;

        // Find closing brace and semicolon
        found = stripped.find('}', found);
        found = stripped.find(';', found);

        if (found != std::string::npos)
        {
            stripped.replace(startPos, found - startPos, "");
        }
        else
        {
            // No closing brace and semicolon.
            break;
        }

        found = stripped.find("sampler_state");
    }

    // replace shader_body with entry point function
    // Use the stripped copy so a commented-out shader_body is not matched.
    found = stripped.find("shader_body");
    if (found != std::string::npos)
    {
        if (type == MilkdropShader::ShaderType::WarpShader)
        {
            program.replace(int(found), 11, R"(
void PS(float4 _vDiffuse : COLOR,
        float4 _uv : TEXCOORD0,
        float2 _rad_ang : TEXCOORD1,
        out float4 _return_value : COLOR0,
        out float4 _mv_tex_coords : COLOR1)
)");
        }
        else
        {
            program.replace(int(found), 11, R"(
void PS(float4 _vDiffuse : COLOR,
        float2 _uv : TEXCOORD0,
        float2 _rad_ang : TEXCOORD1,
        out float4 _return_value : COLOR)
)");
        }
    }
    else
    {
        LOG_DEBUG("[MilkdropShader] Failed " + shaderTypeString + " shader code:\n" + program);
        throw Renderer::ShaderException("[MilkdropShader] Preset " + shaderTypeString + " shader is missing \"shader_body\" entry point.");
    }

    // replace the "{" immediately following shader_body with some variable declarations
    found = program.find('{', found);
    if (found != std::string::npos)
    {
        std::string progMain = "{\nfloat3 ret = 0;\n";
        if (type == MilkdropShader::ShaderType::WarpShader)
        {
            progMain.append("_mv_tex_coords.xy = _uv.xy;\n");
        }
        program.replace(int(found), 1, progMain);
    }
    else
    {
        LOG_DEBUG("[MilkdropShader] Failed " + shaderTypeString + " shader code:\n" + program);
        throw Renderer::ShaderException("[MilkdropShader] Preset " + shaderTypeString + " shader has no opening braces.");
    }

    // replace "}" with return statement (this can probably be optimized for the GLSL conversion...)
    found = program.rfind('}');
    if (found != std::string::npos)
    {
        program.replace(int(found), 1, "_return_value = float4(ret.xyz, 1.0);\n"
                                       "}\n");
    }
    else
    {
        LOG_DEBUG("[MilkdropShader] Failed " + shaderTypeString + " shader code:\n" + program);
        throw Renderer::ShaderException("[MilkdropShader] Preset " + shaderTypeString + " shader has no closing brace.");
    }

    // Find matching closing brace and cut off excess text after shader's main function
    int bracesOpen = 1;
    size_t pos = found + 1;
    for (; pos < program.length() && bracesOpen > 0; ++pos)
    {
        switch (program.at(pos))
        {
            case '/':
                // Skip line comments until EoL to prevent false counting
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
                // Skip block comments to prevent false counting
                else if (pos < program.length() - 1 && program.at(pos + 1) == '*')
                {
                    pos += 2;
                    for (; pos < program.length() - 1; ++pos)
                    {
                        if (program.at(pos) == '*' && program.at(pos + 1) == '/')
                        {
                            ++pos; // skip past '/'
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
    fullSource.append(MilkdropStaticShaders::Get()->GetPresetShaderHeader());

    if (type == MilkdropShader::ShaderType::WarpShader)
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

    return fullSource;
}

namespace {

// Compile-time salt. Bump this string whenever anything that changes the produced GLSL for
// a given input changes: the static preset shader header, the hlslparser/ApplyPreprocessor
// implementation, or AssembleApplyPreprocessorInput above. Bumping invalidates every stale
// cache entry (old keys can no longer be produced).
constexpr const char* kPreprocessCacheSalt = "pmpp-v1:";

uint64_t Fnv1a64(const std::string& data, uint64_t offsetBasis)
{
    constexpr uint64_t prime = 1099511628211ull;
    uint64_t hash = offsetBasis;
    for (const char c : data)
    {
        hash ^= static_cast<uint64_t>(static_cast<unsigned char>(c));
        hash *= prime;
    }
    return hash;
}

} // namespace

std::string ComputePreprocessCacheKey(const std::string& applyPreprocessorInput)
{
    // Two independent FNV-1a passes with different offset bases (the second is the standard
    // basis byte-reversed) give ~128 bits of effective hash; the length is embedded too.
    const uint64_t h1 = Fnv1a64(applyPreprocessorInput, 14695981039346656037ull);
    const uint64_t h2 = Fnv1a64(applyPreprocessorInput, 0x84222325cbf29ce4ull);

    char buffer[64];
    std::snprintf(buffer, sizeof(buffer), "%016llx%016llx-%zx",
                  static_cast<unsigned long long>(h1),
                  static_cast<unsigned long long>(h2),
                  applyPreprocessorInput.size());

    return std::string(kPreprocessCacheSalt) + buffer;
}

std::string PreprocessCacheSalt()
{
    return kPreprocessCacheSalt;
}

} // namespace MilkdropPreset
} // namespace libprojectM
