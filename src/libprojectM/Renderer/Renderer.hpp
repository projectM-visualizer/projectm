#ifndef Renderer_HPP
#define Renderer_HPP

#include "BeatDetect.hpp"
#include "Common.hpp"
#include "MilkdropWaveform.hpp"
#include "PerPixelMesh.hpp"
#include "Pipeline.hpp"
#include "ShaderEngine.hpp"
#include "Transformation.hpp"
#include "projectM-opengl.h"
#ifdef USE_TEXT_MENU
#include "MenuText.h"
#endif /** USE_TEXT_MENU */

#include <chrono>
#include <ctime>
#include <iostream>
#include <list>
#include <set>
#include <string>


using namespace std::chrono;

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
  float touchx{ 0.0 };
  float touchy{ 0.0 };
  int touchp{ 0 }; // Touch Pressure.
  int touchtype{ 0 }; // Touch Type
  double touchr{ 0.0 };
  double touchg{ 0.0 };
  double touchb{ 0.0 };
  double toucha{ 0.0 };
  
  bool showtoast{ false };
  bool showfps{ false };
  bool showtitle{ false };
  bool showpreset{ false };
  bool showhelp{ false };
  bool showsearch{ false };
  bool showmenu{ false };
  bool showstats{ false };

  bool shuffletrack{ false };

  bool studio{ false };
  bool correction{ true };

  bool noSwitch{ false };
  bool writeNextFrameToFile;

  struct preset {
    int id;
    std::string name;
    std::string presetPack;
  };


  milliseconds lastTimeFPS{ nowMilliseconds() };
  milliseconds currentTimeFPS{ nowMilliseconds() };

  milliseconds lastTimeToast{ nowMilliseconds() };
  milliseconds currentTimeToast{ nowMilliseconds() };

  std::string m_helpText;

  std::vector<MilkdropWaveform> waveformList;

  int totalframes{ 1 };
  float realfps{ 0.0 };

  std::string title;
  int m_activePresetID{ 0 };
  std::vector<preset> m_presetList;

  int drawtitle{ 0 };
  int texsizeX{ 0 };
  int texsizeY{ 0 };
  int textMenuPageSize{ 10 };
  int textMenuLineHeight{ 25 };
  int textMenuYOffset{ 60 };
  float m_fAspectX{ 1.0 };
  float m_fAspectY{ 1.0 };
  float m_fInvAspectX{ 1.0 };
  float m_fInvAspectY{ 1.0 };

  Renderer() = delete;
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
  BeatDetect *beatDetect{ nullptr };
  TextureManager *textureManager{ nullptr };
  Pipeline* currentPipe{ nullptr };

  RenderContext renderContext;
  //per pixel equation variables
  ShaderEngine shaderEngine;
  std::string m_presetName;
  std::string m_datadir;
  std::string m_fps;
  std::string m_toastMessage;
  std::string m_searchText;

  float* p{ nullptr };

  /**
   * @brief View start x position
   *
   * <p>Normally 0, but could be different if doing a subset of the window - like for virtual reality</p>
   */
  int vstartx{ 0 };

  /**
   * @brief View start y position
   *
   * <p>Normally 0, but could be different if doing a subset of the window - like
   * for virtual reality. These are currently set only for rendering to the screen, not to the textbuffer.</p>
  */
  int vstarty{ 0 };

  int vw{ 0 };
  int vh{ 0 };

  float aspect;

  std::string title_fontURL;
  std::string menu_fontURL;
  std::string presetURL;

  GLuint m_vbo_Interpolation{ 0 };
  GLuint m_vao_Interpolation{ 0 };

  GLuint m_vbo_CompositeOutput{ 0 };
  GLuint m_vao_CompositeOutput{ 0 };

  GLuint m_vbo_CompositeShaderOutput{ 0 };
  GLuint m_vao_CompositeShaderOutput{ 0 };

#ifdef USE_TEXT_MENU
  MenuText m_menuText;
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

  GLuint textureRenderToTexture{ 0 };

  void InitCompositeShaderVertex();
  float SquishToCenter(float x, float fExp);
  void UvToMathSpace(float u, float v, float* rad, float* ang);
  composite_shader_vertex    m_comp_verts[FCGSX*FCGSY];
  int         m_comp_indices[(FCGSX-2)*(FCGSY-2)*6];

};

#endif
