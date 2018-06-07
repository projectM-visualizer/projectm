#ifndef ShaderContext_hpp
#define ShaderContext_hpp

#include "projectM-opengl.h"

// information needed for communicating with shaders

class ShaderContext {
public:
    GLuint positionAttribute, colorAttribute;
};

#endif /* ShaderContext_hpp */
