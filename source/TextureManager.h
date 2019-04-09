#pragma once
#include <future>
#include <map>
#include <string>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.Graphics.Imaging.h>
#include "angle.h"

struct Texture2D {
public:
	std::wstring Name;
    float Width;
    float Height;
    GLuint TextureIndex;
};

class TextureManager {

public:
	TextureManager(winrt::Windows::UI::Core::CoreDispatcher dispatcher)
		: mIsLoaded(false)
		, mDispatcher(dispatcher) { }

	winrt::fire_and_forget LoadTexturesAsync(std::vector<std::wstring> filename);
	Texture2D GetTexture(std::wstring filename);
	bool IsLoaded() { return mIsLoaded; }

private:
	GLuint CreateTexture(GLubyte* pixels, GLsizei width, GLsizei height);
	winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::Storage::IStorageFile> LoadImageAsync(const std::wstring& filename);
	winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::Graphics::Imaging::PixelDataProvider> GetPixelDataFromImageAsync(winrt::Windows::Storage::IStorageFile file, int& width, int& height);
	std::vector<GLubyte> GetPixelsFromPixelDataProvider(const winrt::Windows::Graphics::Imaging::PixelDataProvider& pixelDataProvider);
	//std::future<Texture2D> LoadTextureAsync(std::wstring filename);
	std::future<void> LoadTextureAsync(Texture2D& texture);

	GLuint GenerateTexture();
	void SetTexturePixels(GLuint textureId, GLsizei width, GLsizei height, GLubyte* pixels);

	bool mIsLoaded;
    std::map<std::wstring, Texture2D> mTextures;
	winrt::Windows::UI::Core::CoreDispatcher mDispatcher;
};
