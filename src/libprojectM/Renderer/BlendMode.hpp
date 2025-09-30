#pragma once

#include <projectM-opengl.h>

namespace libprojectM {
namespace Renderer {

/**
 * A simple wrapper class around OpenGL's blend mode functionality.
 */
class BlendMode
{
public:
    /**
     * Available blend functions.
     * @see https://registry.khronos.org/OpenGL-Refpages/gl4/html/glBlendFunc.xhtml
     */
    enum class Function : int
    {
        Zero,
        One,
        SourceColor,
        OneMinusSourceColor,
        DestinationColor,
        OneMinusDestinationColor,
        SourceAlpha,
        OneMinusSourceAlpha,
        DestinationAlpha,
        OneMinusDestinationAlpha,
        ConstantColor,
        OneMinusConstantColor,
        ConstantAlpha,
        OneMinusConstantAlpha,
        SourceAlphaSaturate,
        Source1Color,
        OneMinusSource1Color,
        Source1Alpha,
        OneMinusSource1Alpha
    };

    BlendMode() = delete;

    /**
     * @brief Enables or disables blending and sets the blend functions.
     * A convenience wrapper around BlendMode::SetBlendActive() and BlendMode::SetBlendFunction().
     * @param enable If true, blending is enabled, otherwise disabled.
     * @param srcFunc The blend function to determine the source color.
     * @param dstFunc the blend function to determine the destination color.
     */
    static void Set(bool enable, Function srcFunc, Function dstFunc);

    /**
     * Enables or disables blending.
     * @param enable If true, blending is enabled, otherwise disabled.
     */
    static void SetBlendActive(bool enable);

    /**
     * Sets the blend functions.
     * @param srcFunc The blend function to determine the source color.
     * @param dstFunc the blend function to determine the destination color.
     */
    static void SetBlendFunction(Function srcFunc, Function dstFunc);

private:
    /**
     * Translates the Function enum values into OpenGL constants.
     * @param func The blend function to translate.
     * @return the OpenGL constant for the given blend function.
     */
    static GLuint FunctionToGL(Function func);
};

} // namespace Renderer
} // namespace libprojectM
