/** ParamUtils.hpp:
 *    A collection of utility functions to make using parameter types easier.
 *    In reality, this stuff belongs elsewhere, but one step at a time
 */
#ifndef _PARAM_UTILS_HPP
#define _PARAM_UTILS_HPP

#include "Param.h"
#include "SplayTree.hpp"
#include <cassert>

class ParamUtils {
public:
    static int insert(Param * param, SplayTree<Param> * paramTree) {

	assert(param);
	assert(paramTree);


	return paramTree->splay_insert(param, param->name);
}

    static const int AUTO_CREATE = 1;
    static const int NO_CREATE = 0;

    template <int FLAGS>
    static Param * find(char * name, SplayTree<Param> * paramTree) {

        assert(name);

        Param * param = NULL;


        /* First look in the builtin database */
        param = (Param *)paramTree->splay_find(name);


        if (((FLAGS == AUTO_CREATE) && ((param = (Param *)paramTree->splay_find(name)) == NULL))) {

            /* Check if string is valid */
            if (!param->is_valid_param_string(name))
                return NULL;

            /* Now, create the user defined parameter given the passed name */
            if ((param = new Param(name)) == NULL)
                return NULL;

            /* Finally, insert the new parameter into this preset's proper splaytree */
            if (paramTree->splay_insert(param, param->name) < 0) {
                delete param;
                return NULL;
            }

        }

        /* Return the found (or created) parameter. Note that this could be null */
        return param;


    }



class LoadInitCondFunctor {
public:
    LoadInitCondFunctor(Preset * preset) :m_preset(preset) {}

    void operator() (Param * param) {

        InitCond * init_cond;
        CValue init_val;

        /* Don't count read only parameters as initial conditions */
        if (param->flags & P_FLAG_READONLY)
            return;

        /* If initial condition was not defined by the preset file, force a default one
           with the following code */
        if ((init_cond = (InitCond*)(m_preset->init_cond_tree->splay_find(param->name))) == NULL) {

            /* Make sure initial condition does not exist in the set of per frame initial equations */
            if ((init_cond = (InitCond*)(m_preset->per_frame_init_eqn_tree->splay_find(param->name))) != NULL)
                return;

            if (param->type == P_TYPE_BOOL)
                init_val.bool_val = 0;

            else if (param->type == P_TYPE_INT)
                init_val.int_val = *(int*)param->engine_val;

            else if (param->type == P_TYPE_DOUBLE)
                init_val.float_val = *(float*)param->engine_val;

            /* Create new initial condition */
            if ((init_cond = new InitCond(param, init_val)) == NULL)
                return;

            /* Insert the initial condition into this presets tree */
            if (m_preset->init_cond_tree->splay_insert(init_cond, init_cond->param->name) < 0) {
                delete init_cond;
                return;
            }

        }

    }

private :
    Preset * m_preset;
};

};

#endif
