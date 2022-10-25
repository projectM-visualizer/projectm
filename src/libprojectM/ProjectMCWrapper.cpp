#include "ProjectMCWrapper.hpp"

#include "projectM.h"

#include <cstring>
#include <utility>

projectMWrapper::projectMWrapper(std::string configFile, int flags)
    : ProjectM(std::move(configFile), static_cast<ProjectM::Flags>(flags))
{
}

projectMWrapper::projectMWrapper(class ProjectM::Settings settings, int flags)
    : ProjectM(std::move(settings), static_cast<ProjectM::Flags>(flags))
{
}

void projectMWrapper::PresetSwitchedEvent(bool isHardCut, size_t presetIndex) const
{
    if (_presetSwitchedEventCallback)
    {
        _presetSwitchedEventCallback(isHardCut, presetIndex, _presetSwitchedEventUserData);
    }
}

void projectMWrapper::ShuffleEnabledValueChanged(bool shuffle_enabled) const
{
    if (_shuffleEnableChangedEventCallback)
    {
        _shuffleEnableChangedEventCallback(shuffle_enabled, _shuffleEnableChangedEventUserData);
    }
}

void projectMWrapper::PresetSwitchFailedEvent(bool isHardCut, unsigned int presetIndex,
                                              const std::string& failureMessage) const
{
    if (_presetSwitchFailedEventCallback)
    {
        _presetSwitchFailedEventCallback(isHardCut, presetIndex,
                                         failureMessage.c_str(), _presetSwitchFailedEventUserData);
    }
}

void projectMWrapper::PresetRatingChanged(unsigned int presetIndex, int rating, PresetRatingType ratingType) const
{
    if (_presetRatingChangedEventCallback)
    {
        _presetRatingChangedEventCallback(presetIndex, rating, static_cast<projectm_preset_rating_type>(ratingType),
                                          _presetRatingChangedEventUserData);
    }
}

projectMWrapper* handle_to_instance(projectm_handle instance)
{
    return reinterpret_cast<projectMWrapper*>(instance);
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
    auto pointer = projectm_alloc_string(str.length() + 1);
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

projectm_settings* projectm_alloc_settings()
{
    try
    {
        return new projectm_settings{};
    }
    catch (...)
    {
        return nullptr;
    }
}

void projectm_free_settings(const projectm_settings* settings)
{
    if (settings)
    {
        projectm_free_string(settings->preset_path);
        projectm_free_string(settings->texture_path);
        projectm_free_string(settings->data_path);
    }

    delete settings;
}

projectm_handle projectm_create(const char* setting_file_path, int flags)
{
    try
    {
        auto projectMInstance = new projectMWrapper(std::string(setting_file_path), flags);
        return reinterpret_cast<projectm_handle>(projectMInstance);
    }
    catch (...)
    {
        return nullptr;
    }
}

projectm_handle projectm_create_settings(const projectm_settings* settings, int flags)
{
    try
    {
        class ProjectM::Settings cppSettings;
        cppSettings.meshX = settings->mesh_x;
        cppSettings.meshY = settings->mesh_y;
        cppSettings.fps = settings->fps;
        cppSettings.textureSize = settings->texture_size;
        cppSettings.windowWidth = settings->window_width;
        cppSettings.windowHeight = settings->window_height;
        cppSettings.presetPath = settings->preset_path ? settings->preset_path : "";
        cppSettings.texturePath = settings->texture_path ? settings->texture_path : "";
        cppSettings.dataPath = settings->data_path ? settings->data_path : "";
        cppSettings.softCutDuration = settings->soft_cut_duration;
        cppSettings.presetDuration = settings->preset_duration;
        cppSettings.hardCutEnabled = settings->hard_cut_enabled;
        cppSettings.hardCutDuration = settings->hard_cut_duration;
        cppSettings.hardCutSensitivity = settings->hard_cut_sensitivity;
        cppSettings.beatSensitivity = settings->beat_sensitivity;
        cppSettings.aspectCorrection = settings->aspect_correction;
        cppSettings.easterEgg = settings->easter_egg;
        cppSettings.shuffleEnabled = settings->shuffle_enabled;
        cppSettings.softCutRatingsEnabled = settings->soft_cut_ratings_enabled;

        auto projectMInstance = new projectMWrapper(cppSettings, flags);
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

void projectm_set_preset_switched_event_callback(projectm_handle instance, projectm_preset_switched_event callback,
                                                 void* user_data)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->_presetSwitchedEventCallback = callback;
    projectMInstance->_presetSwitchedEventUserData = user_data;
}

void projectm_set_shuffle_enable_changed_event_callback(projectm_handle instance,
                                                        projectm_shuffle_enable_changed_event callback, void* user_data)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->_shuffleEnableChangedEventCallback = callback;
    projectMInstance->_shuffleEnableChangedEventUserData = user_data;
}

void projectm_set_preset_switch_failed_event_callback(projectm_handle instance,
                                                      projectm_preset_switch_failed_event callback, void* user_data)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->_presetSwitchFailedEventCallback = callback;
    projectMInstance->_presetSwitchFailedEventUserData = user_data;
}

