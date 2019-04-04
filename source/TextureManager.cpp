#include "pch.h"
#include "TextureManager.h"
#include "OpenGLHelper.h"

using namespace std;
using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Storage;
using namespace Windows::Storage::Streams;
using namespace Windows::Graphics::Imaging;

future<void> TextureManager::LoadTexturesAsync(std::vector<std::wstring> filenames) {
	for (auto const& filename: filenames)
	{
		Texture2D texture = co_await LoadTextureAsync(filename);
		mTextures[filename] =  texture;
	}
	mIsLoaded = true;
}


GLuint TextureManager::CreateTexture(GLubyte* pixels, GLsizei width, GLsizei height)
{
	// Texture object handle
	GLuint textureId;

	//// Use tightly packed data
	//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// Generate a texture object
	glGenTextures(1, &textureId);
	CheckOpenGLError();

	// Bind the texture object
	glBindTexture(GL_TEXTURE_2D, textureId);
	glPixelStorei(GL_PACK_ALIGNMENT, textureId);
	glPixelStorei(GL_UNPACK_ALIGNMENT, textureId);
	CheckOpenGLError();

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	CheckOpenGLError();

	// Set the filtering mode
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	CheckOpenGLError();

	return textureId;
}

IAsyncOperation<IStorageFile> TextureManager::LoadImageAsync(const wstring& filename) {
	auto folder = Windows::ApplicationModel::Package::Current().InstalledLocation();
	auto path = folder.Path().c_str();
	auto file = co_await folder.TryGetItemAsync(filename);
	co_return file.as<IStorageFile>();
}

IAsyncOperation<PixelDataProvider> TextureManager::GetPixelDataFromImageAsync(IStorageFile file, int& width, int& height) {
	auto stream = co_await file.OpenAsync(FileAccessMode::Read);
	auto decoder = co_await BitmapDecoder::CreateAsync(stream);
	auto bitmap = co_await decoder.GetSoftwareBitmapAsync();
	width = bitmap.PixelWidth();
	height = bitmap.PixelHeight();
	auto pixelData = co_await decoder.GetPixelDataAsync(BitmapPixelFormat::Rgba8, BitmapAlphaMode::Straight, BitmapTransform(), ExifOrientationMode::IgnoreExifOrientation, ColorManagementMode::DoNotColorManage);
	co_return pixelData;
}

vector<GLubyte> TextureManager::GetPixelsFromPixelDataProvider(const PixelDataProvider& pixelDataProvider) {
	auto dpPixels = pixelDataProvider.DetachPixelData();
	auto size = dpPixels.size();
	std::vector<GLubyte> vPixels(dpPixels.begin(), dpPixels.end());
	return vPixels;
}

future<Texture2D> TextureManager::LoadTextureAsync(std::wstring filename) {
	Texture2D texture;
	int width, height;

	texture.Name = filename;

	// Load file
	auto file = co_await LoadImageAsync(filename);

	// Get PixelDataProvider
	auto pixelData = co_await GetPixelDataFromImageAsync(file, width, height);

	texture.Width = width;
	texture.Height = height;

	// Get Pixels
	auto dpPixels = GetPixelsFromPixelDataProvider(pixelData);
	auto size = dpPixels.size();

	auto pixels = new GLubyte[size];
	memcpy(pixels, &(dpPixels[0]), size);
	texture.TextureIndex = CreateTexture(pixels, width, height);
	delete (pixels);

	co_return texture;
}

Texture2D TextureManager::GetTexture(std::wstring filename) {
	return mTextures[filename];
}