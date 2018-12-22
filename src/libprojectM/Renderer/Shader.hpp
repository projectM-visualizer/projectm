/*
 * Shader.hpp
 *
 *  Created on: Jun 29, 2008
 *      Author: pete
 */

#ifndef SHADER_HPP_
#define SHADER_HPP_

#include <string>
#include <map>
#include "Texture.hpp"

class Shader
{
public:

    std::map<std::string, TextureSamplerDesc> textures;

    std::string language;
    std::string programSource;
    std::string presetPath;

	Shader();
	void clear();	// clear before parse (doesn't touch textures)
};

#endif /* SHADER_HPP_ */
