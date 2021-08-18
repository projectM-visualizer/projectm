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
        _presetSwitchedEventCallback(isHardCut, presetIndex);
    }
}

void projectMWrapper::shuffleEnabledValueChanged(bool shuffle_enabled) const
{
    if (_shuffleEnableChangedEventCallback)
    {
        _shuffleEnableChangedEventCallback(shuffle_enabled);
    }
}

void projectMWrapper::presetSwitchFailedEvent(bool isHardCut, unsigned int presetIndex,
                                              const std::string& failureMessage) const
{
    if (_presetSwitchFailedEventCallback)
    {
        _presetSwitchFailedEventCallback(isHardCut, presetIndex, failureMessage.c_str());
    }
}

void projectMWrapper::presetRatingChanged(unsigned int presetIndex, int rating, PresetRatingType ratingType) const
{
    if (_presetRatingChangedEventCallback)
    {
        _presetRatingChangedEventCallback(presetIndex, rating, static_cast<projectm_preset_rating_type>(ratingType));
    }
}

projectMWrapper* handle_to_instance(projectm_handle instance)
{
    return reinterpret_cast<projectMWrapper*>(instance);
}

char* PROJECTM_EXPORT projectm_alloc_string(unsigned int length)
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
        cppSettings.smoothPresetDuration = settings->smooth_preset_duration;
        cppSettings.presetDuration = settings->preset_duration;
        cppSettings.hardcutEnabled = settings->hardcut_enabled;
        cppSettings.hardcutDuration = settings->hardcut_duration;
        cppSettings.hardcutSensitivity = settings->hardcut_sensitivity;
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

void projectm_set_preset_switched_event_callback(projectm_handle instance, projectm_preset_switched_event callback)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->_presetSwitchedEventCallback = callback;
}

void projectm_set_shuffle_enable_changed_event_callback(projectm_handle instance,
                                                        projectm_shuffle_enable_changed_event callback)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->_shuffleEnableChangedEventCallback = callback;
}

void projectm_set_preset_switch_failed_event_callback(projectm_handle instance,
                                                      projectm_preset_switch_failed_event callback)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->_presetSwitchFailedEventCallback = callback;
}

void projectm_set_preset_rating_changed_event_callback(projectm_handle instance,
                                                       projectm_preset_rating_changed_event callback)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->_presetRatingChangedEventCallback = callback;
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

void projectm_set_title(projectm_handle instance, const char* title)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->projectM_setTitle(title);
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

void projectm_set_texture_size(projectm_handle instance, int size)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->changeTextureSize(size);
}

void projectm_set_hardcut_duration(projectm_handle instance, double seconds)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->changeHardcutDuration(seconds);
}

void projectm_set_preset_duration(projectm_handle instance, double seconds)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->changePresetDuration(seconds);
}

void projectm_get_mesh_size(projectm_handle instance, int* width, int* height)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->getMeshSize(width, height);
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
    settingsStruct->smooth_preset_duration = settings.smoothPresetDuration;
    settingsStruct->preset_duration = settings.presetDuration;
    settingsStruct->hardcut_enabled = settings.hardcutEnabled;
    settingsStruct->hardcut_duration = settings.hardcutDuration;
    settingsStruct->hardcut_sensitivity = settings.hardcutSensitivity;
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
    cppSettings.smoothPresetDuration = settings->smooth_preset_duration;
    cppSettings.presetDuration = settings->preset_duration;
    cppSettings.hardcutEnabled = settings->hardcut_enabled;
    cppSettings.hardcutDuration = settings->hardcut_duration;
    cppSettings.hardcutSensitivity = settings->hardcut_sensitivity;
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

bool projectm_selected_preset_index(projectm_handle instance, unsigned int* index)
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

const char* projectm_get_preset_url(projectm_handle instance, unsigned int index)
{
    auto projectMInstance = handle_to_instance(instance);
    return projectm_alloc_string_from_std_string(projectMInstance->getPresetURL(index));
}

const char* projectm_get_preset_name(projectm_handle instance, unsigned int index)
{
    auto projectMInstance = handle_to_instance(instance);
    return projectm_alloc_string_from_std_string(projectMInstance->getPresetName(index));
}

void projectm_change_preset_name(projectm_handle instance, unsigned int index, const char* name)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->changePresetName(index, name);
}

int projectm_get_preset_rating(projectm_handle instance, unsigned int index, projectm_preset_rating_type rating_type)
{
    auto projectMInstance = handle_to_instance(instance);
    return projectMInstance->getPresetRating(index, static_cast<PresetRatingType>(rating_type));
}

void projectm_change_preset_rating(projectm_handle instance, unsigned int index, int rating,
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

void projectm_set_shuffle_enabled(projectm_handle instance, bool shuffle_enabled)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->setShuffleEnabled(shuffle_enabled);
}

bool projectm_is_shuffle_enabled(projectm_handle instance)
{
    auto projectMInstance = handle_to_instance(instance);
    return projectMInstance->isShuffleEnabled();
}

unsigned int projectm_get_search_index(projectm_handle instance, const char* name)
{
    auto projectMInstance = handle_to_instance(instance);
    return projectMInstance->getSearchIndex(name);
}

void projectm_select_previous(projectm_handle instance, bool hard_cut)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->selectPrevious(hard_cut);
}

void projectm_select_next(projectm_handle instance, bool hard_cut)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->selectNext(hard_cut);
}

void projectm_select_random(projectm_handle instance, bool hard_cut)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->selectRandom(hard_cut);
}

int projectm_get_window_width(projectm_handle instance)
{
    auto projectMInstance = handle_to_instance(instance);
    return projectMInstance->getWindowWidth();
}

int projectm_get_window_height(projectm_handle instance)
{
    auto projectMInstance = handle_to_instance(instance);
    return projectMInstance->getWindowHeight();
}

bool projectm_get_error_loading_current_preset(projectm_handle instance)
{
    auto projectMInstance = handle_to_instance(instance);
    return projectMInstance->getErrorLoadingCurrentPreset();
}

unsigned int projectm_pcm_get_max_samples()
{
    return PCM::maxsamples;
}

void projectm_pcm_add_float_1ch_data(projectm_handle instance, const float* pcm_data, unsigned int sample_count)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->pcm()->addPCMfloat(pcm_data, sample_count);
}

void projectm_pcm_add_float_2ch_data(projectm_handle instance, const float* pcm_data, unsigned int sample_count)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->pcm()->addPCMfloat_2ch(pcm_data, sample_count);
}

void projectm_pcm_add_16bit_2ch_512(projectm_handle instance, const short (* pcm_data)[512])
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->pcm()->addPCM16(pcm_data);
}

void projectm_pcm_add_16bit_2ch_data(projectm_handle instance, const short* pcm_data, unsigned int sample_count)
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->pcm()->addPCM16Data(pcm_data, sample_count);
}

void projectm_pcm_add_8bit_2ch_1024(projectm_handle instance, const unsigned char (* pcm_data)[1024])
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->pcm()->addPCM8(pcm_data);
}

void projectm_pcm_add_8bit_2ch_512(projectm_handle instance, const unsigned char (* pcm_data)[512])
{
    auto projectMInstance = handle_to_instance(instance);
    projectMInstance->pcm()->addPCM8_512(pcm_data);
}
