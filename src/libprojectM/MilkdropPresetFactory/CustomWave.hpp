/**
 * projectM -- Milkdrop-esque visualisation SDK
 * Copyright (C)2003-2007 projectM Team
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * See 'LICENSE.txt' included within this release
 *
 */
#pragma once

#define CUSTOM_WAVE_DEBUG 0

class Expr;
class PerPointEqn;
class Preset;

#include "Renderer/Waveform.hpp"

#include "Common.hpp"

#include "MilkdropPresetFactory/Param.hpp"
#include "MilkdropPresetFactory/PerFrameEqn.hpp"

#include <map>
#include <vector>

class CustomWave : public Waveform
{
public:
     CustomWave() = delete;

     /**
      * @brief Initializes a custom waveform with the given ID
      * @param id The ID (index) of the custom wave.
      */
     explicit CustomWave(int id);

    ~CustomWave() override;

    int add_per_point_eqn(char* name, Expr* gen_expr);

    void evalCustomWaveInitConditions(Preset* preset);

    void loadUnspecInitConds();

    void evalInitConds();

    ColoredPoint PerPoint(ColoredPoint p, const Context& context) override;

    /* Numerical id */
    int id{ 0 };
    int per_frame_count{ 0 };

    /* Parameter tree associated with this custom wave */
    std::map<std::string,Param*> param_tree;

    /* Engine variables */
    float x{ 0.0f }; /* x position for per point equations */
    float y{ 0.0f }; /* y position for per point equations */
    float r{ 0.0f }; /* red color value */
    float g{ 0.0f }; /* green color value */
    float b{ 0.0f }; /* blue color value */
    float a{ 0.0f }; /* alpha color value */

    std::vector<float> x_mesh;
    std::vector<float> y_mesh;
    std::vector<float> r_mesh;
    std::vector<float> b_mesh;
    std::vector<float> g_mesh;
    std::vector<float> a_mesh;

    bool enabled{ false }; /* if true then wave is visible, hidden otherwise */

    float sample{ 0.0f };

    /* stupid t variables */
    float t1{ 0.0f };
    float t2{ 0.0f };
    float t3{ 0.0f };
    float t4{ 0.0f };
    float t5{ 0.0f };
    float t6{ 0.0f };
    float t7{ 0.0f };
    float t8{ 0.0f };


    /* stupider q variables */
    float q[NUM_Q_VARIABLES]{};

    float v2{ 0.0f};
    float v1{ 0.0f};

    /* Data structures to hold per frame and per point equations */
    std::map<std::string,InitCond*>  init_cond_tree;
    std::vector<PerFrameEqn*>  per_frame_eqn_tree;
    std::vector<PerPointEqn*>  per_point_eqn_tree;
    Expr* per_point_program{ nullptr };
    std::map<std::string,InitCond*>  per_frame_init_eqn_tree;

    /* Denotes the index of the last character for each string buffer */
    int per_point_eqn_string_index;
    int per_frame_eqn_string_index;
    int per_frame_init_eqn_string_index;
};
