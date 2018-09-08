#ifndef Renderer_HPP
#define Renderer_HPP

#include "BeatDetect.hpp"
#include "Common.hpp"
#include <string>
#include <set>
#include "projectM-opengl.h"
#include "Pipeline.hpp"
#include "PerPixelMesh.hpp"
#include "Transformation.hpp"
#include "ShaderEngine.hpp"

#ifdef USE_FTGL
#ifdef WIN32
#include <ftgl.h>
#include <FTGLPixmapFont.h>
#include <FTGLExtrdFont.h>
#else
#include <FTGL/ftgl.h>
#endif
#endif /** USE_FTGL */

// for final composite grid:
#define FCGSX 32 // final composite gridsize - # verts - should be EVEN.
#define FCGSY 24 // final composite gridsize - # verts - should be EVEN.
                 // # of grid *cells* is two less,
                 // since we have redundant verts along the center line in X and Y (...for clean 'ang' interp)
typedef struct
{
    float x, y;     // screen position + Z-buffer depth
    float Diffuse[4];     // diffuse color
    float tu, tv;           // DYNAMIC
    float rad, ang;         // STATIC
} composite_shader_vertex;


class Texture;
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
  int texsizeX;
  int texsizeY;
  float m_fAspectX;
  float m_fAspectY;
  float m_fInvAspectX;
  float m_fInvAspectY;

  Renderer(int width, int height, int gx, int gy, BeatDetect *_beatDetect, std::string presetURL, std::string title_fontURL, std::string menu_fontURL);
  ~Renderer();

  void RenderFrame(const Pipeline &pipeline, const PipelineContext &pipelineContext);
  void ResetTextures();
  void reset(int w, int h);
  GLuint initRenderToTexture();


  std::string SetPipeline(Pipeline &pipeline);

  void setPresetName(const std::string& theValue)
  {
    m_presetName = theValue;
  }

  std::string presetName() const
  {
    return m_presetName;
  }

private:

	PerPixelMesh mesh;
  BeatDetect *beatDetect;
  TextureManager *textureManager;
  static Pipeline* currentPipe;
  RenderContext renderContext;
  //per pixel equation variables
  ShaderEngine shaderEngine;
  std::string m_presetName;

  float* p;


  int vw;
  int vh;

  float aspect;

  std::string title_fontURL;
  std::string menu_fontURL;
  std::string presetURL;

  GLuint m_vbo_Interpolation;
  GLuint m_vao_Interpolation;

  GLuint m_vbo_CompositeOutput;
  GLuint m_vao_CompositeOutput;

  GLuint m_vbo_CompositeShaderOutput;
  GLuint m_vao_CompositeShaderOutput;

#ifdef USE_FTGL
  FTGLPixmapFont *title_font;
  FTGLPixmapFont *other_font;
  FTGLExtrdFont *poly_font;
#endif /** USE_FTGL */

  void SetupPass1(const Pipeline &pipeline, const PipelineContext &pipelineContext);
  void Interpolation(const Pipeline &pipeline, const PipelineContext &pipelineContext);
  void RenderItems(const Pipeline &pipeline, const PipelineContext &pipelineContext);
  void FinishPass1();
  void Pass2 (const Pipeline &pipeline, const PipelineContext &pipelineContext);
  void CompositeShaderOutput(const Pipeline &pipeline, const PipelineContext &pipelineContext);
  void CompositeOutput(const Pipeline &pipeline, const PipelineContext &pipelineContext);

  inline static PixelPoint PerPixel(PixelPoint p, PerPixelContext &context)
  {
	  return currentPipe->PerPixel(p,context);
  }

  void rescale_per_pixel_matrices();

  void draw_fps( float realfps );
  void draw_stats();
  void draw_help();
  void draw_preset();
  void draw_title();
  void draw_title_to_screen(bool flip);
  void draw_title_to_texture();

  int nearestPower2( int value );

  GLuint textureRenderToTexture;

  void InitCompositeShaderVertex();
  float SquishToCenter(float x, float fExp);
  void UvToMathSpace(float u, float v, float* rad, float* ang);
  composite_shader_vertex    m_comp_verts[FCGSX*FCGSY];
  int         m_comp_indices[(FCGSX-2)*(FCGSY-2)*6];

};

#endif
