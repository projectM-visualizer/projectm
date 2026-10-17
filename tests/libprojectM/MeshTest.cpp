#include <gtest/gtest.h>

#include <Renderer/Mesh.hpp>

#include <array>
#include <cstdint>
#include <vector>

namespace {

// Mesh needs a GL context. Record the GL boundary here so these tests also run on headless CI.
class MeshTest : public testing::Test
{
protected:
    struct Attribute {
        GLint size{};
        GLsizei stride{};
        uintptr_t offset{};
        GLuint divisor{};
        bool enabled{};
    };

    struct Calls {
        GLuint nextId{1};
        GLuint vao{};
        GLenum primitive{};
        GLsizei vertices{};
        GLsizei instances{};
        int indexedDraws{};
        int instancedDraws{};
        int uploads{};
        int subUploads{};
        int generatedBuffers{};
        int deletedBuffers{};
        GLenum usage{};
        std::vector<uint8_t> bytes;
        std::array<Attribute, 8> attributes;
    };

    static Calls calls;

    static void GLAD_API_PTR MockglGenBuffers(GLsizei n, GLuint* ids)
    {
        calls.generatedBuffers += n;
        while (n--)
            *ids++ = calls.nextId++;
    }

    static void GLAD_API_PTR MockglGenVertexArrays(GLsizei n, GLuint* ids)
    {
        while (n--)
            *ids++ = calls.nextId++;
    }

    static void GLAD_API_PTR MockglDeleteBuffers(GLsizei n, const GLuint*)
    {
        calls.deletedBuffers += n;
    }

    static void GLAD_API_PTR MockglDeleteVertexArrays(GLsizei, const GLuint*)
    {
    }

    static void GLAD_API_PTR MockglBindBuffer(GLenum, GLuint)
    {
    }

    static void GLAD_API_PTR MockglBindVertexArray(GLuint id)
    {
        calls.vao = id;
    }

    static void GLAD_API_PTR MockglEnableVertexAttribArray(GLuint i)
    {
        calls.attributes.at(i).enabled = true;
    }

    static void GLAD_API_PTR MockglDisableVertexAttribArray(GLuint i)
    {
        calls.attributes.at(i).enabled = false;
    }

    static void GLAD_API_PTR MockglVertexAttribPointer(GLuint i, GLint size, GLenum, GLboolean, GLsizei stride, const void* offset)
    {
        auto& attribute = calls.attributes.at(i);
        attribute.size = size;
        attribute.stride = stride;
        attribute.offset = reinterpret_cast<uintptr_t>(offset);
    }

    static void GLAD_API_PTR MockglVertexAttribDivisor(GLuint i, GLuint divisor)
    {
        calls.attributes.at(i).divisor = divisor;
    }

    static void GLAD_API_PTR MockglBufferData(GLenum, GLsizeiptr size, const void* data, GLenum usage)
    {
        ++calls.uploads;
        calls.usage = usage;
        const auto* bytes = static_cast<const uint8_t*>(data);
        calls.bytes.assign(bytes, bytes + size);
    }

    static void GLAD_API_PTR MockglBufferSubData(GLenum, GLintptr, GLsizeiptr, const void*)
    {
        ++calls.subUploads;
    }

    static void GLAD_API_PTR MockglDrawElements(GLenum primitive, GLsizei count, GLenum, const void*)
    {
        ++calls.indexedDraws;
        calls.primitive = primitive;
        calls.vertices = count;
    }

    static void GLAD_API_PTR MockglDrawArraysInstanced(GLenum primitive, GLint first, GLsizei vertices, GLsizei instances)
    {
        EXPECT_EQ(first, 0);
        EXPECT_NE(calls.vao, 0u);
        ++calls.instancedDraws;
        calls.primitive = primitive;
        calls.vertices = vertices;
        calls.instances = instances;
    }


    void SetUp() override
    {
        calls = {};
#define HOOK(name)             \
    saved##name = glad_##name; \
    glad_##name = &Mock##name
        HOOK(glGenBuffers);
        HOOK(glGenVertexArrays);
        HOOK(glDeleteBuffers);
        HOOK(glDeleteVertexArrays);
        HOOK(glBindBuffer);
        HOOK(glBindVertexArray);
        HOOK(glEnableVertexAttribArray);
        HOOK(glDisableVertexAttribArray);
        HOOK(glVertexAttribPointer);
        HOOK(glVertexAttribDivisor);
        HOOK(glBufferData);
        HOOK(glBufferSubData);
        HOOK(glDrawElements);
        HOOK(glDrawArraysInstanced);
#undef HOOK
    }

