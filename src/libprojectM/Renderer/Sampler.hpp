
#pragma once

#include <projectM-opengl.h>

#include <memory>

namespace libprojectM {
namespace Renderer {


class Sampler
{
public:
    using Ptr = std::shared_ptr<Sampler>;

    Sampler() = delete;
    Sampler(const Sampler&) = delete;
    auto operator=(const Sampler&) -> Sampler& = delete;


    Sampler(GLint wrapMode, GLint filterMode);

    Sampler(Sampler&& other) = default;
    auto operator=(Sampler&& other) -> Sampler& = default;

    ~Sampler();


    void Bind(GLuint unit) const;


    static void Unbind(GLuint unit);


    auto WrapMode() const -> GLint;


    void WrapMode(GLint wrapMode);


    auto FilterMode() const -> GLint;


    void FilterMode(GLint filterMode);

private:
    GLuint m_samplerId{0};
    GLint m_wrapMode{0};
    GLint m_filterMode{0};
};

}
}
