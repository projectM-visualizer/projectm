#include <algorithm>
#include <vector>
#include <memory>

#include "projectM-opengl.h"

#include "SOIL2/SOIL2.h"

#ifdef WIN32
#include "win32-dirent.h"
#endif

#ifdef __unix__
#include <dirent.h>
#endif
#ifdef EMSCRIPTEN
#include <dirent.h>
#endif

#ifdef __APPLE__
#include <dirent.h>
#endif
#include "TextureManager.hpp"
#include "Common.hpp"
#include "IdleTextures.hpp"
#include "Texture.hpp"
#include "PerlinNoise.hpp"


#define NUM_BLUR_TEX    6


TextureManager::TextureManager(const std::string _presetsURL, const int texsizeX, const int texsizeY, std::string datadir):
    presetsURL(_presetsURL)
{
    extensions.push_back(".jpg");
    extensions.push_back(".dds");
    extensions.push_back(".png");
    extensions.push_back(".tga");
    extensions.push_back(".bmp");
    extensions.push_back(".dib");

    Preload();
    // if not data directory specified from user code
    // we use the built-in default directory (unix prefix based)
    if (datadir.empty())
#ifdef WIN32
		datadir = ".";
#else
		datadir = DATADIR_PATH;
#endif /** WIN32 */

    loadTextureDir(datadir + "/presets");
    loadTextureDir(datadir + "/textures");
    loadTextureDir(_presetsURL);

    // Create main texture ans associated samplers
    mainTexture = new Texture("main", texsizeX, texsizeY, false);
    mainTexture->getSampler(GL_REPEAT, GL_LINEAR);
    mainTexture->getSampler(GL_REPEAT, GL_NEAREST);
    mainTexture->getSampler(GL_CLAMP_TO_EDGE, GL_LINEAR);
    mainTexture->getSampler(GL_CLAMP_TO_EDGE, GL_NEAREST);
    textures["main"] = mainTexture;

    // Initialize blur textures
    int w = texsizeX;
    int h = texsizeY;
    for (int i=0; i<NUM_BLUR_TEX; i++)
    {
        // main VS = 1024
        // blur0 = 512
        // blur1 = 256  <-  user sees this as "blur1"
        // blur2 = 128
        // blur3 = 128  <-  user sees this as "blur2"
        // blur4 =  64
        // blur5 =  64  <-  user sees this as "blur3"
        if (!(i&1) || (i<2))
        {
            w = std::max(16, w/2);
            h = std::max(16, h/2);
        }
        int w2 = ((w+3)/16)*16;
        int h2 = ((h+3)/4)*4;

        std::string texname = "blur" + std::to_string(i/2+1) + ((i%2) ? "" : "doNOTuseME");
        Texture * textureBlur = new Texture(texname, w2, h2, false);
        textureBlur->getSampler(GL_CLAMP_TO_EDGE, GL_LINEAR);
        textures[texname] = textureBlur;
        blurTextures.push_back(textureBlur);
    }

    std::unique_ptr<PerlinNoise> noise(new PerlinNoise());

    GLuint noise_texture_lq_lite;
    glGenTextures(1, &noise_texture_lq_lite);
    glBindTexture(GL_TEXTURE_2D, noise_texture_lq_lite);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 32, 32, 0, GL_RGB, GL_FLOAT, noise->noise_lq_lite);
    Texture * textureNoise_lq_lite = new Texture("noise_lq_lite", noise_texture_lq_lite, GL_TEXTURE_2D, 32, 32, false);
    textureNoise_lq_lite->getSampler(GL_REPEAT, GL_LINEAR);
    textures["noise_lq_lite"] = textureNoise_lq_lite;

    GLuint noise_texture_lq;
    glGenTextures(1, &noise_texture_lq);
    glBindTexture(GL_TEXTURE_2D, noise_texture_lq);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGB, GL_FLOAT, noise->noise_lq);
    Texture * textureNoise_lq = new Texture("noise_lq", noise_texture_lq, GL_TEXTURE_2D, 256, 256, false);
    textureNoise_lq->getSampler(GL_REPEAT, GL_LINEAR);
    textures["noise_lq"] = textureNoise_lq;

    GLuint noise_texture_mq;
    glGenTextures(1, &noise_texture_mq);
    glBindTexture(GL_TEXTURE_2D, noise_texture_mq);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGB, GL_FLOAT, noise->noise_mq);
    Texture * textureNoise_mq = new Texture("noise_mq", noise_texture_mq, GL_TEXTURE_2D, 256, 256, false);
    textureNoise_mq->getSampler(GL_REPEAT, GL_LINEAR);
    textures["noise_mq"] = textureNoise_mq;

    GLuint noise_texture_hq;
    glGenTextures(1, &noise_texture_hq);
    glBindTexture(GL_TEXTURE_2D, noise_texture_hq);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGB, GL_FLOAT, noise->noise_hq);
    Texture * textureNoise_hq = new Texture("noise_hq", noise_texture_hq, GL_TEXTURE_2D, 256, 256, false);
    textureNoise_hq->getSampler(GL_REPEAT, GL_LINEAR);
    textures["noise_hq"] = textureNoise_hq;

    GLuint noise_texture_lq_vol;
    glGenTextures( 1, &noise_texture_lq_vol );
    glBindTexture( GL_TEXTURE_3D, noise_texture_lq_vol );
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, 32 ,32 ,32 ,0 ,GL_RGB ,GL_FLOAT ,noise->noise_lq_vol);
    Texture * textureNoise_lq_vol = new Texture("noisevol_lq", noise_texture_lq_vol, GL_TEXTURE_3D, 32, 32, false);
    textureNoise_lq_vol->getSampler(GL_REPEAT, GL_LINEAR);
    textures["noisevol_lq"] = textureNoise_lq_vol;

    GLuint noise_texture_hq_vol;
    glGenTextures( 1, &noise_texture_hq_vol );
    glBindTexture( GL_TEXTURE_3D, noise_texture_hq_vol );
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, 32, 32, 32, 0, GL_RGB, GL_FLOAT, noise->noise_hq_vol);
    Texture * textureNoise_hq_vol = new Texture("noisevol_hq", noise_texture_hq_vol, GL_TEXTURE_3D, 32, 32, false);
    textureNoise_hq_vol->getSampler(GL_REPEAT, GL_LINEAR);
    textures["noisevol_hq"] = textureNoise_hq_vol;
}

