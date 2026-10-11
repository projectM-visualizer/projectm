#include "ProjectM.hpp"

#include <projectM-4/core.h>

#include <gtest/gtest.h>

#include <istream>

TEST(PreloadPresetApiTest, ExposesPresetDataPreloadMethods)
{
    auto cxxMethod = &libprojectM::ProjectM::PreloadPresetData;
    auto cMethod = &projectm_preload_preset_data;

    EXPECT_NE(cxxMethod, nullptr);
    EXPECT_NE(cMethod, nullptr);
}
