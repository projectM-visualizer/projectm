#include "Renderer.hpp"
#include "wipemalloc.h"
#include "math.h"
#include "Common.hpp"
#include "KeyHandler.hpp"
#include "TextureManager.hpp"
#include <iostream>
#include <algorithm>
#include <sys/stat.h>
#include <cassert>
#include "omptl/omptl"
#include "omptl/omptl_algorithm"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


Pipeline* Renderer::currentPipe;


class Preset;

Renderer::Renderer(int width, int height, int gx, int gy, BeatDetect *_beatDetect, std::string _presetURL,
        std::string _titlefontURL, std::string _menufontURL) :
    title_fontURL(_titlefontURL), menu_fontURL(_menufontURL), presetURL(_presetURL), m_presetName("None"), vw(width),
            vh(height), mesh(gx, gy)
{
    this->totalframes = 1;
    this->noSwitch = false;
    this->showfps = false;
    this->showtitle = false;
    this->showpreset = false;
    this->showhelp = false;
    this->showstats = false;
    this->studio = false;
    this->realfps = 0;

    this->drawtitle = 0;

    //this->title = "Unknown";

    /** Other stuff... */
    this->correction = true;

    /// @bug put these on member init list
    this->textureManager = NULL;
    this->beatDetect = _beatDetect;

    textureRenderToTexture = 0;

#ifdef USE_FTGL
    /** Load the standard fonts if they do exist */
    struct stat buffer;

    if (stat( title_fontURL.c_str(), &buffer )) {
      std::cout << "Could not open font file: " << title_fontURL << std::endl;
      exit(1);
    }
    if (stat( menu_fontURL.c_str(), &buffer )) {
      std::cout << "Could not open font file: " << menu_fontURL << std::endl;
      exit(1);
    }

    title_font = new FTGLPixmapFont(title_fontURL.c_str());
    other_font = new FTGLPixmapFont(menu_fontURL.c_str());
    poly_font = new FTGLExtrdFont(title_fontURL.c_str());

    if(title_font->Error()) {
        fprintf(stderr, "Failed to open font %s\n", title_fontURL.c_str());
    } else {
        title_font->UseDisplayList(true);
    }

    other_font->UseDisplayList(true);

    if(poly_font->Error()) {
        fprintf(stderr, "Failed to open font %s\n", title_fontURL.c_str());
    } else {
        poly_font->UseDisplayList(true);
        poly_font->Depth(20);
        poly_font->FaceSize(72);
    }

#endif /** USE_FTGL */


    int size = (mesh.height - 1) *mesh.width * 4 * 2;
    p = ( float * ) wipemalloc ( size * sizeof ( float ) );


    for (int j = 0; j < mesh.height - 1; j++)
    {
        int base = j * mesh.width * 4 * 2;


        for (int i = 0; i < mesh.width; i++)
        {
            int index = j * mesh.width + i;
            int index2 = (j + 1) * mesh.width + i;

            int strip = base + i * 8;
            p[strip + 0] = mesh.identity[index].x;
            p[strip + 1] = mesh.identity[index].y;

            p[strip + 4] = mesh.identity[index2].x;
            p[strip + 5] = mesh.identity[index2].y;
        }
    }

    renderContext.programID_v2f_c4f = shaderEngine.programID_v2f_c4f;
    renderContext.programID_v2f_c4f_t2f = shaderEngine.programID_v2f_c4f_t2f;

    // Interpolation VAO/VBO's
    glGenBuffers(1, &m_vbo_Interpolation);
    glGenVertexArrays(1, &m_vao_Interpolation);

    glBindVertexArray(m_vao_Interpolation);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_Interpolation);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float)*4, (void*)0); // Positions

    glDisableVertexAttribArray(1);

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float)*4, (void*)(sizeof(float)*2)); // Textures

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // CompositeOutput VAO/VBO's
    glGenBuffers(1, &m_vbo_CompositeOutput);
    glGenVertexArrays(1, &m_vao_CompositeOutput);

    float composite_buffer_data[8][2] =
    {
        { -0.5, -0.5 },
        { 0, 1 },
        { -0.5, 0.5 },
        { 0, 0 },
        { 0.5, 0.5 },
        { 1, 0 },
        { 0.5, -0.5 },
        { 1, 1 } };


    glBindVertexArray(m_vao_CompositeOutput);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_CompositeOutput);

    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 8 * 2, composite_buffer_data, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float)*4, (void*)0); // Positions

    glDisableVertexAttribArray(1);

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float)*4, (void*)(sizeof(float)*2)); // Textures

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);



    // CompositeShaderOutput VAO/VBO's
    glGenBuffers(1, &m_vbo_CompositeShaderOutput);
    glGenVertexArrays(1, &m_vao_CompositeShaderOutput);

    glBindVertexArray(m_vao_CompositeShaderOutput);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_CompositeShaderOutput);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(composite_shader_vertex), (void*)0); // Positions

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(composite_shader_vertex), (void*)(sizeof(float)*2)); // Colors

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(composite_shader_vertex), (void*)(sizeof(float)*6)); // UV

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(composite_shader_vertex), (void*)(sizeof(float)*8)); // RAD ANG

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Renderer::SetPipeline(Pipeline &pipeline)
{
    currentPipe = &pipeline;
    shaderEngine.reset();
    shaderEngine.loadPresetShaders(pipeline);
}

