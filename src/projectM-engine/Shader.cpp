/*
 * Shader.cpp
 *
 *  Created on: Jun 29, 2008
 *      Author: pete
 */

#include "Shader.hpp"

Shader::Shader()
	: version(1), warp_version(1), composite_version(1),
	  blur1n(1), blur2n(1), blur3n(1),
	  blur1x(1), blur2x(1), blur3x(1),
	  blur1ed(1){}
