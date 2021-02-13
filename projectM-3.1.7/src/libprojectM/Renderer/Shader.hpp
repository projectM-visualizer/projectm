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

    std::string programSource;
    std::string presetPath;

	Shader();
};

#endif /* SHADER_HPP_ */
