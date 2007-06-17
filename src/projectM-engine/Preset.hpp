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

#ifndef _PRESET_HPP
#define _PRESET_HPP

#include "common.h"
#include <string>
//#define PRESET_DEBUG 2 /* 0 for no debugging, 1 for normal, 2 for insane */

#include "projectM.h"

#include "CustomShape.h"
#include "CustomWave.h"
#include "Expr.h"
#include "PerPixelEqn.h"
#include "PerFrameEqn.h"

class CustomWave;
class InitCond;
class SplayTree;


/** Container for all engine variables. It's a struct
 * so that it's light weight. Access is done directly on
 * members for Mr. Sperl's convenience */
struct EngineVars {

    /// @bug should this exist / be here?
    int mesh_i, mesh_j;

    /* PER FRAME CONSTANTS BEGIN */
    float zoom;
    float zoomexp;
    float rot;
    float warp;

    float sx;
    float sy;
    float dx;
    float dy;
    float cx;
    float cy;

    float decay;

    float wave_r;
    float wave_g;
    float wave_b;
    float wave_o;
    float wave_x;
    float wave_y;
    float wave_mystery;

    float ob_size;
    float ob_r;
    float ob_g;
    float ob_b;
    float ob_a;

    float ib_size;
    float ib_r;
    float ib_g;
    float ib_b;
    float ib_a;

    float mv_a ;
    float mv_r ;
    float mv_g ;
    float mv_b ;
    float mv_l;
    float mv_x;
    float mv_y;
    float mv_dy;
    float mv_dx;

     /* PER_FRAME CONSTANTS END */

    float fRating;
    float fGammaAdj;
    float fVideoEchoZoom;
    float fVideoEchoAlpha;

    int nVideoEchoOrientation;
    int nWaveMode;
    int bAdditiveWaves;
    int bWaveDots;
    int bWaveThick;
    int bModWaveAlphaByVolume;
    int bMaximizeWaveColor;
    int bTexWrap;
    int bDarkenCenter;
    int bRedBlueStereo;
    int bBrighten;
    int bDarken;
    int bSolarize;
    int bInvert;
    int bMotionVectorsOn;


    float fWaveAlpha ;
    float fWaveScale;
    float fWaveSmoothing;
    float fWaveParam;
    float fModWaveAlphaStart;
    float fModWaveAlphaEnd;
    float fWarpAnimSpeed;
    float fWarpScale;
    float fShader;

    /* Q VARIABLES START */

    float q1;
    float q2;
    float q3;
    float q4;
    float q5;
    float q6;
    float q7;
    float q8;


    /* Q VARIABLES END */

    float **zoom_mesh;
    float **zoomexp_mesh;
    float **rot_mesh;

    float **sx_mesh;
    float **sy_mesh;
    float **dx_mesh;
    float **dy_mesh;
    float **cx_mesh;
    float **cy_mesh;

    float **x_mesh;
    float **y_mesh;
    float **rad_mesh;
    float **theta_mesh;

};


class Preset {
protected:	

	/// @idea dynamically allocate this instead (allow pointer to one in constructor, or blank to 
	/// allocate separately)
	/// Benefits: lighter weight presets, but "heavier" ones are still supported too
	/// Negatives: heap is slower, extra pointer dereference 
	EngineVars engineVars;
	 /* preset name as parsed in file */

public:

    /** Default constructor is *not* a properly initalized preset **/
    Preset();

    /** Load a preset by filename. Most common way to allocate new preset */
    Preset(const std::string & filename);

    ~Preset();


    std::string file_path;

    /** IDs of current waves/shapes */
    int interface_id,
   cwave_interface_id;
    CustomWave *active_wave;
    CustomShape *active_shape;

    void load_init_conditions();
  
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

    InitCond *get_init_cond( Param *param );

    void load_custom_wave_init_conditions();
    void load_custom_wave_init( CustomWave *customWave );
    void load_custom_shape_init_conditions();
    void load_custom_shape_init( CustomShape *customShape );

    void reloadPerFrame(char * s);
    void reloadPerFrameInit(char *s);
    void reloadPerPixel(char *s);

    static Preset *load_preset( const char *pathname );
    void savePreset(char * name);
    int write_preset_name( FILE *fs );
    int write_init_conditions( FILE *fs );
    int write_init( InitCond *initCond );
    int write_per_frame_init_equations( FILE *fs );
    int write_per_frame_equations( FILE *fs );
    int write_per_pixel_equations( FILE *fs );

    int destroy();

	/** Splaytree traversal helpers */
   inline void load_custom_shape_init_helper( void *custom_shape ) {
    ((CustomShape *)custom_shape)->load_custom_shape_init();
  }

};

#endif /** !_PRESET_HPP */