void projectm_set_preset_rating_changed_event_callback(projectm_handle instance,
                                                       projectm_preset_rating_changed_event callback, void* user_data)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->_presetRatingChangedEventCallback = callback;
    projectMInstance->_presetRatingChangedEventUserData = user_data;
}

void projectm_reset_gl(projectm_handle instance, int width, int height)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->ResetOpenGL(width, height);
}

void projectm_reset_textures(projectm_handle instance)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->ResetTextures();
}

void projectm_render_frame(projectm_handle instance)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->RenderFrame();
}

unsigned int projectm_init_render_to_texture(projectm_handle instance)
{
    auto projectMInstance = handle_to_instance(instance);
    return projectMInstance->InitRenderToTexture();
}

void projectm_key_handler(projectm_handle instance, projectMEvent event, projectMKeycode keycode,
                          projectMModifier modifier)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->KeyHandler(event, keycode, modifier);
}

void projectm_default_key_handler(projectm_handle instance, projectMEvent event, projectMKeycode keycode)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->DefaultKeyHandler(event, keycode);
}

size_t projectm_get_texture_size(projectm_handle instance)
{
    auto projectMInstance = handle_to_instance(instance);
    return projectMInstance->TextureSize();
}

void projectm_set_texture_size(projectm_handle instance, size_t size)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->SetTextureSize(size);
}

void projectm_set_beat_sensitivity(projectm_handle instance, float sensitivity)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->SetBeatSensitivity(sensitivity);
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

void projectm_set_preset_duration(projectm_handle instance, double seconds)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->SetPresetDuration(seconds);
}

void projectm_get_mesh_size(projectm_handle instance, size_t* width, size_t* height)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->MeshSize(*width, *height);
}

void projectm_set_mesh_size(projectm_handle instance, size_t width, size_t height)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->SetMeshSize(width, height);
}

size_t projectm_get_fps(projectm_handle instance)
{
    auto projectMInstance = handle_to_instance(instance);
    return projectMInstance->Settings().fps;
}

const char* projectm_get_preset_path(projectm_handle instance)
{
    auto projectMInstance = handle_to_instance(instance);
    return projectm_alloc_string_from_std_string(projectMInstance->Settings().presetPath);
}

const char* projectm_get_texture_path(projectm_handle instance)
{
    auto projectMInstance = handle_to_instance(instance);
    return projectm_alloc_string_from_std_string(projectMInstance->Settings().texturePath);
}