TextureManager::~TextureManager()
{
    Clear();
}

void TextureManager::Preload()
{
    int width, height;

    unsigned int tex = SOIL_load_OGL_texture_from_memory(
                M_data,
                M_bytes,
                SOIL_LOAD_AUTO,
                SOIL_CREATE_NEW_ID,
                SOIL_FLAG_POWER_OF_TWO
                |  SOIL_FLAG_MULTIPLY_ALPHA
                ,&width,&height);


    Texture * newTex = new Texture("M", tex, GL_TEXTURE_2D, width, height, true);
    newTex->getSampler(GL_CLAMP_TO_EDGE, GL_LINEAR);
    textures["M"] = newTex;

    //    tex = SOIL_load_OGL_texture_from_memory(
    //                project_data,
    //                project_bytes,
    //                SOIL_LOAD_AUTO,
    //                SOIL_CREATE_NEW_ID,
    //                SOIL_FLAG_POWER_OF_TWO
    //                |  SOIL_FLAG_MULTIPLY_ALPHA
    //                ,&width,&height);

    //    newTex = new Texture("project", tex, GL_TEXTURE_2D, width, height, true);
    //    newTex->getSampler(GL_CLAMP_TO_EDGE, GL_LINEAR);
    //    textures["project"] = newTex;

    tex = SOIL_load_OGL_texture_from_memory(
                headphones_data,
                headphones_bytes,
                SOIL_LOAD_AUTO,
                SOIL_CREATE_NEW_ID,
                SOIL_FLAG_POWER_OF_TWO
                |  SOIL_FLAG_MULTIPLY_ALPHA
                ,&width,&height);

    newTex = new Texture("headphones", tex, GL_TEXTURE_2D, width, height, true);
    newTex->getSampler(GL_CLAMP_TO_EDGE, GL_LINEAR);
    textures["headphones"] = newTex;
}

