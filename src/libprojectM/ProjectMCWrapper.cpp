#include "ProjectMCWrapper.hpp"

#include "projectM.h"

#include <cstring>
#include <utility>

projectMWrapper::projectMWrapper(std::string configFile, int flags)
    : projectM(std::move(configFile), flags)
{
}

projectMWrapper::projectMWrapper(projectM::Settings settings, int flags)
    : projectM(std::move(settings), flags)
{
}

void projectMWrapper::presetSwitchedEvent(bool isHardCut, size_t presetIndex) const
{
    if (_presetSwitchedEventCallback)
    {
        _presetSwitchedEventCallback(isHardCut, presetIndex, _presetSwitchedEventUserData);
    }
}

void projectMWrapper::shuffleEnabledValueChanged(bool shuffle_enabled) const
{
    if (_shuffleEnableChangedEventCallback)
    {
        _shuffleEnableChangedEventCallback(shuffle_enabled, _shuffleEnableChangedEventUserData);
    }
}

void projectMWrapper::presetSwitchFailedEvent(bool isHardCut, unsigned int presetIndex,
                                              const std::string& failureMessage) const
{
    if (_presetSwitchFailedEventCallback)
    {
        _presetSwitchFailedEventCallback(isHardCut, presetIndex,
                                         failureMessage.c_str(), _presetSwitchFailedEventUserData);
    }
}

void projectMWrapper::presetRatingChanged(unsigned int presetIndex, int rating, PresetRatingType ratingType) const
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
        projectm_free_string(settings->preset_url);
        projectm_free_string(settings->title_font_url);
        projectm_free_string(settings->menu_font_url);
        projectm_free_string(settings->data_dir);
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
        projectM::Settings cppSettings;
        cppSettings.meshX = settings->mesh_x;
        cppSettings.meshY = settings->mesh_y;
        cppSettings.fps = settings->fps;
        cppSettings.textureSize = settings->texture_size;
        cppSettings.windowWidth = settings->window_width;
        cppSettings.windowHeight = settings->window_height;
        cppSettings.presetURL = settings->preset_url ? settings->preset_url : "";
        cppSettings.titleFontURL = settings->title_font_url ? settings->title_font_url : "";
        cppSettings.menuFontURL = settings->menu_font_url ? settings->menu_font_url : "";
        cppSettings.datadir = settings->data_dir ? settings->data_dir : "";
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
    projectMInstance->projectM_resetGL(width, height);
}

void projectm_reset_textures(projectm_handle instance)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->projectM_resetTextures();
}

const char* projectm_get_title(projectm_handle instance)
{
    auto projectMInstance = handle_to_instance(instance);
    return projectm_alloc_string_from_std_string(projectMInstance->getTitle());
}

void projectm_set_title(projectm_handle instance, const char* title)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->setTitle(title);
}

void projectm_render_frame(projectm_handle instance)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->renderFrame();
}

unsigned int projectm_init_render_to_texture(projectm_handle instance)
{
    auto projectMInstance = handle_to_instance(instance);
    return projectMInstance->initRenderToTexture();
}

void projectm_key_handler(projectm_handle instance, projectMEvent event, projectMKeycode keycode,
                          projectMModifier modifier)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->key_handler(event, keycode, modifier);
}

void projectm_default_key_handler(projectm_handle instance, projectMEvent event, projectMKeycode keycode)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->default_key_handler(event, keycode);
}

size_t projectm_get_texture_size(projectm_handle instance)
{
    auto projectMInstance = handle_to_instance(instance);
    return projectMInstance->getTextureSize();
}

void projectm_set_texture_size(projectm_handle instance, size_t size)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->setTextureSize(size);
}

double projectm_get_hard_cut_duration(projectm_handle instance)
{
    auto projectMInstance = handle_to_instance(instance);
    return projectMInstance->getHardCutDuration();
}

void projectm_set_hard_cut_duration(projectm_handle instance, double seconds)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->setHardCutDuration(seconds);
}

bool projectm_get_hard_cut_enabled(projectm_handle instance)
{
    auto projectMInstance = handle_to_instance(instance);
    return projectMInstance->getHardCutEnabled();
}

void projectm_set_hard_cut_enabled(projectm_handle instance, bool enabled)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->setHardCutEnabled(enabled);
}

float projectm_get_hard_cut_sensitivity(projectm_handle instance)
{
    auto projectMInstance = handle_to_instance(instance);
    return projectMInstance->getHardCutSensitivity();
}

void projectm_set_hard_cut_sensitivity(projectm_handle instance, float sensitivity)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->setHardCutSensitivity(sensitivity);
}

double projectm_get_soft_cut_duration(projectm_handle instance)
{
    auto projectMInstance = handle_to_instance(instance);
    return projectMInstance->getSoftCutDuration();
}

void projectm_set_soft_cut_duration(projectm_handle instance, double seconds)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->setSoftCutDuration(seconds);
}

void projectm_set_preset_duration(projectm_handle instance, double seconds)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->setPresetDuration(seconds);
}

