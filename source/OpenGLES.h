#pragma once

// EGL includes
#include <EGL/egl.h>
#include <EGL/eglplatform.h>

class OpenGLES {
public:
	OpenGLES();
	virtual ~OpenGLES();

	void Initialize();
	EGLSurface CreateSurface(winrt::Windows::Foundation::IInspectable const& panel, const winrt::Windows::Foundation::Size* renderSurfaceSize, const float* renderResolutionScale);
	void GetSurfaceDimensions(const EGLSurface surface, EGLint* width, EGLint* height);
	void DestroySurface(const EGLSurface surface);
	void MakeCurrent(const EGLSurface surface);
	EGLBoolean SwapBuffers(const EGLSurface surface);
	void Reset();
	void Cleanup();

private:
	EGLDisplay mEglDisplay;
	EGLContext mEglContext;
	EGLConfig mEglConfig;
};