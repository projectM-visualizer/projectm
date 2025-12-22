#include "ProjectMCWrapper.hpp"

#include <projectM-4/projectM.h>

#include <Logging.hpp>

#include <Audio/AudioConstants.hpp>
#include <Renderer/PlatformGLResolver.hpp>

#include <projectM-4/parameters.h>
#include <projectM-4/render_opengl.h>

#include <cstring>
#include <sstream>

namespace libprojectM {

projectMWrapper::projectMWrapper(const std::shared_ptr<Renderer::Platform::GLResolver>& glResolver)
    : ProjectM(glResolver)
{
}

void projectMWrapper::PresetSwitchRequestedEvent(bool isHardCut) const
{
    if (m_presetSwitchRequestedEventCallback)
    {
        m_presetSwitchRequestedEventCallback(isHardCut, m_presetSwitchRequestedEventUserData);
    }
}

void projectMWrapper::PresetSwitchFailedEvent(const std::string& presetFilename,
                                              const std::string& failureMessage) const
{
    if (m_presetSwitchFailedEventCallback)
    {
        m_presetSwitchFailedEventCallback(presetFilename.c_str(),
                                          failureMessage.c_str(), m_presetSwitchFailedEventUserData);
    }
}

} // namespace libprojectM

libprojectM::projectMWrapper* handle_to_instance(projectm_handle instance)
{
    return reinterpret_cast<libprojectM::projectMWrapper*>(instance);
}

char* projectm_alloc_string(unsigned int length)
{
    try
    {
        return new char[length]{};
    }
    catch (...)
    {
        return nullptr;
    }
}

char* projectm_alloc_string_from_std_string(const std::string& str)
{
    auto pointer = projectm_alloc_string(static_cast<uint32_t>(str.length() + 1));
    if (pointer)
    {
        memcpy(pointer, str.c_str(), str.length());
    }
    return pointer;
}

void projectm_free_string(const char* str)
{
    delete[] str;
}

projectm_handle projectm_create()
{
    return projectm_create_with_opengl_load_proc(nullptr, nullptr);
}

projectm_handle projectm_create_with_opengl_load_proc(void* (*load_proc)(const char*, void*), void* user_data)
{
    try
    {
        // obtain shared resolver instance
        auto glResolver = libprojectM::Renderer::Platform::GLResolver::Instance();

        // init resolver to discover gl function pointers and init glad
        // Initialize() is guarded internally, may be executed multiple times
        auto success = glResolver->Initialize(load_proc, user_data);
        if (!success)
        {
            return nullptr;
        }

        // create projectM
        auto* projectMInstance = new libprojectM::projectMWrapper(glResolver);
        return reinterpret_cast<projectm_handle>(projectMInstance);
    }
    catch (...)
    {
        return nullptr;
    }
}

void projectm_destroy(projectm_handle instance)
{
    auto projectMInstance = handle_to_instance(instance);
    delete projectMInstance;
}

void projectm_load_preset_file(projectm_handle instance, const char* filename,
                               bool smooth_transition)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->LoadPresetFile(filename, smooth_transition);
}

void projectm_load_preset_data(projectm_handle instance, const char* data,
                               bool smooth_transition)
{
    std::stringstream presetDataStream(data);
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->LoadPresetData(presetDataStream, smooth_transition);
}

void projectm_set_preset_switch_requested_event_callback(projectm_handle instance,
                                                         projectm_preset_switch_requested_event callback, void* user_data)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->m_presetSwitchRequestedEventCallback = callback;
    projectMInstance->m_presetSwitchRequestedEventUserData = user_data;
}

void projectm_set_preset_switch_failed_event_callback(projectm_handle instance,
                                                      projectm_preset_switch_failed_event callback, void* user_data)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->m_presetSwitchFailedEventCallback = callback;
    projectMInstance->m_presetSwitchFailedEventUserData = user_data;
}

void projectm_set_texture_search_paths(projectm_handle instance,
                                       const char** texture_search_paths,
                                       size_t count)
{
    auto projectMInstance = handle_to_instance(instance);

    std::vector<std::string> texturePaths;

    for (size_t index = 0; index < count; index++)
    {
        texturePaths.emplace_back(texture_search_paths[index]);
    }

    projectMInstance->SetTexturePaths(texturePaths);
}

void projectm_reset_textures(projectm_handle instance)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->ResetTextures();
}

void projectm_get_version_components(int* major, int* minor, int* patch)
{
    if (major != nullptr)
    {
        *major = PROJECTM_VERSION_MAJOR;
    }
    if (minor != nullptr)
    {
        *minor = PROJECTM_VERSION_MINOR;
    }
    if (patch != nullptr)
    {
        *patch = PROJECTM_VERSION_PATCH;
    }
}

