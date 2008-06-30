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
	int blur1n;
	int blur2n;
	int blur3n;
	int blur1x;
	int blur2x;
	int blur3x;
	int blur1ed;

	std::string warp;
	std::string composite;

	Shader();
};

#endif /* SHADER_HPP_ */
