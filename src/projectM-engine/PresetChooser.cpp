//
// C++ Implementation: PresetSwitcher
//
// Description:
//
//
// Author: Carmelo Piccione <carmelo.piccione@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//

#if 0

#include "PresetSwitcher.hpp"

int PresetSwitcher::destroyPresetLoader() {

    //if ((Preset::active_preset != NULL) && (Preset::active_preset != Preset::idle_preset)) {
    //	Preset::active_preset->close_preset();
    //}

    ///Preset::active_preset = NULL;
    //Preset::destroy_idle_preset();
    //destroy_builtin_param_db();
    //destroy_builtin_func_db();
    //Eval::destroy_infix_ops();

    return PROJECTM_SUCCESS;

}

/* Loads a specific preset by absolute path */
int PresetSwitcher::loadPresetByFile(char * filename) {

    Preset * new_preset;

    /* Finally, load the preset using its actual path */
    if ((new_preset = Preset::load_preset(filename)) == NULL) {
#ifdef PRESET_DEBUG
        printf("loadPresetByFile: failed to load preset!\n");
#endif
        return PROJECTM_ERROR;
    }
    abort();

    /* Closes a preset currently loaded, if any */
//  if ((this->activePreset != NULL) && (this->activePreset != Preset::idle_preset))
    //  this->activePreset->close_preset();

    /* Sets active preset global pointer */
    //this->activePreset = new_preset;

    /* Reinitialize engine variables */
//  PresetSwitcher_resetengine();


    /* Add any missing initial conditions for each wave */
//  this->activePreset->load_custom_wave_init_conditions();

    /* Add any missing initial conditions for each wave */
//  this->activePreset->load_custom_shape_init_conditions();

    /* Add any missing initial conditions */
//  load_init_conditions();

    /* Need to do this once for menu */
    //this->activePreset->evalInitConditions();
    //  evalPerFrameInitEquations();


    return PROJECTM_SUCCESS;
}


/* switchPreset: loads the next preset from the directory stream.
   loadPresetDir() must be called first. This is a
   sequential load function */