const char* projectm_get_data_path(projectm_handle instance)
{
    auto projectMInstance = handle_to_instance(instance);
    return projectm_alloc_string_from_std_string(projectMInstance->Settings().dataPath);
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

projectm_settings* projectm_get_settings(projectm_handle instance)
{
    auto projectMInstance = handle_to_instance(instance);
    const auto& settings = projectMInstance->Settings();

    auto settingsStruct = projectm_alloc_settings();
    settingsStruct->mesh_x = settings.meshX;
    settingsStruct->mesh_y = settings.meshY;
    settingsStruct->fps = settings.fps;
    settingsStruct->texture_size = settings.textureSize;
    settingsStruct->window_width = settings.windowWidth;
    settingsStruct->window_height = settings.windowHeight;
    settingsStruct->preset_path = projectm_alloc_string_from_std_string(settings.presetPath);
    settingsStruct->texture_path = projectm_alloc_string_from_std_string(settings.texturePath);
    settingsStruct->data_path = projectm_alloc_string_from_std_string(settings.dataPath);
    settingsStruct->soft_cut_duration = settings.softCutDuration;
    settingsStruct->preset_duration = settings.presetDuration;
    settingsStruct->hard_cut_enabled = settings.hardCutEnabled;
    settingsStruct->hard_cut_duration = settings.hardCutDuration;
    settingsStruct->hard_cut_sensitivity = settings.hardCutSensitivity;
    settingsStruct->beat_sensitivity = settings.beatSensitivity;
    settingsStruct->aspect_correction = settings.aspectCorrection;
    settingsStruct->easter_egg = settings.easterEgg;
    settingsStruct->shuffle_enabled = settings.shuffleEnabled;
    settingsStruct->soft_cut_ratings_enabled = settings.softCutRatingsEnabled;

    return settingsStruct;
}

void projectm_write_config(const char* config_file, const projectm_settings* settings)
{
    class ProjectM::Settings cppSettings;
    cppSettings.meshX = settings->mesh_x;
    cppSettings.meshY = settings->mesh_y;
    cppSettings.fps = settings->fps;
    cppSettings.textureSize = settings->texture_size;
    cppSettings.windowWidth = settings->window_width;
    cppSettings.windowHeight = settings->window_height;
    cppSettings.presetPath = settings->preset_path ? settings->preset_path : "";
    cppSettings.texturePath = settings->texture_path ? settings->texture_path : "";
    cppSettings.dataPath = settings->data_path ? settings->data_path : "";
    cppSettings.softCutDuration = settings->soft_cut_duration;
    cppSettings.presetDuration = settings->preset_duration;
    cppSettings.hardCutEnabled = settings->hard_cut_enabled;
    cppSettings.hardCutDuration = settings->hard_cut_duration;
    cppSettings.hardCutSensitivity = settings->hard_cut_sensitivity;
    cppSettings.beatSensitivity = settings->beat_sensitivity;
    cppSettings.aspectCorrection = settings->aspect_correction;
    cppSettings.easterEgg = settings->easter_egg;
    cppSettings.shuffleEnabled = settings->shuffle_enabled;
    cppSettings.softCutRatingsEnabled = settings->soft_cut_ratings_enabled;

    ProjectM::WriteConfig(config_file, cppSettings);
}

void projectm_select_preset_position(projectm_handle instance, unsigned int index)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->SelectPresetPosition(index);
}

void projectm_select_preset(projectm_handle instance, unsigned int index, bool hard_cut)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->SelectPreset(index, hard_cut);
}

void projectm_remove_preset(projectm_handle instance, unsigned int index)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->RemovePreset(index);
}

void projectm_clear_playlist(projectm_handle instance)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->ClearPlaylist();
}

void projectm_lock_preset(projectm_handle instance, bool lock)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->SetPresetLocked(lock);
}

bool projectm_is_preset_locked(projectm_handle instance)
{
    auto projectMInstance = handle_to_instance(instance);
    return projectMInstance->PresetLocked();
}

unsigned int projectm_get_preset_index(projectm_handle instance, const char* preset_name)
{
    if (!preset_name)
    {
        return 0;
    }

    auto projectMInstance = handle_to_instance(instance);
    return projectMInstance->PresetIndex(preset_name);
}

void projectm_select_preset_by_name(projectm_handle instance, const char* preset_name, bool hard_cut)
{
    if (!preset_name)
    {
        return;
    }

    auto projectMInstance = handle_to_instance(instance);
    return projectMInstance->SelectPresetByName(preset_name, hard_cut);
}

bool projectm_get_selected_preset_index(projectm_handle instance, unsigned int* index)
{
    if (!index)
    {
        return false;
    }

    auto projectMInstance = handle_to_instance(instance);
    return projectMInstance->SelectedPresetIndex(*index);
}

void projectm_add_preset_url(projectm_handle instance, const char* preset_url, const char* preset_name,
                             int* rating_list, unsigned int rating_list_length)
{
    if (!preset_url
        || !preset_name
        || (!rating_list && rating_list_length > 0)
        || rating_list_length != TOTAL_RATING_TYPES)
    {
        return;
    }

    RatingList ratingList;
    for (unsigned int ratingIndex = 0; ratingIndex < rating_list_length; ratingIndex++)
    {
        ratingList.push_back(rating_list[ratingIndex]);
    }

    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->AddPresetURL(preset_url, preset_name, ratingList);
}

