/** ParamUtils.hpp:
 *    A collection of utility functions to make using parameter types easier.
 *    In reality, this stuff belongs elsewhere, but one step at a time
 */
#ifndef _PARAM_UTILS_HPP
#define _PARAM_UTILS_HPP

#include "Param.h"
#include "SplayTree.hpp"

class ParamUtils {
public:

static const int AUTO_CREATE = 1;

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

};
#endif
