/*
 * Shader.cpp
 *
 *  Created on: Jun 29, 2008
 *      Author: pete
 */

#include "Shader.hpp"

Shader::Shader() : language("hlsl"){}

void Shader::clear()
{
    language = "hlsl";
    programSource.clear();
};
