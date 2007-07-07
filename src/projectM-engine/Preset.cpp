/**
 * projectM -- Milkdrop-esque visualisation SDK
 * Copyright (C)2003-2004 projectM Team
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#ifdef WIN32
#include "win32-dirent.h"
#else
#include <dirent.h>
#endif /** WIN32 */
#include <time.h>

#include "Preset.hpp"
#include "Parser.h"
#include "ParamUtils.hpp"



Preset::Preset(const std::string & filename, const PresetInputs & presetInputs, PresetOutputs & presetOutputs):
	file_path(filename),
	builtinParams(presetInputs, presetOutputs),
	customWaves(&presetOutputs.customWaves),
	customShapes(&presetOutputs.customShapes)
{

initialize(filename);

}

Preset::~Preset() {

#if defined(PRESET_DEBUG) && defined(DEBUG)
    DWRITE( "~preset(): in\n" );
#endif



  init_cond_tree->splay_traverse((void (*)(void*))free_init_cond_helper);
  delete init_cond_tree;

  per_frame_init_eqn_tree->splay_traverse((void (*)(void*))free_init_cond_helper);
  delete per_frame_init_eqn_tree;

  per_pixel_eqn_tree->splay_traverse((void (*)(void*))free_per_pixel_eqn_helper);
  delete per_pixel_eqn_tree;

  per_frame_eqn_tree->splay_traverse((void (*)(void*))free_per_frame_eqn_helper);
  delete per_frame_eqn_tree;
   
  user_param_tree->traverse<SplayTreeFunctors::Delete<Param> >();
  delete user_param_tree;

  custom_wave_tree->splay_traverse((void (*)(void*))free_custom_wave_helper);
  delete custom_wave_tree;

  custom_shape_tree->splay_traverse((void (*)(void*))free_custom_shape_helper);
  delete custom_shape_tree;
 
#if defined(PRESET_DEBUG) && defined(DEBUG)
    DWRITE( "~Preset(): out\n" );
#endif


}

void Preset::evalCustomShapeInitConditions() {
    custom_shape_tree->splay_traverse( eval_custom_shape_init_conds_helper );
  }

void Preset::evalCustomWaveInitConditions() {
    custom_wave_tree->splay_traverse( eval_custom_wave_init_conds_helper );
  }


void Preset::evalCustomWavePerFrameEquations() {

    for (PresetOutputs::cwave_container::iterator pos = customWaves->begin(); pos != customWaves->end(); ++pos) {
    	(*pos)->init_cond_tree->splay_traverse((void (*)(void*))eval_init_cond_helper);
    	(*pos)->per_frame_eqn_tree->splay_traverse((void (*)(void*))eval_per_frame_eqn_helper);
   }

}

void Preset::evalCustomShapePerFrameEquations() {

    for (PresetOutputs::cshape_container::iterator pos = customShapes->begin(); pos != customShapes->end(); ++pos) {
    	(*pos)->init_cond_tree->splay_traverse((void (*)(void*))eval_init_cond_helper);
    	(*pos)->per_frame_eqn_tree->splay_traverse((void (*)(void*))eval_per_frame_eqn_helper);
   }

}

void Preset::evalInitConditions() {
  per_frame_init_eqn_tree->splay_traverse((void (*)(void*))eval_init_cond_helper);
}

void Preset::evalPerFrameEquations() {
  init_cond_tree->splay_traverse((void (*)(void*))eval_init_cond_helper);
  per_frame_eqn_tree->splay_traverse((void (*)(void*))eval_per_frame_eqn_helper);
}


void Preset::reloadPerPixel(char *s) {

  int slen;

  if (s == NULL)
    return;

  /* Clear previous per pixel equations */
  per_pixel_eqn_tree->splay_traverse((void (*)(void*))free_per_pixel_eqn_helper);
  delete per_pixel_eqn_tree;
  per_pixel_eqn_tree = SplayTree<PerPixelEqn>::create_splaytree
	((int (*)(const void*,const void*))SplayKeyFunctions::compare_int, (void* (*)(void*))SplayKeyFunctions::copy_int, (void (*)(void*))SplayKeyFunctions::free_int);

  /* Convert string to a stream */
#if !defined(MACOS) && !defined(WIN32)
	{
		FILE* fs = fmemopen (s, strlen(s), "r");
		char c;
		
		while ((c = fgetc(fs)) != EOF) {
			ungetc(c, fs);
			//FIXME Parser::parse_per_pixel_eqn(fs, preset, 0);
		}

		fclose(fs);
	}
#else
printf( "reloadPerPixel()\n" );
#endif

  /* Clear string space */
  memset(per_pixel_eqn_string_buffer, 0, STRING_BUFFER_SIZE);

  /* Compute length of string */
  slen = strlen(s);

  /* Copy new string into buffer */
  strncpy(per_pixel_eqn_string_buffer, s, slen);

  /* Yet again no bounds checking */
  per_pixel_eqn_string_index = slen;

  /* Finished */
 
  return;
}

