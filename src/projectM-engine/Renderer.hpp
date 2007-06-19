#ifndef Renderer_HPP
#define Renderer_HPP

#include "pbuffer.h"
#include "PresetFrameIO.hpp"

class Renderer
{
  RenderTarget *renderTarget;
  //per pixel equation variables
  float **gridx;  //grid containing interpolated mesh 
  float **gridy;
  float **origtheta;  //grid containing interpolated mesh reference values
  float **origrad;
  float **origx;  //original mesh 
  float **origy;
  float **origx2;  //original mesh 
  float **origy2;
  int gx;
  int gy;
  
  int mesh_i, mesh_j;
  
 public:
 
  Renderer(int gx, int gy, int texsize);
  ~Renderer();
  void RenderFrame(PresetOutputs *presetOutputs, PresetInputs *presetInputs);
  void reset(int w, int h);

private:

  void PerFrame(PresetOutputs *presetOutputs, PresetInputs *presetInputs);
  void Interpolation(PresetOutputs *presetOutputs, PresetInputs *presetInputs);
  void PerPixelMath(PresetOutputs *presetOutputs);
  void reset_per_pixel_matrices();
  void rescale_per_pixel_matrices();
  void maximize_colors(PresetOutputs *presetOutputs);
};

#endif
