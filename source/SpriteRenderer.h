#pragma once

#include "pch.h"

namespace Angle
{
    class SpriteRenderer
    {
    public:
		SpriteRenderer();
        ~SpriteRenderer();
        void Draw();
        void UpdateWindowSize(GLsizei width, GLsizei height);
		void UpdateRotation(int delta);

    private:
        GLuint mProgram;
        GLsizei mWindowWidth;
        GLsizei mWindowHeight;

        GLint mPositionAttribLocation;
        GLint mColorAttribLocation;

        GLint mModelUniformLocation;
        GLint mViewUniformLocation;
        GLint mProjUniformLocation;

        GLuint mVertexPositionBuffer;
        GLuint mVertexColorBuffer;
        GLuint mIndexBuffer;

        int mDrawCount;
    };
}