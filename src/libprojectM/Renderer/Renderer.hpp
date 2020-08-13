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
#include <iostream>
#include <chrono>
#include <ctime>

using namespace std::chrono;

#ifdef USE_TEXT_MENU

#define GLT_IMPLEMENTATION
#define GLT_DEBUG_PRINT
#include "gltext.h"

#endif /** USE_TEXT_MENU */

#define TOAST_TIME 2

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
class TimeKeeper;

class Renderer
{

public:
  bool showtoast;
  bool showfps;
  bool showtitle;
  bool showpreset;
  bool showhelp;
  bool showmenu;
  bool showstats;

  bool studio;
  bool correction;

  bool noSwitch;

  struct preset {
    int id;
    std::string name, presetPack;
  };


  milliseconds lastTimeFPS;
  milliseconds currentTimeFPS;

  milliseconds lastTimeToast;
  milliseconds currentTimeToast;

  std::string m_helpText;

  int totalframes;
  float realfps;

  std::string title;
  int m_activePresetID;
  std::vector<preset> m_presetList;

  int drawtitle;
  int texsizeX;
  int texsizeY;
  int textMenuPageSize = 10;
  int textMenuLineHeight = 25;
  int textMenuYOffset = 60;
  float m_fAspectX;
  float m_fAspectY;
  float m_fInvAspectX;
  float m_fInvAspectY;

  Renderer(int width, int height, int gx, int gy, BeatDetect *_beatDetect, std::string presetURL, std::string title_fontURL, std::string menu_fontURL, const std::string& datadir = "");
  ~Renderer();
  
  void RenderFrame(const Pipeline &pipeline, const PipelineContext &pipelineContext);
  void RenderFrameOnlyPass1(const Pipeline &pipeline, const PipelineContext &pipelineContext);
  void RenderFrameOnlyPass2(const Pipeline &pipeline, const PipelineContext &pipelineContext,int xoffset,int yoffset,int eye);
  void ResetTextures();
  void reset(int w, int h);
  GLuint initRenderToTexture();

  bool timeCheck(const milliseconds currentTime, const milliseconds lastTime, const double difference);

  std::string SetPipeline(Pipeline &pipeline);

  void setPresetName(const std::string& theValue)
  {
    m_presetName = theValue;
  }

  std::string presetName() const
  {
    return m_presetName;
  }

  void setHelpText(const std::string& theValue) {
		m_helpText = theValue; 
  }

  std::string helpText() const {
		return m_helpText;
  }

  void setFPS(const int &theValue) {
		m_fps = std::to_string(theValue); 
  }

  std::string fps() const {
		return m_fps;
  }

  milliseconds nowMilliseconds() {
		return duration_cast<milliseconds>(system_clock::now().time_since_epoch());;
  }

  void setToastMessage(const std::string& theValue);

  std::string toastMessage() const {
    return m_toastMessage;
  }
  
private:

  PerPixelMesh mesh;
  BeatDetect *beatDetect;
  TextureManager *textureManager;
  static Pipeline* currentPipe;
  TimeKeeper *timeKeeperFPS;
  TimeKeeper *timeKeeperToast;

#ifdef USE_TEXT_MENU

  void drawText(GLTtext* text, const char* string, GLfloat x, GLfloat y, GLfloat scale, int horizontalAlignment,
                   int verticalAlignment, float r, float b, float g, float a);
  void drawText(const char* string, GLfloat x, GLfloat y, GLfloat scale, int horizontalAlignment,
                   int verticalAlignment, float r, float b, float g, float a);

#endif /** USE_TEXT_MENU */
  RenderContext renderContext;
  //per pixel equation variables
  ShaderEngine shaderEngine;
  std::string m_presetName;
  std::string m_datadir;
  std::string m_fps;
  std::string m_toastMessage;

  float* p;

  int vstartx; /* view start x position - normally 0, but could be different if doing a subset of the window - like
                  for virtual reality */
  int vstarty; /* view start y position - normally 0, but could be different if doing a subset of the window - like
                  for virtual reality */
	       /* these are currently set only for rendering to the screen, not to the textbuffer */
		  
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

#ifdef USE_TEXT_MENU
  GLTtext *title_font;
  GLTtext *other_font;
  GLTtext *poly_font;
#endif /** USE_TEXT_MENU */

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

  void draw_toast();
  void draw_fps();
  void draw_stats();
  void draw_help();
  void draw_menu();
  void draw_preset();
  void draw_title();
  void draw_title_to_screen(bool flip);
  void draw_title_to_texture();
  
  std::string float_stats(float stat);
  
int nearestPower2( int value );

  GLuint textureRenderToTexture;

  void InitCompositeShaderVertex();
  float SquishToCenter(float x, float fExp);
  void UvToMathSpace(float u, float v, float* rad, float* ang);
  composite_shader_vertex    m_comp_verts[FCGSX*FCGSY];
  int         m_comp_indices[(FCGSX-2)*(FCGSY-2)*6];

};

#endif
