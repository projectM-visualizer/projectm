#include <memory>

#include "GLSLGenerator.h"

// Singleton manager for static resource GL shaders. The manager provides
// shaders through its accessor methods, and selects the appropriate shader
// version to be compatible with the system GLSL version.
class StaticGlShaders {
   public:
    // Returns the singleton StaticGlShaders instance.
    static std::shared_ptr<StaticGlShaders> Get() {
        bool use_gles = false;
#if USE_GLES
        use_gles = true;
#endif

        static std::shared_ptr<StaticGlShaders> instance(
            new StaticGlShaders(use_gles));
        return instance;
    }

    // Returns the GLSLGenerator version enum value corresponding to the queried
    // OpenGL shader version.
    M4::GLSLGenerator::Version GetGlslGeneratorVersion() {
        return glsl_generator_version_;
    }

    // Returns the named static GL shader resource.
    std::string GetPresetWarpVertexShader();
    std::string GetPresetCompVertexShader();
    std::string GetV2fC4fVertexShader();
    std::string GetV2fC4fFragmentShader();
    std::string GetV2fC4fT2fVertexShader();
    std::string GetV2fC4fT2fFragmentShader();
    std::string GetBlurVertexShader();
    std::string GetBlur1FragmentShader();
    std::string GetBlur2FragmentShader();
    std::string GetPresetShaderHeader();

   private:
    // POD struct to store parsed GLSL version numbers.
    struct GlslVersion {
        int major, minor;
    };

    // Queries the system GLSL version using
    // `glGetString(GL_SHADING_LANGUAGE_VERSION)` and returns the major and
    // minor numbers.
    GlslVersion QueryGlslVersion();

    // Constructs a StaticGlShaders, overriding the version to GLES3 if
    // `use_gles` is true.
    // Note - this happens after GlslVersion is called, because it uses the 
    // version to determine things.
    StaticGlShaders(bool use_gles);


    // Prepends a string of the form "#version <number>\n" to the provided
    // shader text, where <number> is derived from the queried GLSL version (or
    // overridden when the manager was constructed with `use_gles` = true).
    std::string AddVersionHeader(std::string shader_text);

    // Whether or not to use GLES shaders.
    bool use_gles_;

    // The queried GLSL version.
    GlslVersion version_;

    // The version header to prepended by AddVersionHeader.
    std::string version_header_;

    // The GLSL generator version to pass to the hlslparser generator.
    M4::GLSLGenerator::Version glsl_generator_version_;
};
