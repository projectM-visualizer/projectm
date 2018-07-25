/*
 * HLSLTranslator
 *
 * Translate HLSL (aka directX shader lang) to GLSL.
 * Needed because the presets all contain directX-style shaders, and we want GLSL so we can run on things that aren't windows computers.
 *
 * Also worth looking into generating SPIR-V for Vulkan support, someday.
 *
 *  Created on: May 27, 2018
 *      Author: mischa spiegelmock
 *
 * actual conversion is done by: https://github.com/Thekla/hlslparser
 */

#ifndef HLSLTRANSLATOR_HPP_
#define HLSLTRANSLATOR_HPP_

#include "Common.hpp"
#include "projectM-opengl.h"
#include "HLSLParser.h"
#include "GLSLGenerator.h"
#include <sys/types.h>
#include "projectM-opengl.h"
#include <iostream>
#include <memory>
#include <cstdlib>

class HLSLTranslator {
 public:
    std::string parse(const std::string & shaderType, const char *fileName, const std::string &fullSource);
};

#endif
