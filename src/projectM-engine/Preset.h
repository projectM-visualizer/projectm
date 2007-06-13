/**
 * projectM -- Milkdrop-esque visualisation SDK
 * Copyright (C)2003-2007 projectM Team
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * See 'LICENSE.txt' included within this release
 *
 */
/**
 * $Id$
 *
 * Preset
 *
 * $Log$
 */

#ifndef _PRESET_H
#define _PRESET_H

#include "common.h"

//#define PRESET_DEBUG 2 /* 0 for no debugging, 1 for normal, 2 for insane */

#define HARD_CUT 0
#define SOFT_CUT 1

#define MAX_PRESETS_IN_DIR 50000
#define IDLE_PRESET_STRING "[idlepreset]\n"

#include "projectM.h"

#include "CustomShape.h"
#include "CustomWave.h"
#include "Expr.h"
#include "PerPixelEqn.h"
#include "PerFrameEqn.h"

class CustomWave;
class InitCond;
class SplayTree;

class Preset {
public:
    /** vars */
    char name[MAX_TOKEN_SIZE]; /* preset name as parsed in file */
    char file_path[MAX_PATH_SIZE]; /* Points to the preset file name */

    /** IDs of current waves/shapes */
    int interface_id,
        cwave_interface_id;
    CustomWave *active_wave;
    CustomShape *active_shape;

    static Preset *active_preset,
                  *idle_preset,
                  *old_preset;
  
    int per_pixel_eqn_string_index;
    int per_frame_eqn_string_index;
    int per_frame_init_eqn_string_index;

    int per_frame_eqn_count,
        per_frame_init_eqn_count;
	
    int per_pixel_flag[NUM_OPS];
    char per_pixel_eqn_string_buffer[STRING_BUFFER_SIZE];
    char per_frame_eqn_string_buffer[STRING_BUFFER_SIZE];
    char per_frame_init_eqn_string_buffer[STRING_BUFFER_SIZE];

    /* Data structures that contain equation and initial condition information */
    SplayTree * per_frame_eqn_tree;   /* per frame equations */
    SplayTree * per_pixel_eqn_tree; /* per pixel equation tree */
    GenExpr * per_pixel_eqn_array[NUM_OPS]; /* per pixel equation array */
    SplayTree * per_frame_init_eqn_tree; /* per frame initial equations */
    SplayTree * init_cond_tree; /* initial conditions */
    SplayTree * user_param_tree; /* user parameter splay tree */
    
    SplayTree * custom_wave_tree; /* custom wave forms for this preset */
    SplayTree * custom_shape_tree; /* custom shapes for this preset */
    
    /** Methods */
    void evalInitConditions();
    void evalCustomWaveInitConditions();
    void evalCustomShapeInitConditions();

    void evalPerFrameEquations();

    int add_per_pixel_eqn( char *name, GenExpr *gen_expr );
    int isPerPixelEqn( int op );
    void evalPerPixelEqns();
    int resetPerPixelEqns();
    int resetPerPixelEqnFlags();

    void evalPerPointEqns();

    CustomWave *nextCustomWave();
    CustomShape *nextCustomShape();

    void load_init_conditions();
    InitCond *get_init_cond( Param *param );

    void load_custom_wave_init_conditions();
    void load_custom_wave_init( CustomWave *customWave );
    void load_custom_shape_init_conditions();
    void load_custom_shape_init( CustomShape *customShape );

    void reloadPerFrame(char * s);
    void reloadPerFrameInit(char *s);
    void reloadPerPixel(char *s);

    int close_preset();
    static Preset *load_preset( const char *pathname );
    void savePreset(char * name);
    int write_preset_name( FILE *fs );
    int write_init_conditions( FILE *fs );
    int write_init( InitCond *initCond );
    int write_per_frame_init_equations( FILE *fs );
    int write_per_frame_equations( FILE *fs );
    int write_per_pixel_equations( FILE *fs );

    static Preset *init_idle_preset();
    static int destroy_idle_preset();
    int destroy();
  };    

/** Splaytree traversal helpers */
inline void load_custom_shape_init_helper( void *custom_shape ) {
    ((CustomShape *)custom_shape)->load_custom_shape_init();
  }

#endif /** !_PRESET_H */