void TextureManager::Clear()
{
    for(std::map<std::string, Texture*>::const_iterator iter = textures.begin(); iter != textures.end(); iter++)
        delete(iter->second);

    textures.clear();
}


TextureSamplerDesc TextureManager::getTexture(const std::string fullName, const GLenum defaultWrap, const GLenum defaultFilter)
{
    std::string fileName = fullName;
    std::string unqualifiedName;
    GLint wrap_mode;
    GLint filter_mode;

    // Remove extension
    std::string lowerCaseFileName(fullName);
    std::transform(lowerCaseFileName.begin(), lowerCaseFileName.end(), lowerCaseFileName.begin(), tolower);
    for (size_t x = 0; x < extensions.size(); x++)
    {
        size_t found = lowerCaseFileName.find(extensions[x]);
        if (found != std::string::npos)
        {
            fileName.replace(int(found), extensions[x].size(), "");
            break;
        }
    }

    ExtractTextureSettings(fileName, wrap_mode, filter_mode, unqualifiedName);
    if (textures.find(unqualifiedName) == textures.end())
    {
        return TextureSamplerDesc(NULL, NULL);
    }

    if (fileName == unqualifiedName) {
        // Warp and filter mode not specified in sampler name
        // applying default
        wrap_mode = defaultWrap;
        filter_mode = defaultFilter;
    }

    Texture * texture = textures[unqualifiedName];
    Sampler * sampler = texture->getSampler(wrap_mode, filter_mode);

    return TextureSamplerDesc(texture, sampler);
}


TextureSamplerDesc TextureManager::tryLoadingTexture(const std::string name)
{
    TextureSamplerDesc texDesc;
    GLint wrap_mode;
    GLint filter_mode;
    std::string unqualifiedName;

    ExtractTextureSettings(name, wrap_mode, filter_mode, unqualifiedName);

    for (size_t x = 0; x < extensions.size(); x++)
    {
        std::string filename = unqualifiedName + extensions[x];
        std::string fullURL = presetsURL + PATH_SEPARATOR + filename;

        texDesc = loadTexture(name, fullURL);

        if (texDesc.first != NULL)
        {
            break;
        }
    }

    return texDesc;
}

TextureSamplerDesc TextureManager::loadTexture(const std::string name, const std::string fileName)
{
    int width, height;

    unsigned int tex = SOIL_load_OGL_texture(
                fileName.c_str(),
                SOIL_LOAD_AUTO,
                SOIL_CREATE_NEW_ID,
                SOIL_FLAG_MULTIPLY_ALPHA
                ,&width,&height);

    if (tex == 0)
    {
        return TextureSamplerDesc(NULL, NULL);
    }

    GLint wrap_mode;
    GLint filter_mode;
    std::string unqualifiedName;

    ExtractTextureSettings(name, wrap_mode, filter_mode, unqualifiedName);
    Texture * newTexture = new Texture(unqualifiedName, tex, GL_TEXTURE_2D, width, height, true);
    Sampler * sampler = newTexture->getSampler(wrap_mode, filter_mode);

    if (textures.find(name) != textures.end()) {
        delete textures[name];
    }

    textures[name] = newTexture;

    return TextureSamplerDesc(newTexture, sampler);
}


void TextureManager::loadTextureDir(const std::string &dirname)
{
    DIR * m_dir;

    // Allocate a new a stream given the current directory name
    if ((m_dir = opendir(dirname.c_str())) == NULL)
    {
        std::cout<<"No Textures Loaded from "<<dirname<<std::endl;
        return; // no files loaded. m_entries is empty
    }

    struct dirent * dir_entry;

    while ((dir_entry = readdir(m_dir)) != NULL)
    {
        // Convert char * to friendly string
        std::string filename(dir_entry->d_name);

        if (filename.length() > 0 && filename[0] == '.')
            continue;

        std::string lowerCaseFileName(filename);
        std::transform(lowerCaseFileName.begin(), lowerCaseFileName.end(), lowerCaseFileName.begin(), tolower);

        // Remove extension
        for (size_t x = 0; x < extensions.size(); x++)
        {
            size_t found = lowerCaseFileName.find(extensions[x]);
            if (found != std::string::npos)
            {
                std::string name = filename;
                name.replace(int(found), extensions[x].size(), "");

                // Create full path name
                std::string fullname = dirname + PATH_SEPARATOR + filename;
                loadTexture(name, fullname);

                break;
            }
        }
    }

    if (m_dir)
    {
        closedir(m_dir);
        m_dir = 0;
    }
}

