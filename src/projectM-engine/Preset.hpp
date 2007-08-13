
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
#include <map>

#define PRESET_DEBUG 0 /* 0 for no debugging, 1 for normal, 2 for insane */

#include "CustomShape.hpp"
#include "CustomWave.hpp"
#include "Expr.hpp"
#include "PerPixelEqn.hpp"
#include "PerFrameEqn.hpp"
#include "BuiltinParams.hpp"
#include "PresetFrameIO.hpp"
#include "InitCond.hpp"

class CustomWave;
class CustomShape;
class InitCond;


class Preset
{
protected:


public:


  ///  Load a preset by filename with input and output buffers specified.
  ///  This is the only proper way to allocate a new preset.
  /// \param filename the absolute file path of a preset to load from the file system
  /// \param presetInputs a const reference to read only projectM engine variables
  /// \param presetOutputs initialized and filled with data parsed from a preset
  Preset(const std::string & filename, const PresetInputs & presetInputs, PresetOutputs & presetOutputs);

  /// A special preset destructor. Very important: the preset destructor is currently responsible
  /// for clearing out all heap memory it allocated in presetOutputs. This might change in future.
  /// Conclusion: It does NOT necessarily reset or clear any values. It only deallocates.
  ~Preset();

  /// Evaluates the preset for a frame given the current values of preset inputs / outputs
  /// All calculated values are stored in the associated preset outputs instance
  void evaluateFrame();

  /// All "builtin" parameters for this preset. Anything *but* user defined parameters and
  /// custom waves / shapes objects go here.
  /// @bug encapsulate
  BuiltinParams builtinParams;

  /// Preset name, very boring, like [preset00]
  std::string name;

  /// Used by parser to find/create custom waves and shapes. May be refactored
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

  /// Used by parser
  /// @bug refactor
  int add_per_pixel_eqn( char *name, GenExpr *gen_expr );

  /// Accessor method to retrieve the absolute file path of the loaded preset
  /// \returns a file path string 
  std::string absoluteFilePath() const
  {
    return file_path;
  }

  /// Accessor method for the preset outputs instance associated with this preset
  /// \returns A preset output instance with values computed from most recent evaluateFrame()
  PresetOutputs & presetOutputs() const
  {
    return m_presetOutputs;
  }

  /// @bug encapsulate
  PresetOutputs::cwave_container * customWaves;
  PresetOutputs::cshape_container * customShapes;

  /// @bug encapsulate
  /* Data structures that contain equation and initial condition information */
  std::map<int, PerFrameEqn*>  per_frame_eqn_tree;   /* per frame equations */
  std::map<int, PerPixelEqn*>  per_pixel_eqn_tree; /* per pixel equation tree */
  std::map<std::string,InitCond*>  per_frame_init_eqn_tree; /* per frame initial equations */
  std::map<std::string,InitCond*>  init_cond_tree; /* initial conditions */
  std::map<std::string,Param*> user_param_tree; /* user parameter splay tree */

private:

  // The absolute file path of the preset
  std::string file_path;

  void initialize(const std::string & pathname);
  int loadPresetFile(std::string pathname);

  void loadBuiltinParamsUnspecInitConds();
  void loadCustomWaveUnspecInitConds();
  void loadCustomShapeUnspecInitConds();

  void evalCustomWavePerFrameEquations();
  void evalCustomShapePerFrameEquations();
  void evalPerFrameInitEquations();
  void evalCustomWaveInitConditions();
  void evalCustomShapeInitConditions();
  void evalPerPixelEqns();
  void evalPerFrameEquations();


  inline void clearMeshChecks();

  PresetOutputs & m_presetOutputs;

};

template <class CustomObject>
CustomObject * Preset::find_custom_object(int id, bool create_flag, std::map<int, CustomObject*> & customObjects)
{

  CustomObject * custom_object = NULL;


  typename std::map<int, CustomObject*>::iterator pos = customObjects.find(id);

  if (pos == customObjects.end())
  {
    if (create_flag == false)
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

  }
  else
    custom_object = pos->second;


  assert(custom_object);
  return custom_object;
}


inline void Preset::clearMeshChecks()
{
  m_presetOutputs.zoom_is_mesh = false;
  m_presetOutputs.zoomexp_is_mesh = false;
  m_presetOutputs.rot_is_mesh = false;
  m_presetOutputs.sx_is_mesh = false;
  m_presetOutputs.sy_is_mesh = false;
  m_presetOutputs.dx_is_mesh = false;
  m_presetOutputs.dy_is_mesh = false;
  m_presetOutputs.cx_is_mesh = false;
  m_presetOutputs.cy_is_mesh = false;

}

#endif /** !_PRESET_HPP */