void projectm_insert_preset_url(projectm_handle instance, unsigned int index, const char* preset_url,
                                const char* preset_name, int* rating_list, unsigned int rating_list_length)
{
    if (!preset_url
        || !preset_name
        || (!rating_list && rating_list_length > 0)
        || rating_list_length != TOTAL_RATING_TYPES)
    {
        return;
    }

    RatingList ratingList;
    for (unsigned int ratingIndex = 0; ratingIndex < rating_list_length; ratingIndex++)
    {
        ratingList.push_back(rating_list[ratingIndex]);
    }

    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->InsertPresetURL(index, preset_url, preset_name, ratingList);
}

bool projectm_preset_position_valid(projectm_handle instance)
{
    auto projectMInstance = handle_to_instance(instance);
    return projectMInstance->PresetPositionValid();
}

const char* projectm_get_preset_filename(projectm_handle instance, unsigned int index)
{
    auto projectMInstance = handle_to_instance(instance);
    return projectm_alloc_string_from_std_string(projectMInstance->PresetURL(index));
}

const char* projectm_get_preset_name(projectm_handle instance, unsigned int index)
{
    auto projectMInstance = handle_to_instance(instance);
    return projectm_alloc_string_from_std_string(projectMInstance->PresetName(index));
}

void projectm_set_preset_name(projectm_handle instance, unsigned int index, const char* name)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->ChangePresetName(index, name);
}

int projectm_get_preset_rating(projectm_handle instance, unsigned int index, projectm_preset_rating_type rating_type)
{
    auto projectMInstance = handle_to_instance(instance);
    return projectMInstance->PresetRating(index, static_cast<PresetRatingType>(rating_type));
}

void projectm_set_preset_rating(projectm_handle instance, unsigned int index, int rating,
                                projectm_preset_rating_type rating_type)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->ChangePresetRating(index, rating, static_cast<PresetRatingType>(rating_type));
}

unsigned int projectm_get_playlist_size(projectm_handle instance)
{
    auto projectMInstance = handle_to_instance(instance);
    return projectMInstance->PlaylistSize();
}

bool projectm_get_shuffle_enabled(projectm_handle instance)
{
    auto projectMInstance = handle_to_instance(instance);
    return projectMInstance->ShuffleEnabled();
}

void projectm_set_shuffle_enabled(projectm_handle instance, bool shuffle_enabled)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->SetShuffleEnabled(shuffle_enabled);
}

unsigned int projectm_get_search_index(projectm_handle instance, const char* name)
{
    auto projectMInstance = handle_to_instance(instance);
    return projectMInstance->SearchIndex(name);
}

void projectm_select_previous_preset(projectm_handle instance, bool hard_cut)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->SelectPrevious(hard_cut);
}

void projectm_select_next_preset(projectm_handle instance, bool hard_cut)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->SelectNext(hard_cut);
}

void projectm_select_random_preset(projectm_handle instance, bool hard_cut)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->SelectRandom(hard_cut);
}

void projectm_get_window_size(projectm_handle instance, size_t* width, size_t* height)
{
    auto projectMInstance = handle_to_instance(instance);
    *width = projectMInstance->WindowWidth();
    *height = projectMInstance->WindowHeight();
}

void projectm_set_window_size(projectm_handle instance, size_t width, size_t height)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->ResetOpenGL(width, height);
}

bool projectm_get_error_loading_current_preset(projectm_handle instance)
{
    auto projectMInstance = handle_to_instance(instance);
    return projectMInstance->ErrorLoadingCurrentPreset();
}

unsigned int projectm_pcm_get_max_samples()
{
    return Pcm::maxSamples;
}

template<class BufferType>
static auto PcmAdd(projectm_handle instance, const BufferType* samples, unsigned int count, projectm_channels channels) -> void
{
    auto* projectMInstance = handle_to_instance(instance);

    if(channels == PROJECTM_MONO) {
        projectMInstance->Pcm().AddMono(samples, count);
    }
    else {
        projectMInstance->Pcm().AddStereo(samples, count);
    }
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
