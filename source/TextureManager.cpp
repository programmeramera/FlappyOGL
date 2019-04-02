#include "pch.h"
#include "TextureManager.h"
#include "OpenGLHelper.h"

using namespace std;
using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Storage;
using namespace Windows::Storage::Streams;
using namespace Windows::Graphics::Imaging;

IAsyncAction TextureManager::LoadTexturesAsync(std::vector<std::wstring> filenames) {
	for (auto const& filename: filenames)
	{
		Texture2D texture = co_await LoadTextureAsync(filename);
		mTextures[filename] =  texture;
	}
	mIsLoaded = true;
}

GLuint TextureManager::CreateTexture() {
	// Texture object handle
	GLuint textureId;

	// Use tightly packed data
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	CheckOpenGLError();

	// Generate a texture object
	glGenTextures(1, &textureId);
	CheckOpenGLError();

	return textureId;
}

void TextureManager::BindTexture(GLuint textureId, GLubyte* pixels, GLsizei width, GLsizei height)
{
	// Bind the texture object
	glBindTexture(GL_TEXTURE_2D, textureId);
	CheckOpenGLError();

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	CheckOpenGLError();

	// Set the filtering mode
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	CheckOpenGLError();
}

IAsyncOperation<IStorageFile> TextureManager::LoadImageAsync(const wstring& filename) {
	auto folder = Windows::ApplicationModel::Package::Current().InstalledLocation();
	auto path = folder.Path().c_str();
	auto file = co_await folder.TryGetItemAsync(filename);
	return file.as<IStorageFile>();
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

future<GLubyte*> TextureManager::GetPixelsFromPixelDataProvider(const PixelDataProvider& pixelDataProvider) {
	auto dpPixels = pixelDataProvider.DetachPixelData();
	auto size = dpPixels.size();
	GLubyte* pixels = new GLubyte[size];
	std::vector<unsigned char> vPixels(dpPixels.begin(), dpPixels.end());
	memcpy(pixels, &(vPixels[0]), size);
	co_return pixels;
}

future<Texture2D> TextureManager::LoadTextureAsync(std::wstring filename) {
	// Load the texture
	Texture2D texture;
	int width, height;
	GLuint textureIndex = CreateTexture();

	// Load file
	auto folder = Windows::ApplicationModel::Package::Current().InstalledLocation();
	auto path = folder.Path().c_str();
	auto item = co_await folder.TryGetItemAsync(filename);
	auto file = item.as<IStorageFile>();

	// Get PixelDataProvider
	auto stream = co_await file.OpenAsync(FileAccessMode::Read);
	auto decoder = co_await BitmapDecoder::CreateAsync(stream);
	auto bitmap = co_await decoder.GetSoftwareBitmapAsync();
	width = bitmap.PixelWidth();
	height = bitmap.PixelHeight();
	auto pixelData = co_await decoder.GetPixelDataAsync(BitmapPixelFormat::Rgba8, BitmapAlphaMode::Straight, BitmapTransform(), ExifOrientationMode::IgnoreExifOrientation, ColorManagementMode::DoNotColorManage);

	// Get Pixels
	auto dpPixels = pixelData.DetachPixelData();
	auto size = dpPixels.size();

	GLubyte* pixels = new GLubyte[size];
	//std::vector<unsigned char> vPixels(dpPixels.begin(), dpPixels.end());
	memcpy(pixels, &(dpPixels[0]), size);
	
	BindTexture(textureIndex, pixels, width, height);
	
	delete (pixels);

	texture.TextureIndex = textureIndex;
	texture.Name = filename;
	texture.Width = width;
	texture.Height = height;
	co_return texture;
}

Texture2D TextureManager::GetTexture(std::wstring filename) {
	return mTextures[filename];
}