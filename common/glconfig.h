#ifndef GL_CONFIG_H
#define GL_CONFIG_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#ifdef NDEBUG 
#define GL_ASSERT( gl_code ) gl_code
#else
#define GL_ASSERT( gl_code ) \
do{ \
    glGetError();\
    gl_code; \
    int __gl_error_code = glGetError(); \
    if (__gl_error_code != GL_NO_ERROR) \
    { \
        LOG_FATAL("OpenGL Error: " #gl_code ": 0x%x", __gl_error_code); \
    } \
} while (0)
#endif

inline bool isVAOSupported() { return glIsVertexArray != nullptr; }

#endif //GL_CONFIG_H
