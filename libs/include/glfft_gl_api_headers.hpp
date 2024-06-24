//
// This for GLFFT to use glad
//

#ifndef GLFFT_GL_API_HEADERS_HPP
#define GLFFT_GL_API_HEADERS_HPP

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#define GLFFT_GLSL_LANG_STRING "#version 430 core\n"
#define GLFFT_LOG_OVERRIDE printf
#define GLFFT_TIME_OVERRIDE glfwGetTime

#endif
