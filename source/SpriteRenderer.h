#pragma once

#include <future>
#include "angle.h"

namespace Angle
{
    class SpriteRenderer
    {
    public:
		SpriteRenderer();
        ~SpriteRenderer();
		std::future<GLuint> InitializeAsync();
        void Draw();
        void UpdateWindowSize(GLsizei width, GLsizei height);
		void UpdateRotation(int delta);

    private:
        GLuint mProgram;
        GLsizei mWindowWidth;
        GLsizei mWindowHeight;

		// Vertex shader parameters
        GLint mVertexAttribLocation;
		GLint mUVAttribLocation;
		GLint mSpriteRectUniformLocation;
		GLint mSpriteWorldUniformLocation;
		GLint mTextureSizeUniformLocation;

		// Fragment shader parameters
		GLint mTextureUniformLocation;

        GLuint mVertexPositionBuffer;
        GLuint mVertexUVBuffer;
        //GLuint mIndexBuffer;

		GLuint mTextureIndex;

        int mDrawCount;
    };
}