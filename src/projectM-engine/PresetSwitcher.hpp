/** PresetSwitcher.hpp:
 * 	This class is a WIP that provides methods to load presets, switch presets, and
 *	merge them. The user of this class decides when / how to switch by calling functions
 *	of this class at the appropriate times in the renderer.
 *	DECISION: Merging / etc. goes in PresetFilters.hpp! Loading goes in here, switching goes in here
 */

/// More specifically, we need to 
/// (1) load presets from a path(s)
/// (2) set which preset is the "active" presets
/// (3) merge two presets together - probably doesn't belong in this class
///      how about another class called PresetFilters?


/// @idea Weighted random based on user stats

#ifndef PRESET_SWITCHER_HPP
#define PRESET_SWITCHER_HPP
#include "Preset.hpp"
#include "projectM.h"

typedef enum {
    ALPHA_NEXT,
    ALPHA_PREVIOUS,
    RANDOM_NEXT,
    RESTART_ACTIVE,
} switch_mode_t;


class PresetSwitcher {

public:

    /** Preset switching */
     int loadPresetDir( char *dir );
    int closePresetDir();
    int switchPreset( switch_mode_t switch_mode, int cut_type );
    int loadPresetByFile( char *filename );
    int initPresetLoader();
    int destroyPresetLoader();
    static int is_valid_extension(const struct dirent* ent);

/* destroyPresetLoader: closes the preset
   loading library. This should be done when 
   PresetSwitcher does cleanup */
};
#endif
