
#pragma once

#include <iostream>
#include <projectM-opengl.h>

#define CHECK_GL_ERROR check_gl_error(__FILE__, __LINE__)


namespace libprojectM::Renderer {

void check_gl_error(const char* file, int line);

} // namespace libprojectM::Renderer
