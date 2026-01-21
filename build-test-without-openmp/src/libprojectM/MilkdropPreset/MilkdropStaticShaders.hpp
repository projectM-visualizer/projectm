#pragma once

#include <GLSLGenerator.h>

#include <Renderer/Shader.hpp>

#include <memory>

namespace libprojectM {
namespace MilkdropPreset {

/**
 * @brief Singleton manager for static Milkdrop GL shaders
 *
 * The manager provides shaders through its accessor methods, and selects the appropriate shader
 * version to be compatible with the system GLSL version.
 */
class MilkdropStaticShaders
{
public:
    /**
     * Returns the singleton MilkdropStaticShaders instance.
     * @return The singleton instance of the MilkdropStaticShaders class.
     */
    static std::shared_ptr<MilkdropStaticShaders> Get()
    {
        bool useGLES = false;
#ifdef USE_GLES
        useGLES = true;
#endif

        static std::shared_ptr<MilkdropStaticShaders> instance(
            new MilkdropStaticShaders(useGLES));
        return instance;
    }

    /**
     * Returns the GLSLGenerator version enum value corresponding to the queried OpenGL shader version.
     * @return The GLSLGenerator version enum value corresponding to the queried OpenGL shader version.
     */
    M4::GLSLGenerator::Version GetGlslGeneratorVersion()
    {
        return m_GLSLGeneratorVersion;
    }

    // Accessors for the named static GL shader resources.
    std::string GetBlur1FragmentShader();
    std::string GetBlur2FragmentShader();
    std::string GetBlurVertexShader();
    std::string GetPresetCompVertexShader();
    std::string GetPresetMotionVectorsVertexShader();
    std::string GetPresetShaderHeader();
    std::string GetPresetWarpFragmentShader();
    std::string GetPresetWarpVertexShader();
    std::string GetTexturedDrawFragmentShader();
    std::string GetTexturedDrawVertexShader();
    std::string GetUntexturedDrawFragmentShader();
    std::string GetUntexturedDrawVertexShader();


private:
    /**
     * @brief Constructor, overriding the version to GLES3 if `use_gles` is true.
     * Note - this happens after GlslVersion is called, because it uses the version to determine things.
     * @param use_gles Set to true if GLES is being used.
     */
    MilkdropStaticShaders(bool useGLES);

    /**
     * @brief Prepends a string of the form "#version <number>\n" to the provided shader text.
     * <number> is derived from the queried GLSL version (or overridden when the manager was
     * constructed with `use_gles` = true).
     * @param shader_text The shader text to prepend the header to.
     * @return A "full" shader with header prepended.
     */
    std::string AddVersionHeader(std::string shader_text);

    bool m_useGLES{false};                             //!< Whether or not to use GLES shaders.
    std::string m_versionHeader;                       //!< The version header to prepended by AddVersionHeader.
    M4::GLSLGenerator::Version m_GLSLGeneratorVersion; //!< The GLSL generator version to pass to the hlslparser generator.
};

} // namespace MilkdropPreset
} // namespace libprojectM
