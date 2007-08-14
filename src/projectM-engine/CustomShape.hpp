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
 * Encapsulation of a custom shape
 *
 * $Log$
 */

#ifndef _CUSTOM_SHAPE_H
#define _CUSTOM_SHAPE_H

#define CUSTOM_SHAPE_DEBUG 0
#include <map>
#include "Param.hpp"
#include "PerFrameEqn.hpp"
#include "InitCond.hpp"

class Preset;


class CustomShape {
public:
    /* Numerical id */
    int id;
    int per_frame_count;

    /* Parameter tree associated with this custom shape */
    std::map<std::string,Param*> param_tree;

    /* Engine variables */
    int sides;
    int thickOutline;
    int enabled;
    int additive;
    int textured;
    
    float tex_zoom;
    float tex_ang;
      
    float x; /* x position for per point equations */
    float y; /* y position for per point equations */
    float radius;
    float ang;
    
    float r; /* red color value */
    float g; /* green color value */
    float b; /* blue color value */
    float a; /* alpha color value */
     
    float r2; /* red color value */
    float g2; /* green color value */
    float b2; /* blue color value */
    float a2; /* alpha color value */
    
    float border_r; /* red color value */
    float border_g; /* green color value */
    float border_b; /* blue color value */
    float border_a; /* alpha color value */
    
    /* stupid t variables */
    float t1;
    float t2;
    float t3;
    float t4;
    float t5;
    float t6;
    float t7;
    float t8;
    
    /* Data structure to hold per frame  / per frame init equations */
    std::map<std::string,InitCond*>  init_cond_tree;
    std::map<int, PerFrameEqn*>  per_frame_eqn_tree;
    std::map<std::string,InitCond*>  per_frame_init_eqn_tree;
    
    /* Denotes the index of the last character for each stdring buffer */
    int per_frame_eqn_string_index;
    int per_frame_init_eqn_string_index;
    

    /* String buffers for per frame / per frame init equations */
    char per_frame_eqn_string_buffer[STRING_BUFFER_SIZE];
    char per_frame_init_eqn_string_buffer[STRING_BUFFER_SIZE];

    /* Per point equation array */
    CustomShape( int id );
    ~CustomShape();

    /** Checks all internal trees are built correctly */
    int checkTrees();

    void load_custom_shape_init();
    void evalInitConds();
  };


#endif /** !_CUSTOM_SHAPE_H */

