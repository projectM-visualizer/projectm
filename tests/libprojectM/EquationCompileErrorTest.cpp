#include <gtest/gtest.h>

#include <MilkdropPreset/PerFrameContext.hpp>

#include <projectm-eval.h>

using libprojectM::MilkdropPreset::PerFrameContext;

namespace {

/**
 * PresetState needs an OpenGL context, so the expression context is set up with its own memory here.
 */
class EquationCompileErrorTest : public testing::Test
{
protected:
    void SetUp() override
    {
        memory = projectm_eval_memory_buffer_create();
        context = std::make_unique<PerFrameContext>(memory, &registers);
        context->RegisterBuiltinVariables();
    }

    void TearDown() override
    {
        context.reset();
        projectm_eval_memory_buffer_destroy(memory);
    }

    projectm_eval_mem_buffer memory{};
    PRJM_EVAL_F registers[100]{};
    std::unique_ptr<PerFrameContext> context;
};

} // namespace

/**
 * Like Milkdrop, a code block that can't be compiled is skipped, and the rest of the preset is still used.
 */
TEST_F(EquationCompileErrorTest, InvalidPerFrameCodeIsSkipped)
{
    // "0.99l" isn't a valid number, so this doesn't compile.
    EXPECT_NO_THROW(context->CompilePerFrameCode("additive = above(sin(time * 200), 0.99l);"));
    EXPECT_EQ(context->perFrameCodeHandle, nullptr);

    // Executing the skipped code does nothing.
    EXPECT_NO_THROW(context->ExecutePerFrameCode());
}

TEST_F(EquationCompileErrorTest, ValidPerFrameCodeIsCompiledAndExecuted)
{
    context->CompilePerFrameCode("zoom = 1.25;");
    ASSERT_NE(context->perFrameCodeHandle, nullptr);

    context->ExecutePerFrameCode();
    EXPECT_EQ(*context->zoom, 1.25);
}
