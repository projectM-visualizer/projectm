//
// C++ Implementation: PresetFactoryManager
//
// Description: 
//
//
// Author: Carmelo Piccione <carmelo.piccione@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef __PRESET_FACTORY_MANAGER_HPP
#define __PRESET_FACTORY_MANAGER_HPP
#include "PresetFactory.hpp"


class PresetFactoryException : public std::exception
{
	public:
		inline PresetFactoryException(const std::string & message) : _message(message) {}
		virtual ~PresetFactoryException() throw() {}
		const std::string & message() const { return _message; } 

	private:	
		std::string _message;
};

class PresetFactoryManager {

	public:
		PresetFactoryManager();
		~PresetFactoryManager();

		void initialize(int gx, int gy);
		
		/// Requests a factory given a preset extension type
		/// \param extension a string denoting the preset suffix type
		/// \throws a PresetFactoryException if the extension is unhandled
		PresetFactory & factory(const std::string & extension);

	private:
		int _gx, _gy;				
		mutable std::map<std::string, PresetFactory *> _factoryMap;
		mutable std::vector<PresetFactory *> _factoryList;
		void registerFactory(const std::string & extension, PresetFactory * factory);

};
#endif
