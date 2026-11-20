#include <gtest/gtest.h>

#include <MilkdropPreset/PerFrameContext.hpp>
#include <MilkdropPreset/PresetFileParser.hpp>

#include <projectm-eval.h>

#include <sstream>
#include <utility>

using libprojectM::MilkdropPreset::PerFrameContext;
using libprojectM::MilkdropPreset::PresetFileParser;

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

namespace {

/**
 * Compiles the per-frame code of the given preset text and returns the preset location of the compiler error.
 */
auto CompileErrorLocation(const std::string& presetText) -> std::pair<int, int>
{
    PresetFileParser parser;
    std::istringstream stream("[preset00]\n" + presetText);
    EXPECT_TRUE(parser.Read(stream));
    auto code = parser.GetEquationCode("per_frame_");

    auto* memory = projectm_eval_memory_buffer_create();
    auto* evalContext = projectm_eval_context_create(memory, nullptr);
    EXPECT_EQ(projectm_eval_code_compile(evalContext, code.Code().c_str()), nullptr);
    int line{};
    int column{};
    projectm_eval_get_error(evalContext, &line, &column);
    projectm_eval_context_destroy(evalContext);
    projectm_eval_memory_buffer_destroy(memory);

    auto location = code.PresetLocation(line, column);
    return {location.line, location.column};
}

} // namespace

/**
 * Compiler errors are reported at the preset line and column, also if lines were joined.
 */
TEST(EquationCompileErrorLocation, SeparateLines)
{
    // Unexpected ";" in "y=y+;"
    EXPECT_EQ(CompileErrorLocation("per_frame_1=x = 1;\nper_frame_2=\nper_frame_3=y=y+;\n"), std::make_pair(3, 5));
}

TEST(EquationCompileErrorLocation, FirstLine)
{
    // Unexpected "*" in the first line
    EXPECT_EQ(CompileErrorLocation("per_frame_1=abc = 1; def = 2 +* 3;\n"), std::make_pair(1, 19));
}

TEST(EquationCompileErrorLocation, JoinedLines)
{
    // The second line continues the name "t", and the error is the "*" after it.
    EXPECT_EQ(CompileErrorLocation("per_frame_1=x=above(treb,t\nper_frame_2=reb_att)+*0.25;\n"), std::make_pair(2, 10));
}

TEST(EquationCompileErrorLocation, AfterComment)
{
    // The comment is removed, the location is still the one in the preset.
    EXPECT_EQ(CompileErrorLocation("per_frame_1=q=1; // comment\nper_frame_2=r=2 r;\n"), std::make_pair(2, 5));
}