void Renderer::ResetTextures()
{
    textureManager->Clear();

    reset(vw, vh);

    textureManager->Preload();
}

void Renderer::SetupPass1(const Pipeline &pipeline, const PipelineContext &pipelineContext)
{
    totalframes++;
    glViewport(0, 0, texsizeX, texsizeY);

    renderContext.mat_ortho = glm::ortho(0.0f, 1.0f, 0.0f, 1.0f, -40.0f, 40.0f);
}

void Renderer::RenderItems(const Pipeline &pipeline, const PipelineContext &pipelineContext)
{
    renderContext.time = pipelineContext.time;
    renderContext.texsize = nearestPower2(std::max(texsizeX, texsizeY));
    renderContext.aspectCorrect = correction;
    renderContext.aspectRatio = aspect;
    renderContext.textureManager = textureManager;
    renderContext.beatDetect = beatDetect;

    for (std::vector<RenderItem*>::const_iterator pos = pipeline.drawables.begin(); pos != pipeline.drawables.end(); ++pos)
    {
      if (*pos != NULL)
      {
        (*pos)->Draw(renderContext);
      }
    }
}

void Renderer::FinishPass1()
{
    draw_title_to_texture();

    textureManager->updateMainTexture();
}

void Renderer::Pass2(const Pipeline &pipeline, const PipelineContext &pipelineContext)
{
    //BEGIN PASS 2
    //
    //end of texture rendering
    //now we copy the texture from the FBO or framebuffer to
    //video texture memory and render fullscreen.

    /** Reset the viewport size */
    if (textureRenderToTexture)
    {
        glViewport(0, 0, texsizeX, texsizeY);
    }
    else
        glViewport(0, 0, this->vw, this->vh);

    if (shaderEngine.enableCompositeShader(currentPipe->compositeShader, pipeline, pipelineContext)) {
        CompositeShaderOutput(pipeline, pipelineContext);
    } else {
        CompositeOutput(pipeline, pipelineContext);
    }

/* FTGL does not support OpenGL ES
#ifndef EMSCRIPTEN
    glMatrixMode(GL_MODELVIEW);
#endif
    glLoadIdentity();
    glTranslatef(-0.5, -0.5, 0);

    // When console refreshes, there is a chance the preset has been changed by the user
    refreshConsole();
    draw_title_to_screen(false);
    if (this->showhelp % 2)
        draw_help();
    if (this->showtitle % 2)
        draw_title();
    if (this->showfps % 2)
        draw_fps(this->realfps);
    if (this->showpreset % 2)
        draw_preset();
    if (this->showstats % 2)
        draw_stats();
    glTranslatef(0.5, 0.5, 0);
*/

    if (textureRenderToTexture) {
        glBindTexture(GL_TEXTURE_2D, textureRenderToTexture);
        glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, texsizeX, texsizeY);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

void Renderer::RenderFrame(const Pipeline &pipeline, const PipelineContext &pipelineContext)
{
    shaderEngine.RenderBlurTextures(pipeline, pipelineContext);

    SetupPass1(pipeline, pipelineContext);

    Interpolation(pipeline, pipelineContext);

    RenderItems(pipeline, pipelineContext);

    FinishPass1();

    Pass2(pipeline, pipelineContext);
}

void Renderer::Interpolation(const Pipeline &pipeline, const PipelineContext &pipelineContext)
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureManager->getMainTexture()->texID);

    //Texture wrapping( clamp vs. wrap)
    if (pipeline.textureWrap == 0)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
    else
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }

    int size = (mesh.height - 1) * mesh.width * 4 * 2;

    if (pipeline.staticPerPixel)
    {
        for (int j = 0; j < mesh.height - 1; j++)
        {
            int base = j * mesh.width * 2 * 4;

            for (int i = 0; i < mesh.width; i++)
            {
                int strip = base + i * 8;
                p[strip + 2] = pipeline.x_mesh[i][j];
                p[strip + 3] = pipeline.y_mesh[i][j];

                p[strip + 6] = pipeline.x_mesh[i][j+1];
                p[strip + 7] = pipeline.y_mesh[i][j+1];
            }
        }

    }
    else
    {
        mesh.Reset();
        omptl::transform(mesh.p.begin(), mesh.p.end(), mesh.identity.begin(), mesh.p.begin(), &Renderer::PerPixel);

        for (int j = 0; j < mesh.height - 1; j++)
        {
            int base = j * mesh.width * 2 * 4;

            for (int i = 0; i < mesh.width; i++)
            {
                int strip = base + i * 8;
                int index = j * mesh.width + i;
                int index2 = (j + 1) * mesh.width + i;

                p[strip + 2] = mesh.p[index].x;
                p[strip + 3] = mesh.p[index].y;

                p[strip + 6] = mesh.p[index2].x;
                p[strip + 7] = mesh.p[index2].y;


            }
        }
    }

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_Interpolation);

    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * size, NULL, GL_DYNAMIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * size, p, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    shaderEngine.enableWarpShader(currentPipe->warpShader, pipeline, pipelineContext, renderContext.mat_ortho);

    glVertexAttrib4f(1, 1.0, 1.0, 1.0, pipeline.screenDecay);

    glBlendFunc(GL_SRC_ALPHA, GL_ZERO);

    glBindVertexArray(m_vao_Interpolation);

    for (int j = 0; j < mesh.height - 1; j++)
        glDrawArrays(GL_TRIANGLE_STRIP,j* mesh.width* 2,mesh.width*2);

    glBindVertexArray(0);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindTexture(GL_TEXTURE_2D, 0);
}

