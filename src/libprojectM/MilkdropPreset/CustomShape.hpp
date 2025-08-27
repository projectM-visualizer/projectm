#pragma once

#include "Constants.hpp"
#include "PresetState.hpp"
#include "ShapePerFrameContext.hpp"

#include <Renderer/RenderItem.hpp>

#include <projectm-eval.h>

namespace libprojectM {
namespace MilkdropPreset {

class PresetFileParser;


class CustomShape : public Renderer::RenderItem
{
public:
    CustomShape(PresetState& presetState);

    ~CustomShape() override;

    void InitVertexAttrib() override;


    void Initialize(::libprojectM::PresetFileParser& parsedFile, int index);


    void CompileCodeAndRunInitExpressions();


    void Draw();

private:
    struct ShapeVertex {
        float x{.0f};
        float y{.0f};
    };

    std::string m_image;

    int m_index{0};
    bool m_enabled{false};
    int m_sides{4};
    bool m_additive{false};
    bool m_thickOutline{false};
    bool m_textured{false};
    int m_instances{1};

    float m_x{0.5f};
    float m_y{0.5f};
    float m_radius{0.1f};
    float m_angle{0.0f};

    float m_r{1.0f};
    float m_g{0.0f};
    float m_b{0.0f};
    float m_a{1.0f};

    float m_r2{0.0f};
    float m_g2{1.0f};
    float m_b2{0.0f};
    float m_a2{0.0f};

    float m_border_r{1.0f};
    float m_border_g{1.0f};
    float m_border_b{1.0f};
    float m_border_a{0.0f};

    float m_tex_ang{0.0f};
    float m_tex_zoom{1.0f};

    std::string m_initCode;
    std::string m_perFrameCode;

    PRJM_EVAL_F m_tValuesAfterInitCode[TVarCount]{};

    PresetState& m_presetState;
    ShapePerFrameContext m_perFrameContext;

    GLuint m_vboIdTextured{0};
    GLuint m_vaoIdTextured{0};

    GLuint m_vboIdUntextured{0};
    GLuint m_vaoIdUntextured{0};

    friend class ShapePerFrameContext;
};

}
}