/* Obviously unwritten */
void Preset::reloadPerFrameInit(char *s) {

}

void Preset::reloadPerFrame(char * s) {

  int slen;
  int eqn_count = 1;

  if (s == NULL)
    return;

  /* Clear previous per frame equations */
  per_frame_eqn_tree->splay_traverse((void (*)(void*))free_per_frame_eqn_helper );
  delete per_frame_eqn_tree;
  per_frame_eqn_tree = SplayTree<PerFrameEqn>::create_splaytree((int (*)(const void*,const void*))SplayKeyFunctions::compare_int,(void* (*)(void*)) SplayKeyFunctions::copy_int, (void (*)(void*))SplayKeyFunctions::free_int);

  /* Convert string to a stream */
  //FIXME
#if !defined(MACOS) && !defined(WIN32) && !defined(LINUX)
	{
		FILE* fs = fmemopen (s, strlen(s), "r");
		char c;
		
		while ((c = fgetc(fs)) != EOF) {
			per_frame_eqn_t * per_frame;
			ungetc(c, fs);
			if ((per_frame = Parser::parse_per_frame_eqn(fs, eqn_count, preset)) != NULL) {
				splay_insert(per_frame, &eqn_count, this->per_frame_eqn_tree);
				eqn_count++;
			}
		}
		fclose(fs);
	}
#else
printf( "reloadPerFrame()\n" );
#endif

  /* Clear string space */
  memset(per_frame_eqn_string_buffer, 0, STRING_BUFFER_SIZE);

  /* Compute length of string */
  slen = strlen(s);

  /* Copy new string into buffer */
  strncpy(per_frame_eqn_string_buffer, s, slen);

  /* Yet again no bounds checking */
  per_frame_eqn_string_index = slen;

  /* Finished */
  printf("reloadPerFrame: %d eqns parsed succesfully\n", eqn_count-1);
  return;

}


void Preset::initialize(const std::string & pathname) {

  /* Initialize equation trees */
  init_cond_tree = SplayTree<InitCond>::create_splaytree((int (*)(const void*,const void*))SplayKeyFunctions::compare_string, (void* (*)(void*))SplayKeyFunctions::copy_string, (void (*)(void*))SplayKeyFunctions::free_string);
  this->user_param_tree = SplayTree<Param>::create_splaytree((int (*)(const void*,const void*))SplayKeyFunctions::compare_string,(void* (*)(void*)) SplayKeyFunctions::copy_string,  (void (*)(void*))SplayKeyFunctions::free_string);
  this->per_frame_eqn_tree = SplayTree<PerFrameEqn>::create_splaytree((int (*)(const void*,const void*))SplayKeyFunctions::compare_int,(void* (*)(void*)) SplayKeyFunctions::copy_int, (void (*)(void*)) SplayKeyFunctions::free_int);
  this->per_pixel_eqn_tree = SplayTree<PerPixelEqn>::create_splaytree((int (*)(const void*,const void*))SplayKeyFunctions::compare_int,(void* (*)(void*)) SplayKeyFunctions::copy_int, (void (*)(void*))SplayKeyFunctions::free_int);
  this->per_frame_init_eqn_tree = SplayTree<InitCond>::create_splaytree((int (*)(const void*,const void*))SplayKeyFunctions::compare_string,(void* (*)(void*)) SplayKeyFunctions::copy_string, (void (*)(void*)) SplayKeyFunctions::free_string);
  this->custom_wave_tree = SplayTree<CustomWave>::create_splaytree((int (*)(const void*,const void*))SplayKeyFunctions::compare_int, (void* (*)(void*))SplayKeyFunctions::copy_int, (void (*)(void*)) SplayKeyFunctions::free_int);
  this->custom_shape_tree = SplayTree<CustomShape>::create_splaytree((int (*)(const void*,const void*))SplayKeyFunctions::compare_int, (void* (*)(void*))SplayKeyFunctions::copy_int, (void (*)(void*))SplayKeyFunctions::free_int);

  memset(this->per_pixel_flag, 0, sizeof(int)*NUM_OPS);

  /* Set initial index values */
  this->per_pixel_eqn_string_index = 0;
  this->per_frame_eqn_string_index = 0;
  this->per_frame_init_eqn_string_index = 0;
  
  /* Clear string buffers */
  memset(this->per_pixel_eqn_string_buffer, 0, STRING_BUFFER_SIZE);
  memset(this->per_frame_eqn_string_buffer, 0, STRING_BUFFER_SIZE);
  memset(this->per_frame_init_eqn_string_buffer, 0, STRING_BUFFER_SIZE);
  int retval;
  if ((retval = load_preset_file(pathname.c_str())) < 0) {
#ifdef PRESET_DEBUG
	if (PRESET_DEBUG) printf("load_preset: failed to load file \"%s\"\n", pathname);
#endif
	//this->close_preset();
	/// @bug how should we handle this problem? a well define exception?
	throw retval;
  }

  /* It's kind of ugly to reset these values here. Should definitely be placed in the parser somewhere */
  this->per_frame_eqn_count = 0;
  this->per_frame_init_eqn_count = 0;

}

