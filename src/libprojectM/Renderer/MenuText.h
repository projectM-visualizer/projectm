#pragma once

#ifdef USE_TEXT_MENU

#include "projectM-opengl.h"

#include <string>

class GLTtext;

/**
 * @brief Menu text renderer
 *
 * <p>Uses the single-header glText library to render the menu overlay and toast messages.</p>
 *
 * <p>For proper cleanup, the CleanUp() method should be called once before shutdown or when resetting
 * the OpenGL renderer. It will destroy the texture atlas and shaders. It just calls gltTerminate().
 * The gltInit() function is called each time an instance of MenuText is created, assuring that everything
 * is properly initialized. This is only done once on start or after calling CleanUp().</p>
 */
class MenuText
{
public:
    /**
     * Constants for horizontal text alignment.
     */
    enum class HorizontalAlignment
    {
        Left = 0, //!< Render text aligned to the left
        Center = 1, //!< Render text aligned to the center
        Right = 2 //!< Render text aligned to the right
    };

    /**
     * Constants for vertical text alignment.
     */
    enum class VerticalAlignment
    {
        Top = 0, //!< Render text aligned to the top
        Center = 1, //!< Render text aligned to the center
        Bottom = 2//!< Render text aligned to the bottom
    };

    MenuText() = delete;

    /**
     * Constructor.
     * @param viewportWidth Sets the maximum viewport width.
     */
    explicit MenuText(int viewportWidth);
    virtual ~MenuText();

    /**
     * @brief Sets the maximum viewport width.
     * @param viewportWidth
     */
    void SetViewportWidth(int viewportWidth);

    /**
     * @brief Destroy the internal glText objects, e.g. shaders and the font texture.
     *
     * Only call this on shutdown or when resetting the OpenGL renderer and after all instances of MenuText
     * were destroyed. The next instance of MenuText will reinitialize glText.
     */
    static void CleanUp();

    /**
     * @brief Binds text drawing shaders and textures.
     *
     * Always needs to be called before the initial call to Draw().
     */
    void DrawBegin() const;

    /**
     * @brief Unbinds text drawing shaders and textures.
     *
     * Should be called after the last call to Draw().
     */
    void DrawEnd() const;

    /**
     * @brief Renders a line or block of text.
     *
     * <p>Text is cut off if wider than the viewport. Note this can cull more text than intended if
     * it consists of multiple lines, e.g. after a long line, no more text is rendered.</p>
     *
     * <p>Before calling Draw(), BeginDraw() must be called to set up the required OpenGL states. Draw()
     * can then be called any number of times. If text rendering is done, DrawEnd() should be called.</p>
     *
     * @param textLine The text that should be rendered.
     * @param x X offset of the text.
     * @param y Y offset of the text.
     * @param scale Font size.
     * @param horizontalAlignment Horizontal text alignment inside the viewport.
     * @param verticalAlignment Vertical text alignment inside the viewport.
     * @param r Red color value, 0.0 to 1.0.
     * @param b Blue color value, 0.0 to 1.0.
     * @param g Green color value, 0.0 to 1.0.
     * @param a Alpha value, 0.0 (transparent) to 1.0 (opaque).
     * @param highlightable If true, highlightText will be searched in textLine and highlighted in magenta in the output.
     * @param highlightText Text to be seaqrched and highlighted, case-insensitive.
     */
    void Draw(std::string textLine,
              GLfloat x,
              GLfloat y,
              GLfloat scale,
              HorizontalAlignment horizontalAlignment = HorizontalAlignment::Left,
              VerticalAlignment verticalAlignment = VerticalAlignment::Top,
              float r = 1.0f,
              float b  = 1.0f,
              float g  = 1.0f,
              float a  = 1.0f,
              bool highlightable = false,
              const std::string& highlightText = "") const;

protected:

    /**
     * @brief Searches for highlightText in textLine and renders the search text in a different color.
     *
     * If the text is not found, the line will be rendered normally. The highlighted text is rendered in magenta
     * with the same alpha value as the normal text.
     *
     * @param textLine The text that should be rendered.
     * @param highlightText The text that should be highlighted.
     * @param x X offset of the text.
     * @param y Y offset of the text.
     * @param scale Font size.
     * @param horizontalAlignment Horizontal text alignment inside the viewport.
     * @param verticalAlignment Vertical text alignment inside the viewport.
     * @param r Red color value, 0.0 to 1.0. Only used for the non-highlighted part of the text.
     * @param b Blue color value, 0.0 to 1.0. Only used for the non-highlighted part of the text.
     * @param g Green color value, 0.0 to 1.0. Only used for the non-highlighted part of the text.
     * @param a Alpha value, 0.0 (transparent) to 1.0 (opaque).
     */
    void HighlightNeedle(const std::string& textLine,
                         const std::string& highlightText,
                         GLfloat x,
                         GLfloat y,
                         GLfloat scale,
                         int horizontalAlignment,
                         int verticalAlignment,
                         float r,
                         float b,
                         float g,
                         float a) const;

    int _viewportWidth{ 0 }; //!< The viewport width in pixels that can be used.
    GLTtext* _glTextInstance{ nullptr }; //!< The glText instance pointer.
};

#endif