void projectm_get_mesh_size(projectm_handle instance, size_t* width, size_t* height)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->getMeshSize(*width, *height);
}

void projectm_set_mesh_size(projectm_handle instance, size_t width, size_t height)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->setMeshSize(width, height);
}

size_t projectm_get_fps(projectm_handle instance)
{
    auto projectMInstance = handle_to_instance(instance);
    return projectMInstance->settings().fps;
}

const char* projectm_get_preset_path(projectm_handle instance)
{
    auto projectMInstance = handle_to_instance(instance);
    return projectm_alloc_string_from_std_string(projectMInstance->settings().presetURL);
}

const char* projectm_get_title_font_filename(projectm_handle instance)
{
    auto projectMInstance = handle_to_instance(instance);
    return projectm_alloc_string_from_std_string(projectMInstance->settings().titleFontURL);
}

const char* projectm_get_menu_font_filename(projectm_handle instance)
{
    auto projectMInstance = handle_to_instance(instance);
    return projectm_alloc_string_from_std_string(projectMInstance->settings().menuFontURL);
}

const char* projectm_get_data_dir_path(projectm_handle instance)
{
    auto projectMInstance = handle_to_instance(instance);
    return projectm_alloc_string_from_std_string(projectMInstance->settings().datadir);
}

void projectm_set_aspect_correction(projectm_handle instance, bool enabled)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->setAspectCorrection(enabled);
}

bool projectm_get_aspect_correction(projectm_handle instance)
{
    auto projectMInstance = handle_to_instance(instance);
    return projectMInstance->getAspectCorrection();
}

void projectm_set_easter_egg(projectm_handle instance, float value)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->setEasterEgg(value);
}

float projectm_get_easter_egg(projectm_handle instance)
{
    auto projectMInstance = handle_to_instance(instance);
    return projectMInstance->getEasterEgg();
}

void projectm_touch(projectm_handle instance, float x, float y, int pressure, projectm_touch_type touch_type)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->touch(x, y, pressure, touch_type);
}

void projectm_touch_drag(projectm_handle instance, float x, float y, int pressure)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->touchDrag(x, y, pressure);
}

void projectm_touch_destroy(projectm_handle instance, float x, float y)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->touchDestroy(x, y);
}

void projectm_touch_destroy_all(projectm_handle instance)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->touchDestroyAll();
}

void projectm_set_help_text(projectm_handle instance, const char* help_text)
{
    if (!help_text)
    {
        return;
    }

    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->setHelpText(help_text);
}

void projectm_set_toast_message(projectm_handle instance, const char* toast_message)
{
    if (!toast_message)
    {
        return;
    }

    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->setToastMessage(toast_message);
}

projectm_settings* projectm_get_settings(projectm_handle instance)
{
    auto projectMInstance = handle_to_instance(instance);
    const auto& settings = projectMInstance->settings();

    auto settingsStruct = projectm_alloc_settings();
    settingsStruct->mesh_x = settings.meshX;
    settingsStruct->mesh_y = settings.meshY;
    settingsStruct->fps = settings.fps;
    settingsStruct->texture_size = settings.textureSize;
    settingsStruct->window_width = settings.windowWidth;
    settingsStruct->window_height = settings.windowHeight;
    settingsStruct->preset_url = projectm_alloc_string_from_std_string(settings.presetURL);
    settingsStruct->title_font_url = projectm_alloc_string_from_std_string(settings.titleFontURL);
    settingsStruct->menu_font_url = projectm_alloc_string_from_std_string(settings.menuFontURL);
    settingsStruct->data_dir = projectm_alloc_string_from_std_string(settings.datadir);
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
    projectM::Settings cppSettings;
    cppSettings.meshX = settings->mesh_x;
    cppSettings.meshY = settings->mesh_y;
    cppSettings.fps = settings->fps;
    cppSettings.textureSize = settings->texture_size;
    cppSettings.windowWidth = settings->window_width;
    cppSettings.windowHeight = settings->window_height;
    cppSettings.presetURL = settings->preset_url ? settings->preset_url : "";
    cppSettings.titleFontURL = settings->title_font_url ? settings->title_font_url : "";
    cppSettings.menuFontURL = settings->menu_font_url ? settings->menu_font_url : "";
    cppSettings.datadir = settings->data_dir ? settings->data_dir : "";
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

    projectM::writeConfig(config_file, cppSettings);
}

void projectm_select_preset_position(projectm_handle instance, unsigned int index)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->selectPresetPosition(index);
}

void projectm_select_preset(projectm_handle instance, unsigned int index, bool hard_cut)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->selectPreset(index, hard_cut);
}

void projectm_populate_preset_menu(projectm_handle instance)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->populatePresetMenu();
}

void projectm_remove_preset(projectm_handle instance, unsigned int index)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->removePreset(index);
}

void projectm_clear_playlist(projectm_handle instance)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->clearPlaylist();
}

void projectm_lock_preset(projectm_handle instance, bool lock)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->setPresetLock(lock);
}

bool projectm_is_preset_locked(projectm_handle instance)
{
    auto projectMInstance = handle_to_instance(instance);
    return projectMInstance->isPresetLocked();
}