TextureSamplerDesc TextureManager::getRandomTextureName(std::string random_id)
{
    GLint wrap_mode;
    GLint filter_mode;
    std::string unqualifiedName;

    ExtractTextureSettings(random_id, wrap_mode, filter_mode, unqualifiedName);

    std::vector<std::string> user_texture_names;
    size_t separator = unqualifiedName.find("_");
    std::string textureNameFilter;

    if (separator != std::string::npos)
    {
        textureNameFilter = unqualifiedName.substr(separator+1);
        unqualifiedName = unqualifiedName.substr(0, separator);
    }

    for(std::map<std::string, Texture*>::const_iterator iter = textures.begin(); iter != textures.end(); iter++)
    {
        if (iter->second->userTexture) {
            if (textureNameFilter.empty() || iter->first.find(textureNameFilter) == 0)
                user_texture_names.push_back(iter->first);
        }
    }

    if (user_texture_names.size() > 0)
    {
        std::string random_name = user_texture_names[rand() % user_texture_names.size()];
        random_textures.push_back(random_id);

        Texture * randomTexture = new Texture(*textures[random_name]);
        Sampler * sampler = randomTexture->getSampler(wrap_mode, filter_mode);
        randomTexture->name = unqualifiedName;
        textures[random_id] = randomTexture;

        return TextureSamplerDesc(randomTexture, sampler);
    }

    return TextureSamplerDesc(NULL, NULL);
}

void TextureManager::clearRandomTextures()
{
    for (std::vector<std::string>::iterator pos = random_textures.begin(); pos	!= random_textures.end(); ++pos)
    {
        textures.erase(*pos);
    }
    random_textures.clear();

}

void TextureManager::ExtractTextureSettings(const std::string qualifiedName, GLint & _wrap_mode, GLint & _filter_mode, std::string & name)
{
    std::string lowerQualifiedName(qualifiedName);
    std::transform(lowerQualifiedName.begin(), lowerQualifiedName.end(), lowerQualifiedName.begin(), tolower);

    _wrap_mode = GL_REPEAT;
    _filter_mode = GL_LINEAR;

    if (lowerQualifiedName.substr(0,3) == "fc_")
    {
        name = qualifiedName.substr(3);
        _filter_mode = GL_LINEAR;
        _wrap_mode = GL_CLAMP_TO_EDGE;
    }
    else if (lowerQualifiedName.substr(0,3) == "fw_")
    {
        name = qualifiedName.substr(3);
        _filter_mode = GL_LINEAR;
        _wrap_mode = GL_REPEAT;
    }
    else if (lowerQualifiedName.substr(0,3) == "pc_")
    {
        name = qualifiedName.substr(3);
        _filter_mode = GL_NEAREST;
        _wrap_mode = GL_CLAMP_TO_EDGE;
    }
    else if (lowerQualifiedName.substr(0,3) == "pw_")
    {
        name = qualifiedName.substr(3);
        _filter_mode = GL_NEAREST;
        _wrap_mode = GL_REPEAT;
    }
    else
    {
        name = qualifiedName;
    }
}

const Texture * TextureManager::getMainTexture() const {
    return mainTexture;
}

const std::vector<Texture*> & TextureManager::getBlurTextures() const {
    return blurTextures;
}


void TextureManager::updateMainTexture()
{
    glBindTexture(GL_TEXTURE_2D, mainTexture->texID);
    glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, mainTexture->width, mainTexture->height);
    glBindTexture(GL_TEXTURE_2D, 0);
}
