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


typedef enum {
SOFT_CUT,
HARD_CUT
} cut_type;

class PresetSwitcher {

public:

    /** Preset switching */
    static int loadPresetDir( char *dir );
    static int closePresetDir();
    static int switchPreset( switch_mode_t switch_mode, int cut_type );
    static void switchToIdlePreset();
    static int loadPresetByFile( char *filename );
    static int initPresetLoader();
    static int destroyPresetLoader();
int is_valid_extension(const struct dirent* ent);
/* destroyPresetLoader: closes the preset
   loading library. This should be done when 
   PresetSwitcher does cleanup */
};
#endif