    void TearDown() override
    {
#define RESTORE(name) glad_##name = saved##name
        RESTORE(glGenBuffers);
        RESTORE(glGenVertexArrays);
        RESTORE(glDeleteBuffers);
        RESTORE(glDeleteVertexArrays);
        RESTORE(glBindBuffer);
        RESTORE(glBindVertexArray);
        RESTORE(glEnableVertexAttribArray);
        RESTORE(glDisableVertexAttribArray);
        RESTORE(glVertexAttribPointer);
        RESTORE(glVertexAttribDivisor);
        RESTORE(glBufferData);
        RESTORE(glBufferSubData);
        RESTORE(glDrawElements);
        RESTORE(glDrawArraysInstanced);
#undef RESTORE
    }

#define SAVE(name) decltype(glad_##name) saved##name = nullptr
    SAVE(glGenBuffers);
    SAVE(glGenVertexArrays);
    SAVE(glDeleteBuffers);
    SAVE(glDeleteVertexArrays);
    SAVE(glBindBuffer);
    SAVE(glBindVertexArray);
    SAVE(glEnableVertexAttribArray);
    SAVE(glDisableVertexAttribArray);
    SAVE(glVertexAttribPointer);
    SAVE(glVertexAttribDivisor);
    SAVE(glBufferData);
    SAVE(glBufferSubData);
    SAVE(glDrawElements);
    SAVE(glDrawArraysInstanced);
#undef SAVE
};

MeshTest::Calls MeshTest::calls;

TEST_F(MeshTest, IndexedDrawingKeepsItsDefaultLayoutAndContinuousIndices)
{
    libprojectM::Renderer::Mesh mesh;
    mesh.SetVertexCount(3);
    mesh.Update();
    mesh.Draw();
    EXPECT_EQ(calls.indexedDraws, 1);
    EXPECT_EQ(calls.instancedDraws, 0);
    EXPECT_EQ(calls.primitive, GL_LINES);
    EXPECT_EQ(mesh.Indices().Get(), (std::vector<uint32_t>{0, 1, 2}));
    EXPECT_TRUE(calls.attributes[0].enabled);
    EXPECT_FALSE(calls.attributes[1].enabled);
    EXPECT_FALSE(calls.attributes[2].enabled);
    EXPECT_EQ(calls.attributes[0].divisor, 0u);
}

TEST_F(MeshTest, InstanceUploadsStreamEvenWhenTheirSizeDoesNotChange)
{
    libprojectM::Renderer::Mesh mesh;
    const std::array<float, 6> points{{1, 2, 3, 4, 5, 6}};
    mesh.UpdateInstanceData(points.data(), sizeof(points));
    mesh.UpdateInstanceData(points.data(), sizeof(points));
    EXPECT_EQ(calls.uploads, 2);
    EXPECT_EQ(calls.subUploads, 0);
    EXPECT_EQ(calls.usage, GL_STREAM_DRAW);
    EXPECT_EQ(calls.bytes.size(), sizeof(points));
    EXPECT_EQ(calls.bytes, (std::vector<uint8_t>(reinterpret_cast<const uint8_t*>(points.data()),
                                                 reinterpret_cast<const uint8_t*>(points.data()) + sizeof(points))));
}

TEST_F(MeshTest, InstanceAttributesAcceptOverlappingPointsAndStripOffsets)
{
    libprojectM::Renderer::Mesh mesh;
    const std::array<float, 36> points{};
    mesh.UpdateInstanceData(points.data(), sizeof(points));
    // Two endpoint positions and a color, reading different fields of the same interleaved stream.
    mesh.SetInstanceAttributes({{0, 2, GL_FLOAT, GL_FALSE, 24, 0, 1},
                                {1, 2, GL_FLOAT, GL_FALSE, 24, 24, 1},
                                {4, 4, GL_FLOAT, GL_FALSE, 24, 32, 1}},
                               48);
    EXPECT_EQ(calls.attributes[0].offset, 48u);
    EXPECT_EQ(calls.attributes[1].offset, 72u);
    EXPECT_EQ(calls.attributes[4].offset, 80u);
    EXPECT_EQ(calls.attributes[4].stride, 24);
    EXPECT_EQ(calls.attributes[4].size, 4);
    EXPECT_EQ(calls.attributes[4].divisor, 1u);
    EXPECT_TRUE(calls.attributes[4].enabled);
    mesh.SetInstanceAttributes({{0, 2, GL_FLOAT, GL_FALSE, 24, 0, 2}}, 0);
    EXPECT_EQ(calls.attributes[0].offset, 0u);
    EXPECT_EQ(calls.attributes[0].divisor, 2u);
}

TEST_F(MeshTest, InstancedDrawingSeparatesQuadVerticesFromSegmentCount)
{
    libprojectM::Renderer::Mesh mesh;
    mesh.SetRenderPrimitiveType(libprojectM::Renderer::Mesh::PrimitiveType::TriangleStrip);
    mesh.DrawInstanced(4, 37);
    EXPECT_EQ(calls.indexedDraws, 0);
    EXPECT_EQ(calls.instancedDraws, 1);
    EXPECT_EQ(calls.primitive, GL_TRIANGLE_STRIP);
    EXPECT_EQ(calls.vertices, 4);
    EXPECT_EQ(calls.instances, 37);
    EXPECT_TRUE(mesh.Indices().Empty());
    EXPECT_EQ(calls.vao, 0u);
}

TEST_F(MeshTest, EmptyInstancedDrawsDoNothing)
{
    libprojectM::Renderer::Mesh mesh;
    mesh.DrawInstanced(4, 0);
    mesh.DrawInstanced(0, 10);
    EXPECT_EQ(calls.instancedDraws, 0);
}

TEST_F(MeshTest, InstanceBufferIsCreatedOnceOnDemandAndReleasedWithTheMesh)
{
    int regularBuffers;
    {
        libprojectM::Renderer::Mesh mesh;
        regularBuffers = calls.generatedBuffers;
        EXPECT_EQ(regularBuffers, 4);
        const float value = 0.5f;
        mesh.UpdateInstanceData(&value, sizeof(value));
        mesh.UpdateInstanceData(&value, sizeof(value));
        EXPECT_EQ(calls.generatedBuffers, regularBuffers + 1);
        EXPECT_EQ(calls.deletedBuffers, 0);
    }
    EXPECT_EQ(calls.deletedBuffers, regularBuffers + 1);
}

} // namespace