/// @bug broken / unimplemented
void Preset::savePreset(char * filename) {

return;

#ifdef PANTS
  FILE * fs;

  if (filename == NULL)
    return;
  
  /* Open the file corresponding to pathname */
  if ((fs = fopen(filename, "w+")) == 0) {
#ifdef PRESET_DEBUG
    if (PRESET_DEBUG) printf("savePreset: failed to create filename \"%s\"!\n", filename);
#endif
    return;	
  }

  write_stream = fs;

  if (write_preset_name(fs) < 0) {
    write_stream = NULL;
    fclose(fs);
    return;
  }

  if (write_init_conditions(fs) < 0) {
    write_stream = NULL;
    fclose(fs);
    return;
  }

  if (write_per_frame_init_equations(fs) < 0) {
    write_stream = NULL;
    fclose(fs);
    return;
  }

  if (write_per_frame_equations(fs) < 0) {
    write_stream = NULL;
    fclose(fs);
    return;
  }

  if (write_per_pixel_equations(fs) < 0) {
    write_stream = NULL;
    fclose(fs);
    return;
  }
 
  write_stream = NULL;
  fclose(fs);
#endif
}

int Preset::write_preset_name(FILE * fs) {

  char s[256];
  int len;

  memset(s, 0, 256);

  if (fs == NULL)
    return PROJECTM_FAILURE;

  /* Format the preset name in a string */
   sprintf(s, "[%s]\n", name);

  len = strlen(s);

  /* Write preset name to file stream */
  if (fwrite(s, 1, len, fs) != len)
    return PROJECTM_FAILURE;

  return PROJECTM_SUCCESS;

}

#ifdef NEEDS_MOVED
int Preset::write_init_conditions(FILE * fs) {

  if (fs == NULL)
    return PROJECTM_FAILURE;

  init_cond_tree->splay_traverse( (void (*)(void*))write_init);
  
  return PROJECTM_SUCCESS;
}

void Preset::write_init( InitCond * init_cond ) {

  char s[512];
  int len;

  if (write_stream == NULL)
    return;

  memset(s, 0, 512);

  if (init_cond->param->type == P_TYPE_BOOL)
    sprintf(s, "%s=%d\n", init_cond->param->name, init_cond->init_val.bool_val);

  else if (init_cond->param->type == P_TYPE_INT)    
    sprintf(s, "%s=%d\n", init_cond->param->name, init_cond->init_val.int_val);

  else if (init_cond->param->type == P_TYPE_DOUBLE)
    sprintf(s, "%s=%f\n", init_cond->param->name, init_cond->init_val.float_val);

  else { printf("write_init: unknown parameter type!\n"); return; }
  
  len = strlen(s);

  if ((fwrite(s, 1, len, write_stream)) != len)
    printf("write_init: failed writing to file stream! Out of disk space?\n");
  
}


int Preset::write_per_frame_init_equations(FILE * fs) {

  int len;

  if (fs == NULL)
    return PROJECTM_FAILURE;
  
  len = strlen(per_frame_init_eqn_string_buffer);

  if (fwrite(per_frame_init_eqn_string_buffer, 1, len, fs) != len)
    return PROJECTM_FAILURE;

  return PROJECTM_SUCCESS;
}


