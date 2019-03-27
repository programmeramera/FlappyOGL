#include "pch.h"
#include "OpenGLHelper.h"
//// These are used by the shader compilation methods.
//#include <vector>
//#include <iostream>
//#include <fstream>
#include "winrt/Windows.ApplicationModel.h"
#include "winrt/Windows.Storage.Streams.h"

#include <filesystem>
#include <winrt/Windows.Graphics.Imaging.h>

using namespace std;
using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Storage;
using namespace Windows::Storage::Streams;
using namespace Windows::Graphics::Imaging;

GLuint CompileShader(GLenum type, const string& source)
{
	GLuint shader = glCreateShader(type);

	const char* sourceArray[1] = { source.c_str() };
	glShaderSource(shader, 1, sourceArray, NULL);
	glCompileShader(shader);

	GLint compileResult;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compileResult);

	if (compileResult == 0)
	{
		GLint infoLogLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

		std::vector<GLchar> infoLog(infoLogLength);
		glGetShaderInfoLog(shader, (GLsizei)infoLog.size(), NULL, infoLog.data());

		std::wstring errorMessage = std::wstring(L"Shader compilation failed: ");
		errorMessage += std::wstring(infoLog.begin(), infoLog.end());
		throw winrt::hresult_error(E_FAIL, winrt::hstring(errorMessage));
	}

	return shader;
}

GLuint CompileProgram(const string& vsSource, const string& fsSource)
{
	GLuint program = glCreateProgram();

	if (program == 0)
	{
		throw winrt::hresult_error(E_FAIL, winrt::hstring(L"Program creation failed"));
	}

	GLuint vs = CompileShader(GL_VERTEX_SHADER, vsSource);
	GLuint fs = CompileShader(GL_FRAGMENT_SHADER, fsSource);

	if (vs == 0 || fs == 0)
	{
		glDeleteShader(fs);
		glDeleteShader(vs);
		glDeleteProgram(program);
		return 0;
	}

	glAttachShader(program, vs);
	glDeleteShader(vs);

	glAttachShader(program, fs);
	glDeleteShader(fs);


	glLinkProgram(program);

	GLint linkStatus;
	glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
	
	if (linkStatus == 0)
	{
		GLint infoLogLength;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

		std::vector<GLchar> infoLog(infoLogLength);
		glGetProgramInfoLog(program, (GLsizei)infoLog.size(), NULL, infoLog.data());

		std::wstring errorMessage = std::wstring(L"Program link failed: ");
		errorMessage += std::wstring(infoLog.begin(), infoLog.end());
		throw winrt::hresult_error(E_FAIL, winrt::hstring(errorMessage));
	}

	return program;
}

future<vector<unsigned char>> ReadDataAsync(const wstring& filename)
{
	try {
		auto folder = winrt::Windows::ApplicationModel::Package::Current().InstalledLocation();
		auto file = co_await folder.GetFileAsync(winrt::hstring(filename.c_str()));
		IBuffer fileBuffer = co_await FileIO::ReadBufferAsync(file);
		std::vector<unsigned char> returnBuffer;
		returnBuffer.resize(fileBuffer.Length());
		DataReader::FromBuffer(fileBuffer).ReadBytes(winrt::array_view<unsigned char>(returnBuffer));
		co_return returnBuffer;
	}
	catch (...) {

	}
}

IAsyncOperation<PixelDataProvider> ReadImageAsync(const wstring& filename) {
	auto folder = Windows::ApplicationModel::Package::Current().InstalledLocation();
	auto path = folder.Path().c_str();
	auto file = co_await folder.TryGetItemAsync(filename);
	auto stream = co_await file.as<IStorageFile>().OpenAsync(FileAccessMode::Read);

	BitmapDecoder decoder = co_await BitmapDecoder::CreateAsync(stream);
	SoftwareBitmap bitmap = co_await decoder.GetSoftwareBitmapAsync();
	PixelDataProvider pixelData = co_await decoder.GetPixelDataAsync(BitmapPixelFormat::Rgba8,BitmapAlphaMode::Straight,BitmapTransform(),ExifOrientationMode::IgnoreExifOrientation, ColorManagementMode::DoNotColorManage);
	co_return pixelData;
}

void CheckOpenGLError() {
	GLenum err = glGetError();
	if (err != GL_NO_ERROR) {
		printf("OpenGL error %08x\n", err);
	}
}