char* projectm_get_version_string()
{
    auto versionLength = strlen(PROJECTM_VERSION_STRING);
    auto buffer = projectm_alloc_string(static_cast<uint32_t>(versionLength + 1));
    strncpy(buffer, PROJECTM_VERSION_STRING, versionLength + 1);
    return buffer;
}

char* projectm_get_vcs_version_string()
{
    auto versionLength = strlen(PROJECTM_VERSION_VCS);
    auto buffer = projectm_alloc_string(static_cast<uint32_t>(versionLength + 1));
    strncpy(buffer, PROJECTM_VERSION_VCS, versionLength + 1);
    return buffer;
}

void projectm_opengl_render_frame(projectm_handle instance)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->RenderFrame();
}

void projectm_opengl_render_frame_fbo(projectm_handle instance, uint32_t framebuffer_object_id)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->RenderFrame(framebuffer_object_id);
}

void projectm_opengl_burn_texture(projectm_handle instance, uint32_t texture, int left, int top, int width, int height)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->BurnInTexture(texture, left, top, width, height);
}

void projectm_set_frame_time(projectm_handle instance, double seconds_since_first_frame)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->SetFrameTime(seconds_since_first_frame);
}

double projectm_get_last_frame_time(projectm_handle instance)
{
    auto projectMInstance = handle_to_instance(instance);
    return projectMInstance->GetFrameTime();
}

void projectm_set_beat_sensitivity(projectm_handle instance, float sensitivity)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->SetBeatSensitivity(sensitivity);
}

float projectm_get_beat_sensitivity(projectm_handle instance)
{
    auto projectMInstance = handle_to_instance(instance);
    return projectMInstance->GetBeatSensitivity();
}

double projectm_get_hard_cut_duration(projectm_handle instance)
{
    auto projectMInstance = handle_to_instance(instance);
    return projectMInstance->HardCutDuration();
}

void projectm_set_hard_cut_duration(projectm_handle instance, double seconds)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->SetHardCutDuration(seconds);
}

bool projectm_get_hard_cut_enabled(projectm_handle instance)
{
    auto projectMInstance = handle_to_instance(instance);
    return projectMInstance->HardCutEnabled();
}

void projectm_set_hard_cut_enabled(projectm_handle instance, bool enabled)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->SetHardCutEnabled(enabled);
}

float projectm_get_hard_cut_sensitivity(projectm_handle instance)
{
    auto projectMInstance = handle_to_instance(instance);
    return projectMInstance->HardCutSensitivity();
}

void projectm_set_hard_cut_sensitivity(projectm_handle instance, float sensitivity)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->SetHardCutSensitivity(sensitivity);
}

double projectm_get_soft_cut_duration(projectm_handle instance)
{
    auto projectMInstance = handle_to_instance(instance);
    return projectMInstance->SoftCutDuration();
}

void projectm_set_soft_cut_duration(projectm_handle instance, double seconds)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->SetSoftCutDuration(seconds);
}

double projectm_get_preset_duration(projectm_handle instance)
{
    auto projectMInstance = handle_to_instance(instance);
    return projectMInstance->PresetDuration();
}

void projectm_set_preset_duration(projectm_handle instance, double seconds)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->SetPresetDuration(seconds);
}

void projectm_get_mesh_size(projectm_handle instance, size_t* width, size_t* height)
{
    uint32_t w, h;
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->MeshSize(w, h);
    *width = static_cast<size_t>(w);
    *height = static_cast<size_t>(h);
}

void projectm_set_texel_offset(projectm_handle instance, float offset_X, float offset_y)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->SetTexelOffsets(offset_X, offset_y);
}

void projectm_get_texel_offset(projectm_handle instance, float* offset_X, float* offset_y)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->TexelOffsets(*offset_X, *offset_y);
}

void projectm_set_mesh_size(projectm_handle instance, size_t width, size_t height)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->SetMeshSize(static_cast<uint32_t>(width), static_cast<uint32_t>(height));
}

int32_t projectm_get_fps(projectm_handle instance)
{
    auto projectMInstance = handle_to_instance(instance);
    return projectMInstance->TargetFramesPerSecond();
}

void projectm_set_fps(projectm_handle instance, int32_t fps)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->SetTargetFramesPerSecond(fps);
}

void projectm_set_aspect_correction(projectm_handle instance, bool enabled)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->SetAspectCorrection(enabled);
}

bool projectm_get_aspect_correction(projectm_handle instance)
{
    auto projectMInstance = handle_to_instance(instance);
    return projectMInstance->AspectCorrection();
}

void projectm_set_easter_egg(projectm_handle instance, float value)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->SetEasterEgg(value);
}

float projectm_get_easter_egg(projectm_handle instance)
{
    auto projectMInstance = handle_to_instance(instance);
    return projectMInstance->EasterEgg();
}