Renderer::~Renderer()
{

    if (textureManager)
        delete (textureManager);

    //std::cerr << "grid assign end" << std::endl;

    free(p);

#ifdef USE_FTGL
    //	std::cerr << "freeing title fonts" << std::endl;
    if (title_font)
        delete title_font;
    if (poly_font)
        delete poly_font;
    if (other_font)
        delete other_font;
    //	std::cerr << "freeing title fonts finished" << std::endl;
#endif
    //	std::cerr << "exiting destructor" << std::endl;

    glDeleteBuffers(1, &m_vbo_Interpolation);
    glDeleteVertexArrays(1, &m_vao_Interpolation);

    glDeleteBuffers(1, &m_vbo_CompositeOutput);
    glDeleteVertexArrays(1, &m_vao_CompositeOutput);

    glDeleteTextures(1, &textureRenderToTexture);
}

void Renderer::reset(int w, int h)
{
    aspect = (float) h / (float) w;
    this -> vw = w;
    this -> vh = h;

    glCullFace(GL_BACK);

#ifndef GL_TRANSITION
    glEnable(GL_LINE_SMOOTH);
#endif

    glClearColor(0, 0, 0, 0);

    glViewport(0, 0, w, h);

    glEnable(GL_BLEND);

    texsizeX = w;
    texsizeY = h;

    // snap to 16x16 blocks
    texsizeX = ((texsizeX+15)/16)*16;
    texsizeY = ((texsizeY+15)/16)*16;

    m_fAspectX = (texsizeY > texsizeX) ? (float)texsizeX/(float)texsizeY : 1.0f;
    m_fAspectY = (texsizeX > texsizeY) ? (float)texsizeY/(float)texsizeX : 1.0f;
    m_fInvAspectX = 1.0f/m_fAspectX;
    m_fInvAspectY = 1.0f/m_fAspectY;

    InitCompositeShaderVertex();

    if (textureManager != NULL) {
        delete textureManager;
    }
    textureManager = new TextureManager(presetURL, texsizeX, texsizeY);

    shaderEngine.setParams(texsizeX, texsizeY, beatDetect, textureManager);
    shaderEngine.reset();
    shaderEngine.loadPresetShaders(*currentPipe);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClear(GL_COLOR_BUFFER_BIT);
}

