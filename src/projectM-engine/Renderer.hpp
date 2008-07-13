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

  void SetPipeline(Pipeline &pipeline);

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

  std::string title_fontURL;
  std::string menu_fontURL;
  std::string presetURL;

#ifdef USE_CG

  std::string cgTemplate;

  unsigned int noise_texture_lq_lite;
  unsigned int noise_texture_lq;
  unsigned int noise_texture_mq;
  unsigned int noise_texture_hq;
  unsigned int noise_texture_lq_vol;
  unsigned int noise_texture_hq_vol;

  float rand_preset[4];

  bool warpShadersEnabled;
  bool compositeShadersEnabled;

  CGcontext   myCgContext;
  CGprofile   myCgProfile;
  CGprogram   myCgWarpProgram,
                   myCgCompositeProgram;

 bool LoadCgProgram(std::string program, CGprogram &p);
 bool checkForCgCompileError(const char *situation);
 void checkForCgError(const char *situation);
 void SetupCg();
 void SetupCgVariables(CGprogram program, const Pipeline &pipeline, const PipelineContext &pipelineContext);
 void SetupCgQVariables(CGprogram program, const PresetOutputs &presetOutputs);
#endif

#ifdef USE_FTGL
  FTGLPixmapFont *title_font;
  FTGLPixmapFont *other_font;
  FTGLExtrdFont *poly_font;
#endif /** USE_FTGL */



  void SetupPass1(const Pipeline* pipeline, const PipelineContext &pipelineContext);
  void SetupShaders(const Pipeline* pipeline, const PipelineContext &pipelineContext);
  void Interpolation(const Pipeline* pipeline);
  void RenderItems(const Pipeline* pipeline, const PipelineContext &pipelineContext);
  void CompositeOutput(const Pipeline* pipeline, const PipelineContext &pipelineContext);
  void FinishPass1();
  void Pass2 (const Pipeline* pipeline, const PipelineContext &pipelineContext);

  inline static Point PerPixel(Point p, PerPixelContext &context)
  {
  return currentPipe->PerPixel(p,context);
  }

  void Interpolation(PresetOutputs *presetOutputs, PresetInputs *presetInputs);

  void rescale_per_pixel_matrices();

  void draw_fps( float realfps );
  void draw_stats();
  void draw_help();
  void draw_preset();
  void draw_title();
  void draw_title_to_screen(bool flip);
  void draw_title_to_texture();

};

#endif
