#ifndef _INIT_COND_UTILS_HPP
#define _INIT_COND_UTILS_HPP
#include "SplayTree.hpp"
#include "InitCond.hpp"

namespace InitCondUtils {
class LoadUnspecInitCond {
	public:
	
	LoadUnspecInitCond(SplayTree<InitCond> & initCondTree, SplayTree<InitCond> & perFrameInitEqnTree):
		 m_initCondTree(initCondTree), m_perFrameInitEqnTree(perFrameInitEqnTree) {}

	void operator()(Param * param);

	private:
		SplayTree<InitCond> & m_initCondTree;
		SplayTree<InitCond> & m_perFrameInitEqnTree;
};


inline void LoadUnspecInitCond::operator() (Param * param) {

    InitCond * init_cond;
    CValue init_val;

    /* Don't count these parameters as initial conditions */
    if (param->flags & P_FLAG_READONLY)
        return;
    if (param->flags & P_FLAG_QVAR)
        return;
    if (param->flags & P_FLAG_TVAR)
        return;
    if (param->flags & P_FLAG_USERDEF)
        return;

    /* If initial condition was not defined by the preset file, force a default one
       with the following code */
    if ((init_cond = m_initCondTree.splay_find(param->name)) == NULL) {

        /* Make sure initial condition does not exist in the set of per frame initial equations */
        if ((init_cond = m_perFrameInitEqnTree.splay_find(param->name)) != NULL)
            return;

        if (param->type == P_TYPE_BOOL)
            init_val.bool_val = 0;

        else if (param->type == P_TYPE_INT)
            init_val.int_val = *(int*)param->engine_val;

        else if (param->type == P_TYPE_DOUBLE)
            init_val.float_val = *(float*)param->engine_val;

        //printf("%s\n", param->name);
        /* Create new initial condition */
        if ((init_cond = new InitCond(param, init_val)) == NULL)
            return;

        /* Insert the initial condition into this presets tree */
        if (m_initCondTree.splay_insert(init_cond, init_cond->param->name) < 0) {
            delete init_cond;
            return;
        }
    }


}
}
#endif