GLuint Renderer::initRenderToTexture()
{
    if (textureRenderToTexture == 0) {
        glGenTextures(1, &textureRenderToTexture);
        glBindTexture(GL_TEXTURE_2D, textureRenderToTexture);
        glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, texsizeX, texsizeY, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    return textureRenderToTexture;
}

void Renderer::draw_title_to_texture()
{
#ifdef USE_FTGL
    if (this->drawtitle > 100)
    {
        draw_title_to_screen(true);
        this->drawtitle = 0;
    }
#endif /** USE_FTGL */
}

float title_y;

void Renderer::draw_title_to_screen(bool flip)
{

#ifdef USE_FTGL
    if (this->drawtitle > 0)
    {

        glEnable(GL_DEPTH_TEST);
        glClear(GL_DEPTH_BUFFER_BIT);

        int draw;
        if (drawtitle >= 80)
            draw = 80;
        else
            draw = drawtitle;

        float easein = ((80 - draw) * .0125);
        float easein2 = easein * easein;

        if (drawtitle == 1)
        {
            title_y = (float) rand() / RAND_MAX;
            title_y *= 2;
            title_y -= 1;
            title_y *= .6;
        }
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        //glBlendFunc(GL_SRC_ALPHA_SATURATE,GL_ONE);
        glColor4f(1.0, 1.0, 1.0, 1.0);

        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();

        glFrustum(-1, 1, -1 * (float) vh / (float) vw, 1 * (float) vh / (float) vw, 1, 1000);
        if (flip)
            glScalef(1, -1, 1);
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        glTranslatef(-850, title_y * 850 * vh / vw, easein2 * 900 - 900);

        glRotatef(easein2 * 360, 1, 0, 0);

        poly_font->Render(this->title.c_str());

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        this->drawtitle++;

        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();

        glMatrixMode(GL_MODELVIEW);

        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);

        glDisable(GL_COLOR_MATERIAL);

        glDisable(GL_LIGHTING);
        glDisable(GL_POLYGON_SMOOTH);
    }
#endif /** USE_FTGL */
}

void Renderer::draw_title()
{
#ifdef USE_FTGL
    //glBlendFunc(GL_ONE_MINUS_DST_COLOR,GL_ZERO);

    glColor4f(1.0, 1.0, 1.0, 1.0);
    //  glPushMatrix();
    //  glTranslatef(this->vw*.001,this->vh*.03, -1);
    //  glScalef(this->vw*.015,this->vh*.025,0);

    glRasterPos2f(0.01, 0.05);
    title_font->FaceSize((unsigned) (20 * (this->vh / 512.0)));

    title_font->Render(this->title.c_str());
    //  glPopMatrix();
    //glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

#endif /** USE_FTGL */
}

