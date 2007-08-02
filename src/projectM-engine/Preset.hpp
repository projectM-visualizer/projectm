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
/**
 * $Id$
 *
 * Preset
 *
 * $Log$
 */

#ifndef _PRESET_HPP
#define _PRESET_HPP

#include "Common.hpp"
#include <string>
#include <cassert>

#define PRESET_DEBUG 2 /* 0 for no debugging, 1 for normal, 2 for insane */

#include "CustomShape.hpp"
#include "CustomWave.hpp"
#include "Expr.hpp"
#include "PerPixelEqn.hpp"
#include "PerFrameEqn.hpp"
#include "BuiltinParams.hpp"
#include "PresetFrameIO.hpp"
#include <map>
#include "InitCond.hpp"



class CustomWave;
class CustomShape;
class InitCond;




class Preset {
protected:


public:

    /**  Load a preset by filename with input and output buffers specified.
     * This is the only poper way to allocate a new preset.  */
    Preset(const std::string & filename, const PresetInputs & presetInputs, PresetOutputs & presetOutputs);

    ~Preset();

    /** Evaluates the preset for a frame given the current values of preset inputs / outputs */
    void evaluateFrame();

    BuiltinParams builtinParams;

    std::string name;
    std::string file_path;


    void load_init_conditions();
    template <class CustomObject>
    static CustomObject * find_custom_object(int id, bool create_flag, std::map<int,CustomObject*> & customObjects);

    int per_pixel_eqn_string_index;
    int per_frame_eqn_string_index;
    int per_frame_init_eqn_string_index;

    int per_frame_eqn_count,
    per_frame_init_eqn_count;

    char per_pixel_eqn_string_buffer[STRING_BUFFER_SIZE];
    char per_frame_eqn_string_buffer[STRING_BUFFER_SIZE];
    char per_frame_init_eqn_string_buffer[STRING_BUFFER_SIZE];

    /* Data structures that contain equation and initial condition information */
    std::map<int, PerFrameEqn*>  per_frame_eqn_tree;   /* per frame equations */
    std::map<int, PerPixelEqn*>  per_pixel_eqn_tree; /* per pixel equation tree */
    GenExpr * per_pixel_eqn_array[NUM_OPS]; /* per pixel equation array */
    std::map<std::string,InitCond*>  per_frame_init_eqn_tree; /* per frame initial equations */
    std::map<std::string,InitCond*>  init_cond_tree; /* initial conditions */
    std::map<std::string,Param*> user_param_tree; /* user parameter splay tree */

    int add_per_pixel_eqn( char *name, GenExpr *gen_expr );


    int resetPerPixelEqns();
    int resetPerPixelEqnFlags();

    InitCond *get_init_cond( Param *param );

    void load_custom_wave_init_conditions();
    void load_custom_wave_init( CustomWave *customWave );
    void load_custom_shape_init_conditions();
    void load_custom_shape_init( CustomShape *customShape );
    void initialize(const std::string & pathname);

    void reloadPerFrame(char * s);
    void reloadPerFrameInit(char *s);
    void reloadPerPixel(char *s);
    int load_preset_file(const char * pathname);
    static Preset *load_preset( const char *pathname );
    void savePreset(char * name);
    int write_preset_name( FILE *fs );
    int write_init_conditions( FILE *fs );
    int write_init( InitCond *initCond );
    int write_per_frame_init_equations( FILE *fs );
    int write_per_frame_equations( FILE *fs );
    int write_per_pixel_equations( FILE *fs );
    Param * find(char * name, int flags) ;
    int destroy();
    void load_init_cond(char *name, int flags);

    PresetOutputs::cwave_container * customWaves;
    PresetOutputs::cshape_container * customShapes;
    
private:

    void evalCustomWavePerFrameEquations();
    void evalCustomShapePerFrameEquations();
    void evalInitConditions();
    void evalCustomWaveInitConditions();
    void evalCustomShapeInitConditions();
    void evalPerPixelEqns();
    void evalPerFrameEquations();

    PresetOutputs & m_presetOutputs;
};

template <class CustomObject>
CustomObject * Preset::find_custom_object(int id, bool create_flag, std::map<int, CustomObject*> & customObjects)
{

  CustomObject * custom_object = NULL;
  

  typename std::map<int, CustomObject*>::iterator pos = customObjects.find(id);

  if (pos == customObjects.end())
  {
    if (create_flag == FALSE)
    {
      return NULL;
    }

    if ((custom_object = new CustomObject(id)) == NULL)
    {
      return NULL;
    }

    std::pair<typename std::map<int,CustomObject*>::iterator, bool> inserteePair = 
	customObjects.insert(std::make_pair(custom_object->id, custom_object));

    assert(inserteePair.second);

    custom_object = inserteePair.first->second;

  } else 
	custom_object = pos->second;
  

  assert(custom_object);
  return custom_object;
}
#endif /** !_PRESET_HPP */
