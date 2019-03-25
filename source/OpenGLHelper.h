#pragma once
#include "angle.h"
#include <string>

GLuint CompileShader(GLenum type, const std::string& source);
GLuint CompileProgram(const std::string& vsSource, const std::string& fsSource);