void Renderer::draw_preset()
{
#ifdef USE_FTGL
    //glBlendFunc(GL_ONE_MINUS_DST_COLOR,GL_ZERO);

    glColor4f(1.0, 1.0, 1.0, 1.0);
    //      glPushMatrix();
    //glTranslatef(this->vw*.001,this->vh*-.01, -1);
    //glScalef(this->vw*.003,this->vh*.004,0);


    glRasterPos2f(0.01, 0.01);

    title_font->FaceSize((unsigned) (12 * (this->vh / 512.0)));
    if (this->noSwitch)
        title_font->Render("[LOCKED]  ");
    title_font->FaceSize((unsigned) (20 * (this->vh / 512.0)));

    title_font->Render(this->presetName().c_str());

    //glPopMatrix();
    // glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
#endif /** USE_FTGL */
}

void Renderer::draw_help()
{

#ifdef USE_FTGL
    //glBlendFunc(GL_ONE_MINUS_DST_COLOR,GL_ZERO);

    glColor4f(1.0, 1.0, 1.0, 1.0);
    glPushMatrix();
    glTranslatef(0, 1, 0);
    //glScalef(this->vw*.02,this->vh*.02 ,0);


    title_font->FaceSize((unsigned) (18 * (this->vh / 512.0)));

    glRasterPos2f(0.01, -0.05);
    title_font->Render("Help");

    glRasterPos2f(0.01, -0.09);
    title_font->Render("----------------------------");

    glRasterPos2f(0.01, -0.13);
    title_font->Render("F1: This help menu");

    glRasterPos2f(0.01, -0.17);
    title_font->Render("F2: Show song title");

    glRasterPos2f(0.01, -0.21);
    title_font->Render("F3: Show preset name");

    glRasterPos2f(0.01, -0.25);
    title_font->Render("F4: Show Rendering Settings");

    glRasterPos2f(0.01, -0.29);
    title_font->Render("F5: Show FPS");

    glRasterPos2f(0.01, -0.35);
    title_font->Render("F: Fullscreen");

    glRasterPos2f(0.01, -0.39);
    title_font->Render("L: Lock/Unlock Preset");

    glRasterPos2f(0.01, -0.43);
    title_font->Render("M: Show Menu");

    glRasterPos2f(0.01, -0.49);
    title_font->Render("R: Random preset");
    glRasterPos2f(0.01, -0.53);
    title_font->Render("N: Next preset");

    glRasterPos2f(0.01, -0.57);
    title_font->Render("P: Previous preset");

    glPopMatrix();
    //         glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

#endif /** USE_FTGL */
}

void Renderer::draw_stats()
{

#ifdef USE_FTGL
    char buffer[128];
    float offset = (this->showfps % 2 ? -0.05 : 0.0);
    // glBlendFunc(GL_ONE_MINUS_DST_COLOR,GL_ZERO);

    glColor4f(1.0, 1.0, 1.0, 1.0);
    glPushMatrix();
    glTranslatef(0.01, 1, 0);
    glRasterPos2f(0, -.05 + offset);
    other_font->Render(this->correction ? "  aspect: corrected" : "  aspect: stretched");
    sprintf(buffer, " (%f)", this->aspect);
    other_font->Render(buffer);

    glRasterPos2f(0, -.09 + offset);
    other_font->FaceSize((unsigned) (18 * (vh / 512.0)));

    sprintf(buffer, "       texsize: %d", renderTarget->texsize);
    other_font->Render(buffer);

    glRasterPos2f(0, -.13 + offset);
    sprintf(buffer, "      viewport: %d x %d", vw, vh);

    other_font->Render(buffer);
    glRasterPos2f(0, -.17 + offset);
    other_font->Render((renderTarget->useFBO ? "           FBO: on" : "           FBO: off"));

    glRasterPos2f(0, -.21 + offset);
    sprintf(buffer, "          mesh: %d x %d", mesh.width, mesh.height);
    other_font->Render(buffer);

    glRasterPos2f(0, -.25 + offset);
    sprintf(buffer, "      textures: %.1fkB", textureManager->getTextureMemorySize() / 1000.0f);
    other_font->Render(buffer);

    glRasterPos2f(0, -.29 + offset);
    sprintf(buffer, "shader profile: %s", shaderEngine.profileName.c_str());
    other_font->Render(buffer);

    glRasterPos2f(0, -.33 + offset);
    sprintf(buffer, "   warp shader: %s", currentPipe->warpShader.enabled ? "on" : "off");
    other_font->Render(buffer);

    glRasterPos2f(0, -.37 + offset);
    sprintf(buffer, "   comp shader: %s", currentPipe->compositeShader.enabled ? "on" : "off");
    other_font->Render(buffer);

    glPopMatrix();
    // glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);


#endif /** USE_FTGL */
}
void Renderer::draw_fps(float realfps)
{
#ifdef USE_FTGL
    char bufferfps[20];
    sprintf(bufferfps, "%.1f fps", realfps);
    // glBlendFunc(GL_ONE_MINUS_DST_COLOR,GL_ZERO);

    glColor4f(1.0, 1.0, 1.0, 1.0);
    glPushMatrix();
    glTranslatef(0.01, 1, 0);
    glRasterPos2f(0, -0.05);
    title_font->FaceSize((unsigned) (20 * (this->vh / 512.0)));
    title_font->Render(bufferfps);

    glPopMatrix();
    // glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

#endif /** USE_FTGL */
}

