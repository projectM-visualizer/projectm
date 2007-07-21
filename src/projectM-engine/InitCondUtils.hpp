#ifndef _INIT_COND_UTILS_HPP
#define _INIT_COND_UTILS_HPP
#include <map>
#include "InitCond.hpp"

namespace InitCondUtils {
class LoadUnspecInitCond {
	public:
	
	LoadUnspecInitCond(std::map<std::string,InitCond*> & initCondTree, std::map<std::string,InitCond*> & perFrameInitEqnTree):
		 m_initCondTree(initCondTree), m_perFrameInitEqnTree(perFrameInitEqnTree) {}

	void operator()(Param * param);

	private:
		std::map<std::string,InitCond*> & m_initCondTree;
		std::map<std::string,InitCond*> & m_perFrameInitEqnTree;
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

    if (m_initCondTree.find(param->name) == m_initCondTree.end()) {

        /* Make sure initial condition does not exist in the set of per frame initial equations */
	if (m_perFrameInitEqnTree.find(param->name) != m_perFrameInitEqnTree.end())
		return;

	// Set an initial vialue via correct union member
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
	/// @bug check the reult status of insert
	m_initCondTree.insert(std::make_pair(init_cond->param->name, init_cond));

    }


}
}
#endif