void projectm_touch(projectm_handle instance, float x, float y, int pressure, projectm_touch_type touch_type)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->Touch(x, y, pressure, touch_type);
}

void projectm_touch_drag(projectm_handle instance, float x, float y, int pressure)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->TouchDrag(x, y, pressure);
}

void projectm_touch_destroy(projectm_handle instance, float x, float y)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->TouchDestroy(x, y);
}

void projectm_touch_destroy_all(projectm_handle instance)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->TouchDestroyAll();
}

bool projectm_get_preset_locked(projectm_handle instance)
{
    auto projectMInstance = handle_to_instance(instance);
    return projectMInstance->PresetLocked();
}

void projectm_set_preset_locked(projectm_handle instance, bool lock)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->SetPresetLocked(lock);
}

void projectm_get_window_size(projectm_handle instance, size_t* width, size_t* height)
{
    auto projectMInstance = handle_to_instance(instance);
    *width = static_cast<size_t>(projectMInstance->WindowWidth());
    *height = static_cast<size_t>(projectMInstance->WindowHeight());
}

void projectm_set_window_size(projectm_handle instance, size_t width, size_t height)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->SetWindowSize(static_cast<uint32_t>(width), static_cast<uint32_t>(height));
}

unsigned int projectm_pcm_get_max_samples()
{
    return libprojectM::Audio::WaveformSamples;
}

template<class BufferType>
static auto PcmAdd(projectm_handle instance, const BufferType* samples, unsigned int count, projectm_channels channels) -> void
{
    auto* projectMInstance = handle_to_instance(instance);

    projectMInstance->PCM().Add(samples, channels, count);
}

auto projectm_pcm_add_float(projectm_handle instance, const float* samples, unsigned int count, projectm_channels channels) -> void
{
    PcmAdd(instance, samples, count, channels);
}

auto projectm_pcm_add_int16(projectm_handle instance, const int16_t* samples, unsigned int count, projectm_channels channels) -> void
{
    PcmAdd(instance, samples, count, channels);
}

auto projectm_pcm_add_uint8(projectm_handle instance, const uint8_t* samples, unsigned int count, projectm_channels channels) -> void
{
    PcmAdd(instance, samples, count, channels);
}

auto projectm_write_debug_image_on_next_frame(projectm_handle, const char*) -> void
{
    // UNIMPLEMENTED
}

uint32_t projectm_sprite_create(projectm_handle instance, const char* type, const char* code)
{
    auto* projectMInstance = handle_to_instance(instance);

    return projectMInstance->AddUserSprite(type, code);
}

void projectm_sprite_destroy(projectm_handle instance, uint32_t sprite_id)
{
    auto* projectMInstance = handle_to_instance(instance);

    projectMInstance->DestroyUserSprite(sprite_id);
}

void projectm_sprite_destroy_all(projectm_handle instance)
{
    auto* projectMInstance = handle_to_instance(instance);

    projectMInstance->DestroyAllUserSprites();
}

uint32_t projectm_sprite_get_sprite_count(projectm_handle instance)
{
    auto* projectMInstance = handle_to_instance(instance);

    return projectMInstance->UserSpriteCount();
}

void projectm_sprite_get_sprite_ids(projectm_handle instance, uint32_t* sprite_ids)
{
    auto* projectMInstance = handle_to_instance(instance);

    auto spriteIdList = projectMInstance->UserSpriteIdentifiers();
    for (const auto& spriteId : spriteIdList)
    {
        *sprite_ids = spriteId;
        sprite_ids++;
    }
}

void projectm_sprite_set_max_sprites(projectm_handle instance, uint32_t max_sprites)
{
    auto* projectMInstance = handle_to_instance(instance);

    projectMInstance->SetUserSpriteLimit(max_sprites);
}

uint32_t projectm_sprite_get_max_sprites(projectm_handle instance)
{
    auto* projectMInstance = handle_to_instance(instance);

    return projectMInstance->UserSpriteLimit();
}

void projectm_set_log_callback(projectm_log_callback callback, bool current_thread_only, void* user_data)
{
    if (current_thread_only)
    {
        libprojectM::Logging::SetThreadCallback({reinterpret_cast<libprojectM::Logging::CallbackFunction>(callback), user_data});
    }
    else
    {
        libprojectM::Logging::SetGlobalCallback({reinterpret_cast<libprojectM::Logging::CallbackFunction>(callback), user_data});
    }
}

void projectm_set_log_level(projectm_log_level log_level, bool current_thread_only)
{
    if (current_thread_only)
    {
        libprojectM::Logging::SetThreadLogLevel(static_cast<libprojectM::Logging::LogLevel>(log_level));
    }
    else
    {
        libprojectM::Logging::SetGlobalLogLevel(static_cast<libprojectM::Logging::LogLevel>(log_level));
    }
}