bool projectm_is_text_input_active(projectm_handle instance, bool no_minimum_length)
{
    auto projectMInstance = handle_to_instance(instance);
    return projectMInstance->isTextInputActive(no_minimum_length);
}

unsigned int projectm_get_preset_index(projectm_handle instance, const char* preset_name)
{
    if (!preset_name)
    {
        return 0;
    }

    auto projectMInstance = handle_to_instance(instance);
    return projectMInstance->getPresetIndex(preset_name);
}

void projectm_select_preset_by_name(projectm_handle instance, const char* preset_name, bool hard_cut)
{
    if (!preset_name)
    {
        return;
    }

    auto projectMInstance = handle_to_instance(instance);
    return projectMInstance->selectPresetByName(preset_name, hard_cut);
}

const char* projectm_get_search_text(projectm_handle instance)
{
    auto projectMInstance = handle_to_instance(instance);
    return projectm_alloc_string_from_std_string(projectMInstance->getSearchText());
}

void projectm_set_search_text(projectm_handle instance, const char* search_text)
{
    if (!search_text)
    {
        return;
    }

    auto projectMInstance = handle_to_instance(instance);
    return projectMInstance->setSearchText(search_text);
}

void projectm_delete_search_text(projectm_handle instance)
{
    auto projectMInstance = handle_to_instance(instance);
    return projectMInstance->deleteSearchText();
}

void projectm_reset_search_text(projectm_handle instance)
{
    auto projectMInstance = handle_to_instance(instance);
    return projectMInstance->resetSearchText();
}

bool projectm_get_selected_preset_index(projectm_handle instance, unsigned int* index)
{
    if (!index)
    {
        return false;
    }

    auto projectMInstance = handle_to_instance(instance);
    return projectMInstance->selectedPresetIndex(*index);
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
    projectMInstance->addPresetURL(preset_url, preset_name, ratingList);
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
    projectMInstance->insertPresetURL(index, preset_url, preset_name, ratingList);
}

bool projectm_preset_position_valid(projectm_handle instance)
{
    auto projectMInstance = handle_to_instance(instance);
    return projectMInstance->presetPositionValid();
}

const char* projectm_get_preset_filename(projectm_handle instance, unsigned int index)
{
    auto projectMInstance = handle_to_instance(instance);
    return projectm_alloc_string_from_std_string(projectMInstance->getPresetURL(index));
}

const char* projectm_get_preset_name(projectm_handle instance, unsigned int index)
{
    auto projectMInstance = handle_to_instance(instance);
    return projectm_alloc_string_from_std_string(projectMInstance->getPresetName(index));
}

void projectm_set_preset_name(projectm_handle instance, unsigned int index, const char* name)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->changePresetName(index, name);
}

int projectm_get_preset_rating(projectm_handle instance, unsigned int index, projectm_preset_rating_type rating_type)
{
    auto projectMInstance = handle_to_instance(instance);
    return projectMInstance->getPresetRating(index, static_cast<PresetRatingType>(rating_type));
}

void projectm_set_preset_rating(projectm_handle instance, unsigned int index, int rating,
                                projectm_preset_rating_type rating_type)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->changePresetRating(index, rating, static_cast<PresetRatingType>(rating_type));
}

unsigned int projectm_get_playlist_size(projectm_handle instance)
{
    auto projectMInstance = handle_to_instance(instance);
    return projectMInstance->getPlaylistSize();
}

bool projectm_get_shuffle_enabled(projectm_handle instance)
{
    auto projectMInstance = handle_to_instance(instance);
    return projectMInstance->isShuffleEnabled();
}

void projectm_set_shuffle_enabled(projectm_handle instance, bool shuffle_enabled)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->setShuffleEnabled(shuffle_enabled);
}

unsigned int projectm_get_search_index(projectm_handle instance, const char* name)
{
    auto projectMInstance = handle_to_instance(instance);
    return projectMInstance->getSearchIndex(name);
}

void projectm_select_previous_preset(projectm_handle instance, bool hard_cut)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->selectPrevious(hard_cut);
}

void projectm_select_next_preset(projectm_handle instance, bool hard_cut)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->selectNext(hard_cut);
}

void projectm_select_random_preset(projectm_handle instance, bool hard_cut)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->selectRandom(hard_cut);
}

void projectm_get_window_size(projectm_handle instance, size_t* width, size_t* height)
{
    auto projectMInstance = handle_to_instance(instance);
    *width = projectMInstance->getWindowWidth();
    *height = projectMInstance->getWindowHeight();
}

void projectm_set_window_size(projectm_handle instance, size_t width, size_t height)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->projectM_resetGL(width, height);
}

bool projectm_get_error_loading_current_preset(projectm_handle instance)
{
    auto projectMInstance = handle_to_instance(instance);
    return projectMInstance->getErrorLoadingCurrentPreset();
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
        projectMInstance->pcm()->AddMono(samples, count);
    }
    else {
        projectMInstance->pcm()->AddStereo(samples, count);
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
