#ifdef USE_TEXT_MENU

#include "MenuText.h"

#include "Common.hpp"

#define GLT_IMPLEMENTATION
#include "gltext.h"

MenuText::MenuText(int viewportWidth)
    : _viewportWidth(viewportWidth)
{
    gltInit();
    _glTextInstance = gltCreateText();
}

MenuText::~MenuText()
{
    if (_glTextInstance)
    {
        gltDeleteText(_glTextInstance);
        _glTextInstance = nullptr;
    }
}

void MenuText::SetViewportWidth(int viewportWidth)
{
    _viewportWidth = viewportWidth;
}

void MenuText::CleanUp()
{
    gltTerminate();
}

void MenuText::DrawBegin() const
{
    // Begin text drawing (this for instance calls glUseProgram)
    gltBeginDraw();
}

void MenuText::DrawEnd() const
{
    // Finish drawing text - will unbind font texture & shaders.
    gltEndDraw();
}

void MenuText::Draw(std::string textLine,
                    GLfloat x,
                    GLfloat y,
                    GLfloat scale,
                    HorizontalAlignment horizontalAlignment,
                    VerticalAlignment verticalAlignment,
                    float r,
                    float b,
                    float g,
                    float a,
                    bool highlightable,
                    const std::string& highlightText) const
{
    if (!gltInitialized || !_glTextInstance)
    {
        return;
    }

    int gltHorizontalAlignment = static_cast<int>(horizontalAlignment);
    int gltVerticalAlignment = static_cast<int>(verticalAlignment);

    gltSetText(_glTextInstance, textLine.c_str());
    GLfloat textWidth = gltGetTextWidth(_glTextInstance, scale);

    auto windowWidth = static_cast<float>(_viewportWidth);
    if (gltHorizontalAlignment == GLT_LEFT)
    {
        // if left aligned factor in X offset
        windowWidth -= x;
    }

    // if our window is greater than the text width, there is no overflow so let's display it normally.
    if (windowWidth < textWidth)
    {
        // if the text is greater than the window width, we have a problem.
        while (textWidth > windowWidth)
        {
            textLine.pop_back();
            gltSetText(_glTextInstance, textLine.c_str());

            gltDrawText2DAligned(_glTextInstance, x, y, scale, gltHorizontalAlignment, gltVerticalAlignment);
            textWidth = gltGetTextWidth(_glTextInstance, scale);
        }

        // if it's not multi-line then append a ...
        if (textLine.find('\n') != std::string::npos)
        {
           textLine.pop_back();
           textLine.pop_back();
           textLine.pop_back();
           textLine += "...";
        }
    }

    // redraw without transparency
    if (highlightable && highlightText.length() > 1)
    {
        HighlightNeedle(textLine, highlightText, x, y, scale, gltHorizontalAlignment, gltVerticalAlignment, r, g, b, a);
    }
    else
    {
        gltColor(r, g, b, a);
        gltSetText(_glTextInstance, textLine.c_str());
        gltDrawText2DAligned(_glTextInstance, x, y, scale, gltHorizontalAlignment, gltVerticalAlignment);
    }
}

void MenuText::HighlightNeedle(const std::string& textLine,
                               const std::string& highlightText,
                               GLfloat x,
                               GLfloat y,
                               GLfloat scale,
                               int horizontalAlignment = GLT_LEFT,
                               int verticalAlignment = GLT_TOP,
                               float r = 1.0f,
                               float b = 1.0f,
                               float g = 1.0f,
                               float a = 1.0f) const
{

    size_t pos = caseInsensitiveSubstringFind(textLine, highlightText);

    gltColor(r, g, b, a);

    if (pos == std::string::npos)
    {
        // Search term not found (e.g. cropped due to window size), render whole line.
        gltSetText(_glTextInstance, textLine.c_str());
        gltDrawText2DAligned(_glTextInstance, x, y, scale, horizontalAlignment, verticalAlignment);

        return;
    }

    // Draw everything normal, up to search term.
    gltSetText(_glTextInstance, textLine.substr(0, pos).c_str());
    gltDrawText2DAligned(_glTextInstance, x, y, scale, horizontalAlignment, verticalAlignment);

    // Crop original text from "textLine" to retain casing.
    std::string originalHighlightText = textLine.substr(pos, highlightText.length());

    // highlight search term
    GLfloat textWidth = gltGetTextWidth(_glTextInstance, scale);
    GLfloat offset = x + textWidth;
    gltColor(1.0f, 0.0f, 1.0f, a);
    gltSetText(_glTextInstance, originalHighlightText.c_str());
    gltDrawText2DAligned(_glTextInstance, offset, y, scale, horizontalAlignment, verticalAlignment);

    // draw rest of name, normally
    textWidth = gltGetTextWidth(_glTextInstance, scale);
    offset = offset + textWidth;
    gltColor(r, g, b, a);
    gltSetText(_glTextInstance, textLine.substr(pos + originalHighlightText.length(), textLine.length()).c_str());
    gltDrawText2DAligned(_glTextInstance, offset, y, scale, horizontalAlignment, verticalAlignment);
}

#endif