#pragma once
#include "angle.h"
#include <string>
#include <future>

GLuint CompileShader(GLenum type, const std::string& source);
GLuint CompileProgram(const std::string& vsSource, const std::string& fsSource);
std::future<std::vector<unsigned char>> ReadDataAsync(const std::wstring& filename);