int Preset::write_per_frame_equations(FILE * fs) {

  int len;

  if (fs == NULL)
    return PROJECTM_FAILURE;

  len = strlen(per_frame_eqn_string_buffer);

  if (fwrite(per_frame_eqn_string_buffer, 1, len, fs) != len)
    return PROJECTM_FAILURE;

  return PROJECTM_SUCCESS;
}


int Preset::write_per_pixel_equations(FILE * fs) {

  int len;

  if (fs == NULL)
    return PROJECTM_FAILURE;

  len = strlen(per_pixel_eqn_string_buffer);

  if (fwrite(per_pixel_eqn_string_buffer, 1, len, fs) != len)
    return PROJECTM_FAILURE;

  return PROJECTM_SUCCESS;
}
#endif /** NEEDS_MOVED */

void Preset::load_custom_wave_init_conditions() {
  custom_wave_tree->splay_traverse((void (*)(void*))load_custom_wave_init_helper);
}

void Preset::load_custom_shape_init_conditions() {
  /// @bug  busted
   abort();
  //custom_shape_tree->splay_traverse((void (*)(void*))load_custom_shape_init_helper);
}




void Preset::evaluateFrame() {

	/* Evaluate all equation objects in same order as the renderer */

	evalInitConditions();
	evalPerFrameEquations();
	evalPerPixelEqns();
        evalInitConditions();
        evalCustomShapeInitConditions();
	evalCustomWavePerFrameEquations();
	evalCustomShapePerFrameEquations();

}

/** Evaluates all per-pixel equations */
void Preset::evalPerPixelEqns() {

    /* Evaluate all per pixel equations using splay traversal */
    per_pixel_eqn_tree->splay_traverse((void (*)(void*))eval_per_pixel_eqn_helper);

    /* Set mesh i / j values to -1 so engine vars are used by default again */
    this->mesh_i = -1;
    this->mesh_j = -1;
}

/** Is the opcode a per-pixel equation? */
int Preset::isPerPixelEqn( int op ) {
    return per_pixel_flag[op];
  }

/** Reset all per-pixel equation flags */
int Preset::resetPerPixelEqnFlags() {

    int i;

    for (i = 0; i < NUM_OPS;i++) {
        per_pixel_flag[i] = FALSE;
      }

    return PROJECTM_SUCCESS;
}

/* Adds a per pixel equation according to its string name. This
   will be used only by the parser */
int Preset::add_per_pixel_eqn(char * name, GenExpr * gen_expr) {

  PerPixelEqn * per_pixel_eqn = NULL;
  int index;
  Param * param = NULL;

  /* Argument checks */
  if (gen_expr == NULL)
	  return PROJECTM_FAILURE;
  if (name == NULL)
	  return PROJECTM_FAILURE;
  
 if (PER_PIXEL_EQN_DEBUG) printf("add_per_pixel_eqn: per pixel equation (name = \"%s\")\n", name);
 
 if (!strncmp(name, "dx", strlen("dx"))) 
   per_pixel_flag[DX_OP] = TRUE;
 else if (!strncmp(name, "dy", strlen("dy"))) 
   per_pixel_flag[DY_OP] = TRUE;
 else if (!strncmp(name, "cx", strlen("cx")))
   per_pixel_flag[CX_OP] = TRUE;
 else if (!strncmp(name, "cy", strlen("cy")))
   per_pixel_flag[CX_OP] = TRUE;
 else if (!strncmp(name, "zoom", strlen("zoom")))
   per_pixel_flag[ZOOM_OP] = TRUE;
 else if (!strncmp(name, "zoomexp", strlen("zoomexp"))) 
   per_pixel_flag[ZOOMEXP_OP] = TRUE;
 else if (!strncmp(name, "rot", strlen("rot")))
   per_pixel_flag[ROT_OP] = TRUE;
 else if (!strncmp(name, "sx", strlen("sx")))
   per_pixel_flag[SX_OP] = TRUE;
 else if (!strncmp(name, "sy", strlen("sy")))
   per_pixel_flag[SY_OP] = TRUE;
 
 /* Search for the parameter so we know what matrix the per pixel equation is referencing */

 param = Param::find_param(name, this, TRUE);
 if ( !param ) {
   if (PER_PIXEL_EQN_DEBUG) printf("add_per_pixel_eqn: failed to allocate a new parameter!\n");
   return PROJECTM_FAILURE;
 } 	 

/**
 if ( !param->matrix ) {
    if (PER_PIXEL_EQN_DEBUG) printf( "add_per_pixel_eqn: failed to locate param matrix\n" );
    return PROJECTM_FAILURE;
  }
*/

 /* Find most largest index in the splaytree */
 if ((per_pixel_eqn = (PerPixelEqn *)per_pixel_eqn_tree->splay_find_max()) == NULL)
  index = 0;
  else
 index = per_pixel_eqn_tree->splay_size();
   
 /* Create the per pixel equation given the index, parameter, and general expression */
 if ((per_pixel_eqn = PerPixelEqn::new_per_pixel_eqn(index, param, gen_expr)) == NULL) {
   if (PER_PIXEL_EQN_DEBUG) printf("add_per_pixel_eqn: failed to create new per pixel equation!\n");
   return PROJECTM_FAILURE;

 }

 if (PER_PIXEL_EQN_DEBUG) printf("add_per_pixel_eqn: new equation (index = %d,matrix=%X) (param = \"%s\")\n", 
				 per_pixel_eqn->index, per_pixel_eqn->param->matrix, per_pixel_eqn->param->name);
 /* Insert the per pixel equation into the preset per pixel database */
 if (per_pixel_eqn_tree->splay_insert(per_pixel_eqn, &per_pixel_eqn->index) < 0) {
   per_pixel_eqn->free_per_pixel_eqn();
   printf("failed to add per pixel eqn!\n");
   return PROJECTM_FAILURE;	 
 }

 /* Done */ 
 return PROJECTM_SUCCESS;
}

