/** ParamUtils.hpp:
 *    A collection of utility functions to make using parameter types easier.
 *    In reality, this stuff belongs elsewhere, but one step at a time
 */
#ifndef _PARAM_UTILS_HPP
#define _PARAM_UTILS_HPP

#include "Param.hpp"
#include <map>
#include <cassert>


class BuiltinParams;

class ParamUtils
{
public:
  static bool insert(Param * param, std::map<std::string,Param*> * paramTree)
  {

    assert(param);
    assert(paramTree);

    
    return ((paramTree->insert(std::make_pair(param->name,param))).second);

  }

  static const int AUTO_CREATE = 1;
  static const int NO_CREATE = 0;

  template <int FLAGS>
  static Param * find(std::string name, std::map<std::string,Param*> * paramTree)
  {

    assert(paramTree);

    Param * param = NULL;

    /* First look in the builtin database */
    std::map<std::string,Param*>::iterator pos = paramTree->find(name);
		

    if ((FLAGS == AUTO_CREATE) && ((pos == paramTree->end())))
    {
      /* Check if string is valid */
      if (!Param::is_valid_param_string(name.c_str()))
        return NULL;

      /* Now, create the user defined parameter given the passed name */
      if ((param = new Param(name)) == NULL)
        return NULL;

      /* Finally, insert the new parameter into this preset's proper splaytree */
      std::pair<std::map<std::string,Param*>::iterator, bool>  insertRetPair = 
		paramTree->insert(std::make_pair(param->name, param));

	if (insertRetPair.second)
		param = insertRetPair.first->second;
	
    } else 
	param = pos->second;
	

    /* Return the found (or created) parameter. Note that this could be null */
    return param;


  }

  /// Checks attempt
  static Param * find(const std::string & name, BuiltinParams * builtinParams, std::map<std::string,Param*> * insertionTree)
  {

    Param * param;

    // Check first db
    if ((param = builtinParams->find_builtin_param(name)) != 0)
      return param;

    // Check second db, create if necessary
    return find<AUTO_CREATE>(name, insertionTree);

  }

  class LoadInitCondFunctor
  {
  public:
    LoadInitCondFunctor(Preset * preset) :m_preset(preset) {}

    void operator() (Param * param)
    {

      InitCond * init_cond;
      CValue init_val;

      /* Don't count read only parameters as initial conditions */
      if (param->flags & P_FLAG_READONLY)
        return;

      /* If initial condition was not defined by the preset file, force a default one
         with the following code */
      std::map<std::string,InitCond*>::iterator pos;
      if ((pos = (m_preset->init_cond_tree->find(param->name))) == m_preset->init_cond_tree->end())
      {

        std::map<std::string,InitCond*>::iterator per_frame_init_pos;
        /* Make sure initial condition does not exist in the set of per frame initial equations */
        if ((per_frame_init_pos = (m_preset->per_frame_init_eqn_tree->find(param->name))) != m_preset->per_frame_init_eqn_tree->end())
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
        /// @bug not error checking
        m_preset->init_cond_tree->insert(std::make_pair(init_cond->param->name,init_cond));
      }

    }

  private :
    Preset * m_preset;
  };

};

#endif
