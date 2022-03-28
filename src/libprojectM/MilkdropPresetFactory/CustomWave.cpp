/**
 * projectM -- Milkdrop-esque visualisation SDK
 * Copyright (C)2003-2004 projectM Team
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
 * You  have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * See 'LICENSE.txt' included within this release
 *
 */

#include "CustomWave.hpp"

#include "Common.hpp"
#include "Expr.hpp"
#include "InitCondUtils.hpp"
#include "Param.hpp"
#include "ParamUtils.hpp"
#include "PerPointEqn.hpp"
#include "Preset.hpp"
#include "fatal.h"

#include <cstdio>
#include <map>
#include <sstream>

#define MAX_SAMPLE_SIZE 4096

CustomWave::CustomWave(int _id)
    : Waveform(512)
    , id(_id)
{
    this->r_mesh.resize(MAX_SAMPLE_SIZE);
    this->g_mesh.resize(MAX_SAMPLE_SIZE);
    this->b_mesh.resize(MAX_SAMPLE_SIZE);
    this->a_mesh.resize(MAX_SAMPLE_SIZE);
    this->x_mesh.resize(MAX_SAMPLE_SIZE);
    this->y_mesh.resize(MAX_SAMPLE_SIZE);

    /* Start: Load custom wave parameters */
    ParamUtils::insert(Param::new_param_float(
        "r", P_FLAG_NONE | P_FLAG_PER_POINT, &r, r_mesh.data(), 1.0, 0.0, .5), &param_tree);
    ParamUtils::insert(Param::new_param_float(
        "g", P_FLAG_NONE | P_FLAG_PER_POINT, &g, g_mesh.data(), 1.0, 0.0, .5), &param_tree);
    ParamUtils::insert(Param::new_param_float(
        "b", P_FLAG_NONE | P_FLAG_PER_POINT, &b, b_mesh.data(), 1.0, 0.0, .5), &param_tree);
    ParamUtils::insert(Param::new_param_float(
        "a", P_FLAG_NONE | P_FLAG_PER_POINT, &this->a, this->a_mesh.data(), 1.0, 0.0, .5), &param_tree);
    ParamUtils::insert(Param::new_param_float(
        "x", P_FLAG_NONE | P_FLAG_PER_POINT, &this->x, this->x_mesh.data(), 1.0, 0.0, .5), &param_tree);
    ParamUtils::insert(Param::new_param_float(
        "y", P_FLAG_NONE | P_FLAG_PER_POINT, &this->y, this->y_mesh.data(), 1.0, 0.0, .5), &param_tree);
    ParamUtils::insert(Param::new_param_bool(
        "enabled", P_FLAG_NONE, &this->enabled, 1, 0, 0), &param_tree);
    ParamUtils::insert(Param::new_param_int(
        "sep", P_FLAG_NONE, &this->sep, 100, -100, 0), &param_tree);
    ParamUtils::insert(Param::new_param_bool(
        "bspectrum", P_FLAG_NONE, &this->spectrum, 1, 0, 0), &param_tree);
    ParamUtils::insert(Param::new_param_bool(
        "bdrawthick", P_FLAG_NONE, &this->thick, 1, 0, 0), &param_tree);
    ParamUtils::insert(Param::new_param_bool(
        "busedots", P_FLAG_NONE, &this->dots, 1, 0, 0), &param_tree);
    ParamUtils::insert(Param::new_param_bool(
        "badditive", P_FLAG_NONE, &this->additive, 1, 0, 0), &param_tree);
    ParamUtils::insert(Param::new_param_int(
        "samples", P_FLAG_NONE, &this->samples, 2048, 1, 512), &param_tree);
    ParamUtils::insert(Param::new_param_float(
        "sample", P_FLAG_READONLY | P_FLAG_NONE, &this->sample, nullptr, 1.0, 0.0, 0.0), &param_tree);
    ParamUtils::insert(Param::new_param_float(
        "value1", P_FLAG_READONLY | P_FLAG_NONE, &this->v1, nullptr, 1.0, -1.0, 0.0), &param_tree);
    ParamUtils::insert(Param::new_param_float(
        "value2", P_FLAG_READONLY | P_FLAG_NONE, &this->v2, nullptr, 1.0, -1.0, 0.0), &param_tree);
    ParamUtils::insert(Param::new_param_float(
        "smoothing", P_FLAG_NONE, &this->smoothing, nullptr, 1.0, 0.0, 0.0), &param_tree);
    ParamUtils::insert(Param::new_param_float(
        "scaling", P_FLAG_NONE, &this->scaling, nullptr, MAX_DOUBLE_SIZE, 0.0, 1.0), &param_tree);
    ParamUtils::insert(Param::new_param_float(
        "t1", P_FLAG_TVAR, &this->t1, nullptr, MAX_DOUBLE_SIZE, -MAX_DOUBLE_SIZE, 0.0), &param_tree);
    ParamUtils::insert(Param::new_param_float(
        "t2", P_FLAG_TVAR, &this->t2, nullptr, MAX_DOUBLE_SIZE, -MAX_DOUBLE_SIZE, 0.0), &param_tree);
    ParamUtils::insert(Param::new_param_float(
        "t3", P_FLAG_TVAR, &this->t3, nullptr, MAX_DOUBLE_SIZE, -MAX_DOUBLE_SIZE, 0.0), &param_tree);
    ParamUtils::insert(Param::new_param_float(
        "t4", P_FLAG_TVAR, &this->t4, nullptr, MAX_DOUBLE_SIZE, -MAX_DOUBLE_SIZE, 0.0), &param_tree);
    ParamUtils::insert(Param::new_param_float(
        "t5", P_FLAG_TVAR, &this->t5, nullptr, MAX_DOUBLE_SIZE, -MAX_DOUBLE_SIZE, 0.0), &param_tree);
    ParamUtils::insert(Param::new_param_float(
        "t6", P_FLAG_TVAR, &this->t6, nullptr, MAX_DOUBLE_SIZE, -MAX_DOUBLE_SIZE, 0.0),&param_tree);
    ParamUtils::insert(Param::new_param_float(
        "t7", P_FLAG_TVAR, &this->t7, nullptr, MAX_DOUBLE_SIZE, -MAX_DOUBLE_SIZE, 0.0), &param_tree);
    ParamUtils::insert(Param::new_param_float(
        "t8", P_FLAG_TVAR, &this->t8, nullptr, MAX_DOUBLE_SIZE, -MAX_DOUBLE_SIZE, 0.0), &param_tree);

    for (unsigned int i = 0; i < NUM_Q_VARIABLES; i++)
    {
        std::ostringstream os;
        os << "q" << i + 1;
        ParamUtils::insert(Param::new_param_float(
            os.str().c_str(), P_FLAG_QVAR, &this->q[i], nullptr, MAX_DOUBLE_SIZE, -MAX_DOUBLE_SIZE,0.0), &this->param_tree);
    }

     /* End of parameter loading. Note that the read only parameters associated
     with custom waves (ie, sample) are variables stored in PresetFrameIO.hpp,
     and not specific to the custom wave data structure. */
}

