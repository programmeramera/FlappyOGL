#pragma once
#include "angle.h"
#include <string>
#include <future>
#include <vector>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Graphics.Imaging.h>

GLuint CompileShader(GLenum type, const std::string& source);
GLuint CompileProgram(const std::string& vsSource, const std::string& fsSource);
std::future<std::vector<unsigned char>> ReadDataAsync(const std::wstring& filename);
winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::Graphics::Imaging::PixelDataProvider> ReadImageAsync(const std::wstring& filename);
void CheckOpenGLError();
