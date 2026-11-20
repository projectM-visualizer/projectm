#include <gtest/gtest.h>

#include <MilkdropPreset/EquationCode.hpp>

#include <projectm-eval.h>

using libprojectM::MilkdropPreset::EquationCode;

TEST(EquationCode, JoinsLinesWithoutSeparator)
{
    // Milkdrop's editor wrapped long lines anywhere, here in the middle of "treb_att".
    EquationCode const equations("rot = above(treb, t\nreb_att) * 0.1;\n");

    EXPECT_EQ(equations.Code(), "rot = above(treb, treb_att) * 0.1;");
}

TEST(EquationCode, StripsCommentsUntilEndOfLine)
{
    EquationCode const equations("r = 1.0; // red\ng = 1.0; \\\\ green\n// a comment line\nb = 1.0;\n");

    EXPECT_EQ(equations.Code(), "r = 1.0; g = 1.0; b = 1.0;");
}

TEST(EquationCode, KeepsMultilineComments)
{
    // The expression compiler skips them itself.
    EquationCode const equations("r = 1.0; /* Comment...\ng = 1.0;\n... ends here */b = 1.0;\n");

    EXPECT_EQ(equations.Code(), "r = 1.0; /* Comment...g = 1.0;... ends here */b = 1.0;");
}

TEST(EquationCode, EmptyCode)
{
    EquationCode const equations("");

    EXPECT_EQ(equations.Code(), "");
    EXPECT_EQ(equations.OriginalPosition(0).line, 1);
    EXPECT_EQ(equations.OriginalPosition(0).column, 1);
}

TEST(EquationCode, OriginalPosition)
{
    // Lines: "r = 1.0; // red", "", "g = 2.0;", "b = 3.0;"
    EquationCode const equations("r = 1.0; // red\n\ng = 2.0;\nb = 3.0;\n");
    ASSERT_EQ(equations.Code(), "r = 1.0; g = 2.0;b = 3.0;");

    // "r", first character
    EXPECT_EQ(equations.OriginalPosition(0).line, 1);
    EXPECT_EQ(equations.OriginalPosition(0).column, 1);

    // ";" at the end of line 1, before the removed comment
    EXPECT_EQ(equations.OriginalPosition(7).line, 1);
    EXPECT_EQ(equations.OriginalPosition(7).column, 8);

    // "g" starts line 3: the empty line 2 holds no character
    EXPECT_EQ(equations.OriginalPosition(9).line, 3);
    EXPECT_EQ(equations.OriginalPosition(9).column, 1);

    // "2" in line 3
    EXPECT_EQ(equations.OriginalPosition(13).line, 3);
    EXPECT_EQ(equations.OriginalPosition(13).column, 5);

    // "b" starts line 4
    EXPECT_EQ(equations.OriginalPosition(17).line, 4);
    EXPECT_EQ(equations.OriginalPosition(17).column, 1);

    // End of the code: after the last character of the last line
    EXPECT_EQ(equations.OriginalPosition(25).line, 4);
    EXPECT_EQ(equations.OriginalPosition(25).column, 9);
}

/**
 * The column given by the expression compiler maps back to the line and column of the preset.
 */
TEST(EquationCode, CompileErrorPosition)
{
    auto* memory = projectm_eval_memory_buffer_create();
    auto* context = projectm_eval_context_create(memory, nullptr);

    // "0.99l" on line 3 isn't a valid number. Line 2 ends in the middle of "zoom".
    EquationCode const equations("rot = 0.1; // turn\nzo\nom = 1.0; warp = 0.99l;\n");
    auto* code = projectm_eval_code_compile(context, equations.Code().c_str());
    ASSERT_EQ(code, nullptr);

    int line{};
    int column{};
    ASSERT_NE(projectm_eval_get_error(context, &line, &column), nullptr);

    auto const position = equations.OriginalPosition(column);
    EXPECT_EQ(position.line, 3);
    EXPECT_EQ(position.column, 22); // the "l"

    projectm_eval_context_destroy(context);
    projectm_eval_memory_buffer_destroy(memory);
}

TEST(EquationCode, CompilesLinesSplitInsideAName)
{
    auto* memory = projectm_eval_memory_buffer_create();
    auto* context = projectm_eval_context_create(memory, nullptr);

    EquationCode const equations("rot = above(treb, t\nreb_att) * 0.1;\n");
    auto* code = projectm_eval_code_compile(context, equations.Code().c_str());
    EXPECT_NE(code, nullptr);

    if (code != nullptr)
    {
        projectm_eval_code_destroy(code);
    }
    projectm_eval_context_destroy(context);
    projectm_eval_memory_buffer_destroy(memory);
}