void Renderer::CompositeOutput(const Pipeline &pipeline, const PipelineContext &pipelineContext)
{

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureManager->getMainTexture()->texID);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    renderContext.mat_ortho = glm::ortho(-0.5f, 0.5f, -0.5f, 0.5f, -40.0f, 40.0f);

    shaderEngine.enableCompositeShader(currentPipe->compositeShader, pipeline, pipelineContext);

    glUniformMatrix4fv(ShaderEngine::Uniform_V2F_C4F_T2F_VertexTranformation(), 1, GL_FALSE, glm::value_ptr(renderContext.mat_ortho));
    glUniform1i(ShaderEngine::Uniform_V2F_C4F_T2F_FragTextureSampler(), 0);

    //Overwrite anything on the screen
    glBlendFunc(GL_ONE, GL_ZERO);
    glVertexAttrib4f(1, 1.0, 1.0, 1.0, 1.0);

    glBindVertexArray(m_vao_CompositeOutput);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    glBindVertexArray(0);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for (std::vector<RenderItem*>::const_iterator pos = pipeline.compositeDrawables.begin(); pos
            != pipeline.compositeDrawables.end(); ++pos)
        (*pos)->Draw(renderContext);

    glBindTexture(GL_TEXTURE_2D, 0);
}

/**
 * Calculates the nearest power of two to the given number using the
 * appropriate rule
 */
int Renderer::nearestPower2( int value ) {

    int x = value;
    int power = 0;

    while ( ( x & 0x01 ) != 1 ) {
        x >>= 1;
    }

    if ( x == 1 ) {
        return value;
    } else {
        x = value;
        while ( x != 0 ) {
            x >>= 1;
            power++;
          }
          if ( ( ( 1 << power ) - value ) <= ( value - ( 1 << ( power - 1 ) ) ) ) {
            return 1 << power;
          } else {
            return 1 << ( power - 1 );
          }
    }
    return 0;
}

float Renderer::SquishToCenter(float x, float fExp)
{
    if (x > 0.5f)
        return powf(x*2-1, fExp)*0.5f + 0.5f;

    return (1-powf(1-x*2, fExp))*0.5f;
}

void Renderer::UvToMathSpace(float u, float v, float* rad, float* ang)
{
    // (screen space = -1..1 on both axes; corresponds to UV space)
    // uv space = [0..1] on both axes
    // "math" space = what the preset authors are used to:
    //      upper left = [0,0]
    //      bottom right = [1,1]
    //      rad == 1 at corners of screen
    //      ang == 0 at three o'clock, and increases counter-clockwise (to 6.28).
    float px = (u*2-1) * m_fAspectX;  // probably 1.0
    float py = (v*2-1) * m_fAspectY;  // probably <1

    *rad = sqrtf(px*px + py*py) / sqrtf(m_fAspectX*m_fAspectX + m_fAspectY*m_fAspectY);
    *ang = atan2f(py, px);
    if (*ang < 0)
        *ang += 6.2831853071796f;
}