int PresetSwitcher::switchPreset(switch_mode_t switch_mode, int cut_type) {

    Preset * new_preset = 0;

    int switch_index;
    int sindex = 0;
    int slen = 0;

    DWRITE( "switchPreset(): in\n" );
//    DWRITE( "switchPreset(): %s\n", presetURL );

    switch (switch_mode) {
    case ALPHA_NEXT:
//		preset_index = switch_index = preset_index + 1;
        break;
    case ALPHA_PREVIOUS:
//		preset_index = switch_index = preset_index - 1;
        break;
    case RANDOM_NEXT:
        switch_index = rand();
        break;
    case RESTART_ACTIVE:
//		switch_index = preset_index;
        break;
    default:
        return PROJECTM_FAILURE;
    }

    DWRITE( "switch_index: %d\n", switch_index );

    // iterate through the presetURL directory looking for the next entry
    {
        struct dirent** entries;
//		int dir_size = scandir(presetURL, &entries, /* is_valid_extension */ NULL, alphasort);
//        DWRITE( "dir_size: %d\n", dir_size );
//		if (dir_size > 0) {
        int i;

//            DWRITE( "nentries: %d\n", dir_size );

//			switch_index %= dir_size;
//			if (switch_index < 0) switch_index += dir_size
//			for (i = 0; i < dir_size; ++i) {
//				if (switch_index == i) {
        // matching entry
//					const size_t len = strlen(presetURL);
//					char* path = (char *) malloc(len + strlen(entries[i]->d_name) + 2);
//					if (path) {
//						strcpy(path, presetURL);
//						if (len && ((path[len - 1] != '/')||(path[len - 1] != '\\'))) {
//#ifdef WIN32
//							strcat(path + len, "\\");
//#else
//							strcat(path + len, "/");
//#endif
//						}
//						strcat(path + len, entries[i]->d_name);
//
//						new_preset = Preset::load_preset(path);
//						free(path);

//						// we must keep iterating to free the remaining entries
//					}
//				  }
//				free(entries[i]);
//			}
//			free(entries);
//		}
    }

#ifdef WIN32
    new_preset = Preset::load_preset( "c:\\tmp\\PresetSwitcher-1.00\\presets_test\\C.milk" );
#else
    //    new_preset = Preset::load_preset( "/Users/descarte/tmp/PresetSwitcher-1.00/presets_test/B.milk" );
    //    new_preset = NULL;
#endif

    if (!new_preset) {
        return PROJECTM_ERROR;
    }


    /* Closes a preset currently loaded, if any */
//  if ((this->activePreset != NULL) && (this->activePreset != Preset::idle_preset)) {
    //      this->activePreset->close_preset();
    // }

    /* Sets global this->activePreset pointer */
    //this->activePreset = new_preset;

#if 0
#ifndef PANTS
    /** Split out the preset name from the path */
    slen = strlen( new_preset->file_path );
    sindex = slen;
    while ( new_preset->file_path[sindex] != WIN32_PATH_SEPARATOR &&
            new_preset->file_path[sindex] != UNIX_PATH_SEPARATOR && sindex > 0 ) {
        sindex--;
    }
    sindex++;
    if ( presetName != NULL ) {
        free( presetName );
        presetName = NULL;
    }
    presetName = (char *)wipemalloc( sizeof( char ) * (slen - sindex + 1) );
    strncpy( presetName, new_preset->file_path + sindex, slen - sindex );
    presetName[slen - sindex] = '\0';
#endif

    /* Reinitialize the engine variables to sane defaults */
    PresetSwitcher_resetengine();

    /* Add any missing initial conditions */
    load_init_conditions();

    /* Add any missing initial conditions for each wave */
    this->activePreset->load_custom_wave_init_conditions();

    /* Add any missing initial conditions for each shape */
    this->activePreset->load_custom_shape_init_conditions();

    /* Need to evaluate the initial conditions once */
    this->activePreset->evalInitConditions();
    this->activePreset->evalCustomWaveInitConditions();
    this->activePreset->evalCustomShapeInitConditions();
    //  evalInitPerFrameEquations();
#endif
    return PROJECTM_SUCCESS;
}



