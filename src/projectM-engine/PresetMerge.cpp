#include "PresetMerge.hpp"

void PresetMerger::MergePresets(PresetOutputs & A, PresetOutputs & B, double ratio, int gx, int gy)
{
  double invratio =  ratio;
  ratio = 1.0 - invratio;
  
  for (PresetOutputs::cshape_container::iterator pos = A.customShapes.begin();
	pos != A.customShapes.end(); ++pos) 
    {
      pos->second->a *= ratio;
      pos->second->a2 *= ratio;
      pos->second->border_a *= ratio;
    }

  for (PresetOutputs::cshape_container::iterator pos = B.customShapes.begin();
	pos != B.customShapes.end(); ++pos) 
    {
      pos->second->a *= invratio;
      pos->second->a2 *= invratio;
      pos->second->border_a *= invratio;
      A.customShapes[pos->first>>4]=pos->second;

    }
 for (PresetOutputs::cwave_container::iterator pos = A.customWaves.begin();
	pos != A.customWaves.end(); ++pos) 
    {
      pos->second->a *= ratio;     
    }

  for (PresetOutputs::cwave_container::iterator pos = B.customWaves.begin();
	pos != B.customWaves.end(); ++pos) 
    {
      pos->second->a *= invratio;    
      A.customWaves[pos->first>>4]=pos->second;

    }



 PresetMerger::mergeMesh(A.zoom_is_mesh, B.zoom_is_mesh, A.zoom, B.zoom,  A.zoom_mesh, B.zoom_mesh, gx, gy, ratio);
 PresetMerger:: mergeMesh(A.zoomexp_is_mesh, B.zoomexp_is_mesh, A.zoomexp, B.zoomexp,  A.zoomexp_mesh, B.zoomexp_mesh, gx, gy, ratio);
  PresetMerger::mergeMesh(A.rot_is_mesh, B.rot_is_mesh, A.rot, B.rot,  A.rot_mesh, B.rot_mesh, gx, gy, ratio);
 //mergeMesh(A.warp_is_mesh, B.warp_is_mesh, A.warp, B.warp,  A.warp_mesh, B.warp_mesh, gx, gy, ratio);

  PresetMerger::mergeMesh(A.sx_is_mesh, B.sx_is_mesh, A.sx, B.sx,  A.sx_mesh, B.sx_mesh, gx, gy, ratio);
  PresetMerger::mergeMesh(A.sy_is_mesh, B.sy_is_mesh, A.sy, B.sy,  A.sy_mesh, B.sy_mesh, gx, gy, ratio);
  PresetMerger::mergeMesh(A.dx_is_mesh, B.dx_is_mesh, A.dx, B.dx,  A.dx_mesh, B.dx_mesh, gx, gy, ratio);
  PresetMerger::mergeMesh(A.dy_is_mesh, B.dy_is_mesh, A.dy, B.dy,  A.dy_mesh, B.dy_mesh, gx, gy, ratio);
  PresetMerger::mergeMesh(A.cx_is_mesh, B.cx_is_mesh, A.cx, B.cx,  A.cx_mesh, B.cx_mesh, gx, gy, ratio);
  PresetMerger::mergeMesh(A.cy_is_mesh, B.cy_is_mesh, A.cy, B.cy,  A.cy_mesh, B.cy_mesh, gx, gy, ratio);
 

  A.decay = A.decay * invratio + B.decay * ratio;
  
  A.wave_r = A.wave_r* invratio + B.wave_r*ratio;
  A.wave_g = A.wave_g* invratio + B.wave_g*ratio;
  A.wave_b = A.wave_b* invratio + B.wave_b*ratio;
  A.wave_o = A.wave_o* invratio + B.wave_o*ratio;
  A.wave_x = A.wave_x* invratio + B.wave_x*ratio;
  A.wave_y = A.wave_y* invratio + B.wave_y*ratio;
  A.wave_mystery = A.wave_mystery* invratio + B.wave_mystery*ratio;
  
  A.ob_size = A.ob_size* invratio + B.ob_size*ratio;
  A.ob_r = A.ob_r* invratio + B.ob_r*ratio;
  A.ob_g = A.ob_g* invratio + B.ob_g*ratio;
  A.ob_b = A.ob_b* invratio + B.ob_b*ratio;
  A.ob_a = A.ob_a* invratio + B.ob_a*ratio;
  
  A.ib_size = A.ib_size* invratio + B.ib_size*ratio;
  A.ib_r = A.ib_r* invratio + B.ib_r*ratio;
  A.ib_g = A.ib_g* invratio + B.ib_g*ratio;
  A.ib_b = A.ib_b* invratio + B.ib_b*ratio;
  A.ib_a = A.ib_a* invratio + B.ib_a*ratio;
  
  A.mv_a  = A.mv_a* invratio + B.mv_a*ratio;
  A.mv_r  = A.mv_r* invratio + B.mv_r*ratio;
  A.mv_g  = A.mv_g* invratio + B.mv_g*ratio;
  A.mv_b  = A.mv_b* invratio + B.mv_b*ratio;
  A.mv_l = A.mv_l* invratio + B.mv_l*ratio;
  A.mv_x = A.mv_x* invratio + B.mv_x*ratio;
  A.mv_y = A.mv_y* invratio + B.mv_y*ratio;
  A.mv_dy = A.mv_dy* invratio + B.mv_dy*ratio;
  A.mv_dx = A.mv_dx* invratio + B.mv_dx*ratio;
  
  /* PER_FRAME VARIABLES END */
  
  A.fRating = A.fRating* invratio + B.fRating*ratio;
  A.fGammaAdj = A.fGammaAdj* invratio + B.fGammaAdj*ratio;
  A.fVideoEchoZoom = A.fVideoEchoZoom* invratio + B.fVideoEchoZoom*ratio;
  A.fVideoEchoAlpha = A.fVideoEchoAlpha* invratio + B.fVideoEchoAlpha*ratio;
  
  if (ratio > 0.5)
    {
      A.nVideoEchoOrientation = B.nVideoEchoOrientation;
      A.nWaveMode = B.nWaveMode;
      A.bAdditiveWaves = B.bAdditiveWaves;
      A.bWaveDots = B.bWaveDots;
      A.bWaveThick = B.bWaveThick;
      A.bModWaveAlphaByVolume = B.bModWaveAlphaByVolume;
      A.bMaximizeWaveColor = B.bMaximizeWaveColor;
      A.bTexWrap = B.bTexWrap;
      A.bDarkenCenter = B.bDarkenCenter;
      A.bRedBlueStereo = B.bRedBlueStereo;
      A.bBrighten = B.bBrighten;
      A.bDarken = B.bDarken;
      A.bSolarize = B.bSolarize;
      A.bInvert = B.bInvert;
      A.bMotionVectorsOn = B.bMotionVectorsOn;
    }
  
  A.fWaveAlpha = A.fWaveAlpha* invratio + B.fWaveAlpha*ratio;
  A.fWaveScale = A.fWaveScale* invratio + B.fWaveScale*ratio;
  A.fWaveSmoothing = A.fWaveSmoothing* invratio + B.fWaveSmoothing*ratio;
  A.fWaveParam = A.fWaveParam* invratio + B.fWaveParam*ratio;
  A.fModWaveAlphaStart = A.fModWaveAlphaStart* invratio + B.fModWaveAlphaStart*ratio;
  A.fModWaveAlphaEnd = A.fModWaveAlphaEnd * invratio + B.fModWaveAlphaEnd *ratio;
  A.fWarpAnimSpeed = A.fWarpAnimSpeed* invratio + B.fWarpAnimSpeed*ratio;
  A.fWarpScale = A.fWarpScale* invratio + B.fWarpScale*ratio;
  A.fShader = A.fShader* invratio + B.fShader*ratio;   

  return;
}

void PresetMerger::mergeMesh(bool & isMeshA, bool & isMeshB, float & varA, float & varB,  float** meshA, float** meshB, int gx, int gy, float ratio)
{
  float invratio = 1.0 - ratio;

  if (!isMeshA && !isMeshB)
    {
      varA = varA * invratio + varB * ratio;
    }
  else if(isMeshA && !isMeshB)
    {
      for (int x=0;x<gx;x++)
	{
	  for(int y=0;y<gy;y++)
	    {
	      meshA[x][y]=meshA[x][y] * ratio + varB * invratio;
	    }
	}
    }
 else if(isMeshA && isMeshB)
    {
      for (int x=0;x<gx;x++)
	{
	  for(int y=0;y<gy;y++)
	    {
	      meshA[x][y]=meshA[x][y] * ratio + meshB[x][y] * invratio;
	    }
	}
    }
  else
    {
      for (int x=0;x<gx;x++)
	{
	  for(int y=0;y<gy;y++)
	    {
	      meshA[x][y]=varA * ratio + meshB[x][y] * invratio;
	    }
	}

      isMeshA = true;
    }
}