void Renderer::InitCompositeShaderVertex() {

    // BUILD VERTEX LIST for final composite blit
    //   note the +0.5-texel offset!
    //   (otherwise, a 1-pixel-wide line of the image would wrap at the top and left edges).
    memset(m_comp_verts, 0, sizeof(composite_shader_vertex)*FCGSX*FCGSY);

    float fHalfTexelW =  0.5f / (float)vw;   // 2.5: 2 pixels bad @ bottom right
    float fHalfTexelH =  0.5f / (float)vh;
    float fDivX = 1.0f / (float)(FCGSX-2);
    float fDivY = 1.0f / (float)(FCGSY-2);
    for (int j=0; j<FCGSY; j++)
    {
        int j2 = j - j/(FCGSY/2);
        float v = j2*fDivY;
        v = SquishToCenter(v, 3.0f);
        float sy = -((v-fHalfTexelH)*2-1);//fOnePlusInvHeight*v*2-1;
        for (int i=0; i<FCGSX; i++)
        {
            int i2 = i - i/(FCGSX/2);
            float u = i2*fDivX;
            u = SquishToCenter(u, 3.0f);
            float sx = (u-fHalfTexelW)*2-1;//fOnePlusInvWidth*u*2-1;
            composite_shader_vertex* pComp = &m_comp_verts[i + j*FCGSX];
            pComp->x = sx;
            pComp->y = sy;

            float rad, ang;
            UvToMathSpace( u, v, &rad, &ang );
                // fix-ups:
               if (i==FCGSX/2-1) {
                   if (j < FCGSY/2-1)
                       ang = 3.1415926535898f*1.5f;
                   else if (j == FCGSY/2-1)
                       ang = 3.1415926535898f*1.25f;
                   else if (j == FCGSY/2)
                       ang = 3.1415926535898f*0.75f;
                   else
                       ang = 3.1415926535898f*0.5f;
               }
               else if (i==FCGSX/2) {
                   if (j < FCGSY/2-1)
                       ang = 3.1415926535898f*1.5f;
                   else if (j == FCGSY/2-1)
                       ang = 3.1415926535898f*1.75f;
                   else if (j == FCGSY/2)
                       ang = 3.1415926535898f*0.25f;
                   else
                       ang = 3.1415926535898f*0.5f;
               }
               else if (j==FCGSY/2-1) {
                   if (i < FCGSX/2-1)
                       ang = 3.1415926535898f*1.0f;
                   else if (i == FCGSX/2-1)
                       ang = 3.1415926535898f*1.25f;
                   else if (i == FCGSX/2)
                       ang = 3.1415926535898f*1.75f;
                   else
                       ang = 3.1415926535898f*2.0f;
               }
               else if (j==FCGSY/2) {
                   if (i < FCGSX/2-1)
                       ang = 3.1415926535898f*1.0f;
                   else if (i == FCGSX/2-1)
                       ang = 3.1415926535898f*0.75f;
                   else if (i == FCGSX/2)
                       ang = 3.1415926535898f*0.25f;
                   else
                       ang = 3.1415926535898f*0.0f;
               }
            pComp->tu = u;
            pComp->tv = v;

            pComp->rad = rad;
            pComp->ang = ang;
        }
    }

    // build index list for final composite blit -
    // order should be friendly for interpolation of 'ang' value!
    int* cur_index = &m_comp_indices[0];
    for (int y=0; y<FCGSY-1; y++)
    {
        if (y==FCGSY/2-1)
            continue;
        for (int x=0; x<FCGSX-1; x++)
        {
            if (x==FCGSX/2-1)
                continue;
            bool left_half = (x < FCGSX/2);
            bool top_half  = (y < FCGSY/2);
            bool center_4 = ((x==FCGSX/2 || x==FCGSX/2-1) && (y==FCGSY/2 || y==FCGSY/2-1));

            if ( ((int)left_half + (int)top_half + (int)center_4) % 2 )
            {
                *(cur_index+0) = (y  )*FCGSX + (x  );
                *(cur_index+1) = (y  )*FCGSX + (x+1);
                *(cur_index+2) = (y+1)*FCGSX + (x  );
                *(cur_index+3) = (y+1)*FCGSX + (x+1);
            }
            else
            {
                *(cur_index+0) = (y+1)*FCGSX + (x  );
                *(cur_index+1) = (y  )*FCGSX + (x  );
                *(cur_index+2) = (y+1)*FCGSX + (x+1);
                *(cur_index+3) = (y  )*FCGSX + (x+1);
            }
            cur_index += 4;
        }
    }
}