/** Finds / Creates (if necessary) initial condition associated with passed parameter */
InitCond * Preset::get_init_cond( Param *param ) {

  InitCond * init_cond;
  CValue init_val;

    if ( param == NULL ) {
        return NULL;
      }

  if ((init_cond = (InitCond*)(init_cond_tree->splay_find(param->name))) == NULL) {

    if (param->type == P_TYPE_BOOL)
      init_val.bool_val = 0;

    else if (param->type == P_TYPE_INT)
      init_val.int_val = *(int*)param->engine_val;

    else if (param->type == P_TYPE_DOUBLE)
      init_val.float_val = *(float*)param->engine_val;

    /* Create new initial condition */
    if ((init_cond = new InitCond(param, init_val)) == NULL)
      return NULL;

    /* Insert the initial condition into this presets tree */
    if (init_cond_tree->splay_insert(init_cond, init_cond->param->name) < 0) {
      delete init_cond;
      return NULL;
    }
  }

  return init_cond;

}

/* load_preset_file: private function that loads a specific preset denoted
   by the given pathname */
int Preset::load_preset_file(const char * pathname) { 

  FILE * fs;
  int retval;
  int lineno;
   line_mode_t line_mode;

  if (pathname == NULL)
	  return PROJECTM_FAILURE;
  
  /* Open the file corresponding to pathname */
  if ((fs = fopen(pathname, "rb")) == 0) {
#if defined(PRESET_DEBUG) && defined(DEBUG)
    DWRITE( "load_preset_file: loading of file %s failed!\n", pathname);
#endif
    return PROJECTM_ERROR;	
  }

#if defined(PRESET_DEBUG) && defined(DEBUG)
    DWRITE( "load_preset_file: file stream \"%s\" opened successfully\n", pathname);
#endif

  /* Parse any comments */
  if (Parser::parse_top_comment(fs) < 0) {
#if defined(PRESET_DEBUG) && defined(DEBUG)
    DWRITE( "load_preset_file: no left bracket found...\n");
#endif
    fclose(fs);
    return PROJECTM_FAILURE;
  }

  /* Parse the preset name and a left bracket */
  char tmp_name[MAX_TOKEN_SIZE];

  if (Parser::parse_preset_name(fs, tmp_name) < 0) {
#if defined(PRESET_DEBUG) && defined(DEBUG)
    DWRITE( "load_preset_file: loading of preset name in file \"%s\" failed\n", pathname);
#endif
    fclose(fs);
    return PROJECTM_ERROR;
  }
  name = std::string(tmp_name);

#if defined(PRESET_DEBUG) && defined(DEBUG)
   DWRITE( "load_preset_file: preset \"%s\" parsed\n", this->name);
#endif

  /* Parse each line until end of file */
    lineno = 0;
#if defined(PRESET_DEBUG) && defined(DEBUG)
    DWRITE( "load_preset_file: beginning line parsing...\n");
#endif
  while ((retval = Parser::parse_line(fs, this)) != EOF) {
    if (retval == PROJECTM_PARSE_ERROR) {
	line_mode = NORMAL_LINE_MODE;
#if defined(PRESET_DEBUG) && defined(DEBUG)
    DWRITE( "load_preset_file: parse error in file \"%s\": line %d\n", pathname,lineno);
#endif
    }
    lineno++;
  }

#if defined(PRESET_DEBUG) && defined(DEBUG)
    DWRITE("load_preset_file: finished line parsing successfully\n"); 
#endif

  /* Now the preset has been loaded.
     Evaluation calls can be made at appropiate
     times in the frame loop */

 fclose(fs);
#if defined(PRESET_DEBUG) && defined(DEBUG)
    DWRITE("load_preset_file: file \"%s\" closed, preset ready\n", pathname);
#endif
  return PROJECTM_SUCCESS;
}


