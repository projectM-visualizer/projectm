#ifndef _INIT_COND_UTILS_HPP
#define _INIT_COND_UTILS_HPP
#include <map>
#include "InitCond.hpp"
#include <cstring>
#include <iostream>
#include <stdlib.h>

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


inline bool ends_with(std::string str, const char *match)
{
    size_t len = strlen(match);
    if (str.length() < len)
        return false;
    return 0 == strcmp(str.c_str() + str.length() - len, match);
}

inline void LoadUnspecInitCond::operator() (Param * param) {

    InitCond * init_cond = 0;
    CValue init_val;

    assert(param);
//    assert(param->engine_val);


    /* Don't count these parameters as initial conditions */
    if (param->flags & P_FLAG_READONLY)
        return;
    if (param->flags & P_FLAG_QVAR)
        return;
//    if (param->flags & P_FLAG_TVAR)
 //       return;
    if (param->flags & P_FLAG_USERDEF)
        return;
    if (param->flags & P_FLAG_RGB)
        return;

    /* If initial condition was not defined by the preset file, force a default one
       with the following code */

    if (m_initCondTree.find(param->name) == m_initCondTree.end()) {

        /* Make sure initial condition does not exist in the set of per frame initial equations */
	if (m_perFrameInitEqnTree.find(param->name) != m_perFrameInitEqnTree.end())
		return;

	/* If an _rgb or _rgba init_cond is specified, we want to act as if the corresponding _r, _g _b, and _a
	 * init conditions were explicitly specified.  That is we don't want the default or "unspecified" init conditions
	 * added to m_initCondTree.
	 */
	std::string param_name = param->name;
	std::string suffix;
	if (ends_with(param_name, "2"))
    {
	    param_name = param_name.substr(0,param_name.length()-1);
	    suffix = "2";
    }
    if (ends_with(param_name, "_r") || ends_with(param_name,"_g") || ends_with(param_name,"_b"))
    {
        std::string param_rgb = param_name.substr(0,param_name.length()-2) + "_rgb" + suffix;
        std::string param_rgba = param_name.substr(0,param_name.length()-2) + "_rgba" + suffix;
        if (m_initCondTree.find(param_rgba) != m_initCondTree.end() || m_initCondTree.find(param_rgb) != m_initCondTree.end() ||
            m_perFrameInitEqnTree.find(param_rgba) != m_perFrameInitEqnTree.end() || m_perFrameInitEqnTree.find(param_rgb) != m_perFrameInitEqnTree.end())
            return;
    }
    if (ends_with(param_name, "_a"))
    {
        std::string param_rgba = param_name.substr(0,param_name.length()-2) + "_rgba" + suffix;
        if (m_initCondTree.find(param_rgba) != m_initCondTree.end() || m_perFrameInitEqnTree.find(param_rgba) != m_perFrameInitEqnTree.end())
            return;
    }

	// Set an initial value via correct union member
        if (param->type == P_TYPE_BOOL)
            init_val = param->default_init_val.bool_val();
        else if (param->type == P_TYPE_INT)
            init_val = param->default_init_val.int_val();

        else if (param->type == P_TYPE_DOUBLE) {
           		init_val = param->default_init_val.float_val();
	}

        //printf("%s\n", param->name);
        /* Create new initial condition */
	//std::cerr << "[InitCondUtils] creating an unspecified initial condition of name " << param->name << std::endl;
        if ((init_cond = new InitCond(param, init_val)) == NULL) {
	    abort();
        }

        /* Insert the initial condition into this presets tree */
	std::pair<std::map<std::string, InitCond*>::iterator, bool> inserteePair =
		m_initCondTree.insert(std::make_pair(init_cond->param->name, init_cond));
	assert(inserteePair.second);
	assert(inserteePair.first->second);
    } else
	assert(m_initCondTree.find(param->name)->second);


}
}
#endif
