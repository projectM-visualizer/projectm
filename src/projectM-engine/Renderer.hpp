#ifndef Renderer_HPP
#define Renderer_HPP

#include "FBO.hpp"
#include "PresetFrameIO.hpp"
#include "BeatDetect.hpp"
#include <string>

#ifdef USE_GLES1
#include <GLES/gl.h>
#else
#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif
#endif

#ifdef USE_FTGL
#ifdef WIN32
#include <FTGL.h>
#include <FTGLPixmapFont.h>
#include <FTGLExtrdFont.h>
#else
#include <FTGL/FTGL.h>
#include <FTGL/FTGLPixmapFont.h>
#include <FTGL/FTGLExtrdFont.h>
#endif
#endif /** USE_FTGL */

#ifdef USE_CG
#include <Cg/cg.h>    /* Can't include this?  Is Cg Toolkit installed! */
#include <Cg/cgGL.h>
#endif

#include "Pipeline.hpp"
#include "PerPixelMesh.hpp"
#include "Transformation.hpp"

class BeatDetect;
class TextureManager;

class Renderer
{

public:

  bool showfps;
  bool showtitle;
  bool showpreset;
  bool showhelp;
  bool showstats;

  bool studio;
  bool correction;

  bool noSwitch;

  int totalframes;
  float realfps;

  std::string title;
  int drawtitle;
  int texsize;

  PerPixelMesh mesh;


  Renderer( int width, int height, int gx, int gy, int texsize,  BeatDetect *beatDetect, std::string presetURL, std::string title_fontURL, std::string menu_fontURL);
  ~Renderer();
  void RenderFrame(PresetOutputs *presetOutputs, PresetInputs *presetInputs);
  void RenderFrame(const Pipeline *pipeline, const PipelineContext &pipelineContext);
  void ResetTextures();
  void reset(int w, int h);
  GLuint initRenderToTexture();
  void PerPixelMath(PresetOutputs *presetOutputs,  PresetInputs *presetInputs);
  void WaveformMath(PresetOutputs *presetOutputs, PresetInputs *presetInputs, bool isSmoothing);

  void setPresetName(const std::string& theValue)
  {
    m_presetName = theValue;
  }

  std::string presetName() const
  {
    return m_presetName;
  }


private:

  RenderTarget *renderTarget;
  BeatDetect *beatDetect;
  TextureManager *textureManager;
  static Pipeline* currentPipe;
  RenderContext renderContext;
  //per pixel equation variables

  std::string m_presetName;


  float **gridx;  //grid containing interpolated mesh
    float **gridy;
    float **origx2;  //original mesh
    float **origy2;



  int vw;
  int vh;

  float aspect;

#ifdef USE_CG

  CGcontext   myCgContext;
 CGprofile   myCgProfile;

 CGprogram   myCgWarpProgram,
                   myCgCompositeProgram;


 std::string warpProgram;
 std::string compositeProgram;
 std::string shaderFile;


 void checkForCgError(const char *situation);
void SetupCg();
#endif

#ifdef USE_FTGL
  FTGLPixmapFont *title_font;
  FTGLPixmapFont *other_font;
  FTGLExtrdFont *poly_font;
#endif /** USE_FTGL */

  std::string title_fontURL;
  std::string menu_fontURL;
  std::string presetURL;



  void CompositeOutput(const Pipeline* pipeline);
  void Interpolation(const Pipeline* pipeline);
  inline static Point PerPixel(Point p, PerPixelContext &context)
  {
  return currentPipe->PerPixel(p,context);
  }

  void Interpolation(PresetOutputs *presetOutputs, PresetInputs *presetInputs);

  void draw_waveform(PresetOutputs * presetOutputs);
  void rescale_per_pixel_matrices();
  void maximize_colors(PresetOutputs *presetOutputs);
  void render_texture_to_screen(PresetOutputs *presetOutputs);
  void render_texture_to_studio(PresetOutputs *presetOutputs, PresetInputs *presetInputs);
  void draw_fps( float realfps );
  void draw_stats(PresetInputs *presetInputs);
  void draw_help( );
  void draw_preset();
  void draw_title();
  void draw_title_to_screen(bool flip);
  void maximize_colors();
  void draw_title_to_texture();
  void draw_motion_vectors(PresetOutputs *presetOutputs);
  void draw_borders(PresetOutputs *presetOutputs);
  void draw_shapes(PresetOutputs *presetOutputs);
  void draw_custom_waves(PresetOutputs *presetOutputs);

  void modulate_opacity_by_volume(PresetOutputs *presetOutputs) ;
  void darken_center();
};

#endif