void Renderer::CompositeShaderOutput(const Pipeline &pipeline, const PipelineContext &pipelineContext)
{
    // hue shader
    float shade[4][3] = {
        { 1.0f, 1.0f, 1.0f },
        { 1.0f, 1.0f, 1.0f },
        { 1.0f, 1.0f, 1.0f },
        { 1.0f, 1.0f, 1.0f } };  // for each vertex, then each comp.

    // pick 4 colors for the 4 corners
    for (int i=0; i<4; i++)
    {
        shade[i][0] = 0.6f + 0.3f*sinf(pipelineContext.time*30.0f*0.0143f + 3 + i*21);
        shade[i][1] = 0.6f + 0.3f*sinf(pipelineContext.time*30.0f*0.0107f + 1 + i*13);
        shade[i][2] = 0.6f + 0.3f*sinf(pipelineContext.time*30.0f*0.0129f + 6 + i*9 );
        float max = ((shade[i][0] > shade[i][1]) ? shade[i][0] : shade[i][1]);
        if (shade[i][2] > max) max = shade[i][2];
        for (int k=0; k<3; k++)
        {
            shade[i][k] /= max;
            shade[i][k] = 0.5f + 0.5f*shade[i][k];
        }
    }

    // interpolate the 4 colors & apply to all the verts
    for (int j=0; j<FCGSY; j++)
    {
        for (int i=0; i<FCGSX; i++)
        {
            composite_shader_vertex* pComp = &m_comp_verts[i + j*FCGSX];
            float x = pComp->x*0.5f + 0.5f;
            float y = pComp->y*0.5f + 0.5f;

            float col[3] = { 1, 1, 1 };
            for (int c=0; c<3; c++)
                col[c] = shade[0][c]*(  x)*(  y) +
                         shade[1][c]*(1-x)*(  y) +
                         shade[2][c]*(  x)*(1-y) +
                         shade[3][c]*(1-x)*(1-y);

            pComp->Diffuse[0] = col[0];
            pComp->Diffuse[1] = col[1];
            pComp->Diffuse[2] = col[2];
            pComp->Diffuse[3] = 1.0;
        }
    }


    int primCount = (FCGSX-2)*(FCGSY-2)*4;
    composite_shader_vertex tempv[primCount];
    memset(tempv, 0, sizeof(composite_shader_vertex) * primCount);
    int src_idx = 0;
    for (int i=0; i<primCount; i++)
    {
        tempv[i] = m_comp_verts[ m_comp_indices[src_idx++] ];
    }

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_CompositeShaderOutput);

    glBufferData(GL_ARRAY_BUFFER, sizeof(composite_shader_vertex) * primCount, NULL, GL_DYNAMIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, sizeof(composite_shader_vertex) * primCount, tempv, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBlendFunc(GL_ONE, GL_ZERO);

    glBindVertexArray(m_vao_CompositeShaderOutput);

    // Now do the final composite blit, fullscreen;
    glDrawArrays(GL_TRIANGLE_STRIP, 0, primCount);

    glBindVertexArray(0);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


