#ifndef Renderer_HPP
#define Renderer_HPP

#include "BeatDetect.hpp"
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
  struct preset {
    int id;
    std::string name;
    std::string presetPack;
  };

  Renderer() = delete;
  Renderer(int width, int height, int gx, int gy, BeatDetect* beatDetect, const std::vector<std::string>& textureSearchPaths);
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

    bool showfps{ false };
    bool showtitle{ false };
    bool showpreset{ false };
    bool showhelp{ false };
    bool showsearch{ false };
    bool showmenu{ false };
    bool showstats{ false };

    bool shuffletrack{ false };

    bool correction{ true };

    bool noSwitch{ false };
    bool writeNextFrameToFile;

    milliseconds lastTimeFPS{ nowMilliseconds() };
    milliseconds currentTimeFPS{ nowMilliseconds() };

    int totalframes{ 1 };
    float realfps{ 0.0 };

    std::string title;
    int m_activePresetID{ 0 };
    std::vector<preset> m_presetList;

    int drawtitle{ 0 };
    int textMenuPageSize{ 10 };

private:
    void SetupPass1(const Pipeline& pipeline, const PipelineContext& pipelineContext);
    void Interpolation(const Pipeline& pipeline, const PipelineContext& pipelineContext);
    void RenderItems(const Pipeline& pipeline, const PipelineContext& pipelineContext);
    void RenderTouch(const Pipeline& pipeline, const PipelineContext& pipelineContext);
    void FinishPass1();
    void Pass2(const Pipeline& pipeline, const PipelineContext& pipelineContext);
    void CompositeShaderOutput(const Pipeline& pipeline, const PipelineContext& pipelineContext);
    void CompositeOutput(const Pipeline& pipeline, const PipelineContext& pipelineContext);

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

    int nearestPower2(int value);

    GLuint textureRenderToTexture{0};

    void InitCompositeShaderVertex();
    float SquishToCenter(float x, float fExp);
    void UvToMathSpace(float u, float v, float* rad, float* ang);

    PerPixelMesh m_perPixelMesh;
    BeatDetect* m_beatDetect{nullptr};
    std::unique_ptr<TextureManager> m_textureManager;
    Pipeline* m_currentPipeline{nullptr};
    RenderContext m_renderContext;
    ShaderEngine m_shaderEngine;

    std::string m_presetName;
    std::string m_fps;
    std::string m_toastMessage;
    std::string m_searchText;

    float* m_perPointMeshBuffer{nullptr};

    /**
       * @brief View start x position
       *
       * <p>Normally 0, but could be different if doing a subset of the window - like for virtual reality</p>
       */
    int m_viewStartX{0};

    /**
       * @brief View start y position
       *
       * <p>Normally 0, but could be different if doing a subset of the window - like
       * for virtual reality. These are currently set only for rendering to the screen, not to the textbuffer.</p>
      */
    int m_viewStartY{0};

    int m_viewportWidth{0};
    int m_viewportHeight{0};

    const std::vector<std::string>& m_textureSearchPaths; ///!< List of paths to search for texture files

    GLuint m_vboInterpolation{0};
    GLuint m_vaoInterpolation{0};

    GLuint m_vboCompositeOutput{0};
    GLuint m_vaoCompositeOutput{0};

    GLuint m_vboCompositeShaderOutput{0};
    GLuint m_vaoCompositeShaderOutput{0};

#ifdef USE_TEXT_MENU
    MenuText m_menuText;
#endif /** USE_TEXT_MENU */

    composite_shader_vertex m_compositeVertices[FCGSX * FCGSY];
    int m_compositeIndices[(FCGSX - 2) * (FCGSY - 2) * 6];

    float m_touchX{0.0}; ///!< X position for touch waveform to start displaying(scale of 0 - 1 and not the exact coordinates)
    float m_touchY{0.0}; ///!< y position for touch waveform to start displaying(scale of 0 - 1 and not the exact coordinates)
    double m_touchR{0.0};///!< Red
    double m_touchG{0.0};///!< Green
    double m_touchB{0.0};///!< Blue
    double m_touchA{0.0};///!< Alpha

    bool m_showToast{false};

    milliseconds m_lastTimeToast{nowMilliseconds()};
    milliseconds m_currentTimeToast{nowMilliseconds()};

    std::string m_helpText;

    std::vector<MilkdropWaveform> m_waveformList;

    int m_textureSizeX{0};
    int m_textureSizeY{0};

    int m_textMenuLineHeight{25};
    const int m_textMenuYOffset{60};

    float m_fAspectX{1.0};
    float m_fAspectY{1.0};
    float m_fInvAspectX{1.0};
    float m_fInvAspectY{1.0};
};

#endif