void Preset::load_init_conditions() {

   ParamUtils::LoadInitCondFunctor functor(this);
   builtinParams.traverse(functor);

}



CustomWave * Preset::find_custom_wave(int id, bool create_flag) {
  CustomWave * custom_wave = NULL;

  assert(customWaves);

  if ((custom_wave = (*customWaves)[id]) == NULL) {

    if (CUSTOM_WAVE_DEBUG) { printf("find_custom_wave: creating custom wave (id = %d)...", id);fflush(stdout);}

    if (create_flag == FALSE) {
      if (CUSTOM_WAVE_DEBUG) printf("you specified not to (create flag = false), returning null\n");
       return NULL;
    }

    if ((custom_wave = new CustomWave(id)) == NULL) {
      if (CUSTOM_WAVE_DEBUG) printf("failed...out of memory?\n");
      return NULL;
    }

    customWaves->push_back(custom_wave);
  }
 
  return custom_wave;

}


CustomShape * Preset::find_custom_shape(int id, bool create_flag) {

  CustomShape * custom_shape = NULL;
  assert(customShapes);
  if ((custom_shape = (*customShapes)[id]) == NULL) {

    if (CUSTOM_SHAPE_DEBUG) { printf("find_custom_shape: creating custom shape (id = %d)...", id);fflush(stdout);}

    if (create_flag == FALSE) {
      if (CUSTOM_SHAPE_DEBUG) printf("you specified not to (create flag = false), returning null\n");
      return NULL;
    }

    if ((custom_shape = new CustomShape(id)) == NULL) {
      if (CUSTOM_SHAPE_DEBUG) printf("failed...out of memory?\n");
      return NULL;
    }

    customShapes->push_back(custom_shape);

  }

  return custom_shape;
}

/* Find a parameter given its name, will create one if not found */
Param * Preset::find_param(char * name, int flags) {

    Param * param = NULL;

    /* Null argument checks */
    assert(name);

    /* First look in the builtin database */
    param = (Param *)this->builtinParams.find_builtin_param(name);

    /* If the search failed, check the user database */
    if (param == NULL) {
        param = (Param*)this->user_param_tree->splay_find(name);
    }

    /* If it doesn't exist in the user (or builtin) database and
    	  create_flag is set, then make it and insert into the database 
    */

    if ((param == NULL) && (flags & P_CREATE)) {

        /* Check if string is valid */
        if (!Param::is_valid_param_string(name)) {
            if (PARAM_DEBUG) printf("find_param: invalid parameter name:\"%s\"\n", name);
            return NULL;
        }
        /* Now, create the user defined parameter given the passed name */
        if ((param = new Param(name)) == NULL) {
            if (PARAM_DEBUG) printf("find_param: failed to create a new user parameter!\n");
            return NULL;
        }
        /* Finally, insert the new parameter into this preset's proper splaytree */
        if (this->user_param_tree->splay_insert(param, param->name) < 0) {
            if (PARAM_DEBUG) printf("PARAM \"%s\" already exists in user parameter tree!\n", param->name);
            delete param;
            return NULL;
        }

    }

    /* Return the found (or created) parameter. Note that if P_CREATE is not set, this could be null */
    return param;

}