CustomWave::~CustomWave()
{
  for (const auto& eqn : per_point_eqn_tree)
  {
      delete eqn;
  }

  for (const auto& eqn : per_frame_eqn_tree)
  {
      delete eqn;
  }

  for (const auto& cond : init_cond_tree)
  {
      delete cond.second;
  }

  for (const auto& cond : per_frame_init_eqn_tree)
  {
      delete cond.second;
  }

  for (const auto& param : param_tree)
  {
      delete param.second;
  }
}


// Comments: index is not passed, so we assume monotonic increment by 1 is ok here
int CustomWave::add_per_point_eqn(char* name, Expr* gen_expr)
{
    /* Argument checks */
    if (!gen_expr || !name)
    {
        return PROJECTM_FAILURE;
    }

    if (CUSTOM_WAVE_DEBUG)
    {
        printf("add_per_point_eqn: per pixel equation (name = \"%s\")\n", name);
    }

    /* Search for the parameter so we know what matrix the per pixel equation is referencing */
    auto param = ParamUtils::find<ParamUtils::AUTO_CREATE>(name, &param_tree);
    if (!param)
    {
        if (CUSTOM_WAVE_DEBUG)
        {
            printf("add_per_point_eqn: failed to allocate a new parameter!\n");
        }
        return PROJECTM_FAILURE;
    }

    /* Create a new per point equation given the next available index, parameter, and general expression tree */
    per_point_eqn_tree.push_back(new PerPointEqn(static_cast<int>(per_point_eqn_tree.size()), param, gen_expr));

    /* Done */
    return PROJECTM_SUCCESS;
}

void CustomWave::evalInitConds()
{
    for (auto& eqn: per_frame_init_eqn_tree)
    {
        assert(eqn.second);
        eqn.second->evaluate();
    }
}

Waveform::ColoredPoint CustomWave::PerPoint(ColoredPoint p, const Context& context)
{
    if (!per_point_program)
    {
        // see comment in MilkdropPreset, collect a list of assignments into one ProgramExpr
        // which (theoretically) could be compiled together.
        std::vector<Expr*> steps;
        for (const auto& eqn : per_point_eqn_tree)
        {
            steps.push_back(eqn->assign_expr);
        }
        Expr *program_expr  = Expr::create_program_expr(steps, false);
        Expr *jit = nullptr;
#if HAVE_LLVM
        char buffer[100];
        sprintf(buffer, "wave_%d", id);
        if (!steps.empty())
            jit = Expr::jit(program_expr, buffer);
#endif
        per_point_program = jit ? jit : program_expr;
    }

    r_mesh[context.sample_int] = r;
    g_mesh[context.sample_int] = g;
    b_mesh[context.sample_int] = b;
    a_mesh[context.sample_int] = a;
    x_mesh[context.sample_int] = x;
    y_mesh[context.sample_int] = y;
    sample = context.sample;
    v1 = context.left;
    v2 = context.right;

    per_point_program->eval(context.sample_int, -1);

    p.a = a_mesh[context.sample_int];
    p.r = r_mesh[context.sample_int];
    p.g = g_mesh[context.sample_int];
    p.b = b_mesh[context.sample_int];
    p.x = x_mesh[context.sample_int];
    p.y = y_mesh[context.sample_int];

    return p;
}


void CustomWave::loadUnspecInitConds()
{

  InitCondUtils::LoadUnspecInitCond fun(this->init_cond_tree, this->per_frame_init_eqn_tree);
  traverse(param_tree, fun);
}

