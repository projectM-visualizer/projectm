/*
 * Shader.hpp
 *
 *  Created on: Jun 29, 2008
 *      Author: pete
 */

#ifndef SHADER_HPP_
#define SHADER_HPP_

#include <string>

class Shader
{
public:
	float blur1n;
	float blur2n;
	float blur3n;
	float blur1x;
	float blur2x;
	float blur3x;
	float blur1ed;

	std::string warp;
	std::string composite;

	Shader();
};

#endif /* SHADER_HPP_ */