int PresetSwitcher::switchPreset(switch_mode_t switch_mode, int cut_type) {

    Preset * new_preset = 0;

    int switch_index;
    int sindex = 0;
    int slen = 0;

    DWRITE( "switchPreset(): in\n" );
    DWRITE( "switchPreset(): %s\n", presetURL );

    switch (switch_mode) {
    case ALPHA_NEXT:
        preset_index = switch_index = preset_index + 1;
        break;
    case ALPHA_PREVIOUS:
        preset_index = switch_index = preset_index - 1;
        break;
    case RANDOM_NEXT:
        switch_index = rand();
        break;
    case RESTART_ACTIVE:
        switch_index = preset_index;
        break;
    default:
        return PROJECTM_FAILURE;
    }

    DWRITE( "switch_index: %d\n", switch_index );

    // iterate through the presetURL directory looking for the next entry
    {
        struct dirent** entries;
        int dir_size = scandir(presetURL, &entries, /* is_valid_extension */ NULL, alphasort);
        DWRITE( "dir_size: %d\n", dir_size );
        if (dir_size > 0) {
            int i;

            DWRITE( "nentries: %d\n", dir_size );

            switch_index %= dir_size;
            if (switch_index < 0) switch_index += dir_size;
            for (i = 0; i < dir_size; ++i) {
                if (switch_index == i) {
                    // matching entry
                    const size_t len = strlen(presetURL);
                    char* path = (char *) malloc(len + strlen(entries[i]->d_name) + 2);
                    if (path) {
                        strcpy(path, presetURL);
                        if (len && ((path[len - 1] != '/')||(path[len - 1] != '\\'))) {
#ifdef WIN32
                            strcat(path + len, "\\");
#else
                            strcat(path + len, "/");
#endif
                        }
                        strcat(path + len, entries[i]->d_name);

                        new_preset = Preset::load_preset(path);
                        free(path);

                        // we must keep iterating to free the remaining entries
                    }
                }
                free(entries[i]);
            }
            free(entries);
        }
    }

#ifdef WIN32
    new_preset = Preset::load_preset( "c:\\tmp\\projectM-1.00\\presets_test\\C.milk" );
#else
    //    new_preset = Preset::load_preset( "/Users/descarte/tmp/projectM-1.00/presets_test/B.milk" );
    //    new_preset = NULL;
#endif

    if (!new_preset) {
        switchToIdlePreset();
        return PROJECTM_ERROR;
    }


    /* Closes a preset currently loaded, if any */
    if ((Preset::active_preset != NULL) && (Preset::active_preset != Preset::idle_preset)) {
        Preset::active_preset->close_preset();
    }

    /* Sets global Preset::active_preset pointer */
    Preset::active_preset = new_preset;

#ifndef PANTS
    /** Split out the preset name from the path */
    slen = strlen( new_preset->file_path );
    sindex = slen;
    while ( new_preset->file_path[sindex] != WIN32_PATH_SEPARATOR &&
            new_preset->file_path[sindex] != UNIX_PATH_SEPARATOR && sindex > 0 ) {
        sindex--;
    }
    sindex++;
    if ( presetName != NULL ) {
        free( presetName );
        presetName = NULL;
    }
    presetName = (char *)wipemalloc( sizeof( char ) * (slen - sindex + 1) );
    strncpy( presetName, new_preset->file_path + sindex, slen - sindex );
    presetName[slen - sindex] = '\0';
#endif

    /* Reinitialize the engine variables to sane defaults */
    projectM_resetengine();

    /* Add any missing initial conditions */
    load_init_conditions();

    /* Add any missing initial conditions for each wave */
    Preset::active_preset->load_custom_wave_init_conditions();

    /* Add any missing initial conditions for each shape */
    Preset::active_preset->load_custom_shape_init_conditions();

    /* Need to evaluate the initial conditions once */
    Preset::active_preset->evalInitConditions();
    Preset::active_preset->evalCustomWaveInitConditions();
    Preset::active_preset->evalCustomShapeInitConditions();

    //  evalInitPerFrameEquations();
    return PROJECTM_SUCCESS;

}

/* Loads a specific preset by absolute path */
int projectM::loadPresetByFile(char * filename) {

    Preset * new_preset;

    /* Finally, load the preset using its actual path */
    if ((new_preset = Preset::load_preset(filename)) == NULL) {
#ifdef PRESET_DEBUG
        printf("loadPresetByFile: failed to load preset!\n");
#endif
        return PROJECTM_ERROR;
    }

    /* Closes a preset currently loaded, if any */
    if ((Preset::active_preset != NULL) && (Preset::active_preset != Preset::idle_preset))
        Preset::active_preset->close_preset();

    /* Sets active preset global pointer */
    Preset::active_preset = new_preset;

    /* Reinitialize engine variables */
    projectM_resetengine();


    /* Add any missing initial conditions for each wave */
    Preset::active_preset->load_custom_wave_init_conditions();

    /* Add any missing initial conditions for each wave */
    Preset::active_preset->load_custom_shape_init_conditions();

    /* Add any missing initial conditions */
    load_init_conditions();

    /* Need to do this once for menu */
    Preset::active_preset->evalInitConditions();
    //  evalPerFrameInitEquations();

    return PROJECTM_SUCCESS;
}

/* Returns nonzero if string 'name' contains .milk or
   (the better) .prjm extension. Not a very strong function currently */
int PresetSwitcher::is_valid_extension(const struct dirent* ent) {
    const char* ext = 0;

    if (!ent) return FALSE;

    ext = strrchr(ent->d_name, '.');
    if (!ext) ext = ent->d_name;

    if (0 == strcasecmp(ext, MILKDROP_FILE_EXTENSION)) return TRUE;
    if (0 == strcasecmp(ext, PROJECTM_FILE_EXTENSION)) return TRUE;

    return FALSE;
}

#endif