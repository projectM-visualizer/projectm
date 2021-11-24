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
#include "MilkdropWaveform.hpp"
#include "ShaderEngine.hpp"
#include <iostream>
#include <chrono>
#include <ctime>
#include <list>

using namespace std::chrono;

#ifdef USE_TEXT_MENU

#define GLT_IMPLEMENTATION
#define GLT_DEBUG_PRINT
#define __gl_h_  // gltext doesn't do a great job of noticing we included gl
#include "gltext.h"

#endif /** USE_TEXT_MENU */

#define TOAST_TIME 2
#define TOUCH_TIME 5

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
    /*  touchx(float) x for touch waveform to start displaying(scale of 0 - 1 and not the exact coordinates)
        touchy(float) y for touch waveform to start displaying(scale of 0 - 1 and not the exact coordinates)
        touchp(int) touch pressure - @TODO not implemented yet!
        touchtype(int) Waveform type (bias to Circle). 1 = Circle; 2 = RadialBlob; 3 = Blob2; 4 = Blob 3; 5 = DerivativeLine; 6 = Blob5; 7 = Line; 8 DoubleLine; 
        touchr(double) Red
        touchb(double) Blue
        touchg(double) Green
        toucha(double) Alpha
    */
  float touchx;
  float touchy;
  int touchp; // Touch Pressure.
  int touchtype; // Touch Type
  double touchr;
  double touchg;
  double touchb;
  double toucha;
  
  bool showtoast;
  bool showfps;
  bool showtitle;
  bool showpreset;
  bool showhelp;
  bool showsearch;
  bool showmenu;
  bool showstats;

  bool shuffletrack;

  bool studio;
  bool correction;

  bool noSwitch;
  bool writeNextFrameToFile;

  struct preset {
    int id;
    std::string name, presetPack;
  };


  milliseconds lastTimeFPS;
  milliseconds currentTimeFPS;

  milliseconds lastTimeToast;
  milliseconds currentTimeToast;

  std::string m_helpText;

  std::vector<MilkdropWaveform> waveformList;

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

  void toggleSearchText();
  void toggleInput();
  void touch(float x, float y, int pressure, int type);
  void touchDrag(float x, float y, int pressure);
  void touchDestroy(float x, float y);
  void touchDestroyAll();
  bool touchedWaveform(float x, float y, std::size_t i);

  void setToastMessage(const std::string& theValue);
  std::string getSearchText() const;
  void setSearchText(const std::string& theValue);
  void resetSearchText();
  void deleteSearchText();
  /// Writes the contents of current mainTexture in TextureManager to a bmp file
  void debugWriteMainTextureToFile() const;
  std::string toastMessage() const {
    return m_toastMessage;
  }

  std::string searchText() const {
    return m_searchText;
  }

  void UpdateContext(PipelineContext& context);

private:

  PerPixelMesh mesh;
  BeatDetect *beatDetect;
  TextureManager *textureManager;
  Pipeline* currentPipe;
  TimeKeeper *timeKeeperFPS;
  TimeKeeper *timeKeeperToast;

#ifdef USE_TEXT_MENU
  // draw text with search term a/k/a needle & highlight text
  void drawText(GLTtext* text, const char* string, const char* needle, GLfloat x, GLfloat y, GLfloat scale, int horizontalAlignment, int verticalAlignment, float r, float b, float g, float a, bool highlightable);
  void drawText(GLTtext* text, const char* string, GLfloat x, GLfloat y, GLfloat scale, int horizontalAlignment, int verticalAlignment, float r, float b, float g, float a, bool highlightable);
  void drawText(const char* string, GLfloat x, GLfloat y, GLfloat scale, int horizontalAlignment, int verticalAlignment, float r, float b, float g, float a, bool highlightable);
  bool textHighlightable(bool highlightable);

#endif /** USE_TEXT_MENU */
  RenderContext renderContext;
  //per pixel equation variables
  ShaderEngine shaderEngine;
  std::string m_presetName;
  std::string m_datadir;
  std::string m_fps;
  std::string m_toastMessage;
  std::string m_searchText;

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
  void RenderTouch(const Pipeline &pipeline, const PipelineContext &pipelineContext);
  void FinishPass1();
  void Pass2 (const Pipeline &pipeline, const PipelineContext &pipelineContext);
  void CompositeShaderOutput(const Pipeline &pipeline, const PipelineContext &pipelineContext);
  void CompositeOutput(const Pipeline &pipeline, const PipelineContext &pipelineContext);

  void rescale_per_pixel_matrices();

  void draw_toast();
  void draw_fps();
  void draw_stats();
  void draw_help();
  void draw_menu();
  void draw_preset();
  void draw_search();
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
