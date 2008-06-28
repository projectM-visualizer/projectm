#include "PresetMerge.hpp"

#include <iostream>

void PresetMerger::MergePresets(PresetOutputs & A, PresetOutputs & B, double ratio, int gx, int gy)
{

double invratio = 1.0 - ratio;
  //Merge Simple Waveforms
  //
  // All the mess is because of Waveform 7, which is two lines.
  //


  //Merge Custom Shapes and Custom Waves

  for (PresetOutputs::cshape_container::iterator pos = A.customShapes.begin();
	pos != A.customShapes.end(); ++pos)
    {
       (*pos)->a *= invratio;
       (*pos)->a2 *= invratio;
       (*pos)->border_a *= invratio;
    }

  for (PresetOutputs::cshape_container::iterator pos = B.customShapes.begin();
	pos != B.customShapes.end(); ++pos)
    {
       (*pos)->a *= ratio;
       (*pos)->a2 *= ratio;
       (*pos)->border_a *= ratio;

        A.customShapes.push_back(*pos);

    }
 for (PresetOutputs::cwave_container::iterator pos = A.customWaves.begin();
	pos != A.customWaves.end(); ++pos)
    {
       (*pos)->a *= invratio;
      for (int x=0; x <   (*pos)->samples; x++)
	{
	   (*pos)->a_mesh[x]= (*pos)->a_mesh[x]*invratio;
	}
    }

  for (PresetOutputs::cwave_container::iterator pos = B.customWaves.begin();
	pos != B.customWaves.end(); ++pos)
    {
       (*pos)->a *= ratio;
      for (int x=0; x < (*pos)->samples; x++)
	{
	   (*pos)->a_mesh[x]= (*pos)->a_mesh[x]*ratio;
	}
       A.customWaves.push_back(*pos);
    }


  //Interpolate Per-Pixel mesh

  for (int x=0;x<gx;x++)
    {
      for(int y=0;y<gy;y++)
	{
	  A.x_mesh[x][y]  = A.x_mesh[x][y]* invratio + B.x_mesh[x][y]*ratio;
	}
    }
 for (int x=0;x<gx;x++)
    {
      for(int y=0;y<gy;y++)
	{
	  A.y_mesh[x][y]  = A.y_mesh[x][y]* invratio + B.y_mesh[x][y]*ratio;
	}
    }



 //Interpolate PerFrame floats

  A.screenDecay = A.screenDecay * invratio + B.screenDecay * ratio;

  A.wave.r = A.wave.r* invratio + B.wave.r*ratio;
  A.wave.g = A.wave.g* invratio + B.wave.g*ratio;
  A.wave.b = A.wave.b* invratio + B.wave.b*ratio;
  A.wave.a = A.wave.a* invratio + B.wave.a*ratio;
  A.wave.x = A.wave.x* invratio + B.wave.x*ratio;
  A.wave.y = A.wave.y* invratio + B.wave.y*ratio;
  A.wave.mystery = A.wave.mystery* invratio + B.wave.mystery*ratio;

  A.border.outer_size = A.border.outer_size* invratio + B.border.outer_size*ratio;
  A.border.outer_r = A.border.outer_r* invratio + B.border.outer_r*ratio;
  A.border.outer_g = A.border.outer_g* invratio + B.border.outer_g*ratio;
  A.border.outer_b = A.border.outer_b* invratio + B.border.outer_b*ratio;
  A.border.outer_a = A.border.outer_a* invratio + B.border.outer_a*ratio;

  A.border.inner_size = A.border.inner_size* invratio + B.border.inner_size*ratio;
  A.border.inner_r = A.border.inner_r* invratio + B.border.inner_r*ratio;
  A.border.inner_g = A.border.inner_g* invratio + B.border.inner_g*ratio;
  A.border.inner_b = A.border.inner_b* invratio + B.border.inner_b*ratio;
  A.border.inner_a = A.border.inner_a* invratio + B.border.inner_a*ratio;

  A.mv.a  = A.mv.a* invratio + B.mv.a*ratio;
  A.mv.r  = A.mv.r* invratio + B.mv.r*ratio;
  A.mv.g  = A.mv.g* invratio + B.mv.g*ratio;
  A.mv.b  = A.mv.b* invratio + B.mv.b*ratio;
  A.mv.length = A.mv.length* invratio + B.mv.length*ratio;
  A.mv.x_num = A.mv.x_num* invratio + B.mv.x_num*ratio;
  A.mv.y_num = A.mv.y_num* invratio + B.mv.y_num*ratio;
  A.mv.y_offset = A.mv.y_offset* invratio + B.mv.y_offset*ratio;
  A.mv.x_offset = A.mv.x_offset* invratio + B.mv.x_offset*ratio;


  A.fRating = A.fRating* invratio + B.fRating*ratio;
  A.fGammaAdj = A.fGammaAdj* invratio + B.fGammaAdj*ratio;
  A.videoEchoZoom = A.videoEchoZoom* invratio + B.videoEchoZoom*ratio;
  A.videoEchoAlpha = A.videoEchoAlpha* invratio + B.videoEchoAlpha*ratio;


  A.fWarpAnimSpeed = A.fWarpAnimSpeed* invratio + B.fWarpAnimSpeed*ratio;
  A.fWarpScale = A.fWarpScale* invratio + B.fWarpScale*ratio;
  A.fShader = A.fShader* invratio + B.fShader*ratio;

  //Switch bools and discrete values halfway.  Maybe we should do some interesting stuff here.

  if (ratio > 0.5)
    {
      A.videoEchoOrientation = B.videoEchoOrientation;
      A.textureWrap = B.textureWrap;
      A.bDarkenCenter = B.bDarkenCenter;
      A.bRedBlueStereo = B.bRedBlueStereo;
      A.bBrighten = B.bBrighten;
      A.bDarken = B.bDarken;
      A.bSolarize = B.bSolarize;
      A.bInvert = B.bInvert;
      A.bMotionVectorsOn = B.bMotionVectorsOn;
    }

  return;
}

