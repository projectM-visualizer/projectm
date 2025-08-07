#pragma once

#include "Renderer/VertexBufferUsage.hpp"

#include <projectM-opengl.h>
#include <vector>

namespace libprojectM {
namespace Renderer {

class VertexIndexArray
{
public:
    VertexIndexArray();

    virtual ~VertexIndexArray();

    void Bind() const;

    static void Unbind();

    auto Get() -> std::vector<uint32_t>&;

    auto Get() const -> const std::vector<uint32_t>&;

    void Set(const std::vector<uint32_t>& buffer);

    auto VertexIndex(size_t index) -> uint32_t;

    auto VertexIndex(size_t index) const -> uint32_t;

    void SetVertexIndex(size_t index, uint32_t value);

    auto Empty() const -> bool;

    auto Size() const -> size_t;

    void Resize(size_t size);

    void Resize(size_t size, uint32_t value);

    void MakeContinuous();

    void Update();

    auto operator[](size_t index) -> uint32_t&;

    auto operator[](size_t index) const -> uint32_t;

private:
    GLuint m_veabID{};
    size_t m_veabSize{};

    VertexBufferUsage m_vboUsage{VertexBufferUsage::StaticDraw};

    std::vector<uint32_t> m_indices;
};

} // namespace Renderer
} // namespace libprojectM
