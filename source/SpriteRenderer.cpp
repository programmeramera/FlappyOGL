//
// This file is used by the template to render a basic scene using GL.
//

#include "pch.h"
#include "SpriteRenderer.h"
#include "OpenGLHelper.h"
#include "MathHelper.h"

using namespace Angle;
using namespace std;


#define STRING(s) #s

SpriteRenderer::SpriteRenderer(std::shared_ptr<TextureManager> textureManager) 
	: mWindowWidth(0)
	, mWindowHeight(0)
	, mDrawCount(0)
	, mInitialized(false)
	, mTextureManager(textureManager)
{
	Initialize();
}

SpriteRenderer::~SpriteRenderer()
{
    if (mProgram != 0)
    {
        glDeleteProgram(mProgram);
        mProgram = 0;
    }

    if (mVertexPositionBuffer != 0)
    {
        glDeleteBuffers(1, &mVertexPositionBuffer);
        mVertexPositionBuffer = 0;
    }

    if (mVertexUVBuffer != 0)
    {
        glDeleteBuffers(1, &mVertexUVBuffer);
		mVertexUVBuffer = 0;
    }
}

void SpriteRenderer::InitializeShaders() {
	// Vertex Shader source
	const std::string vs = STRING
	(
		//the incoming vertex' position
		attribute vec4 a_position;
		//and its texture coordinate
		attribute vec2 a_uv;
		uniform vec2 screenSize;
		// array that contains information on
		// sprite
		// [0] -> spriteSourceX
		// [1] -> spriteSourceY
		// [2] -> spriteWidth
		// [3] -> spriteHeight
		uniform vec4 spriteRect;

		// a vec2 that represents sprite position in the world
		// [0] -> spriteX
		// [1] -> spriteY
		uniform vec2 spriteWorld;

		// texture width and height
		uniform vec2 textureSize;

		//the varying statement tells the shader pipeline that this variable
		//has to be passed on to the next stage (so the fragment shader)
		varying vec2 v_uv;

		//the shader entry point is the main method
		void main()
		{
			gl_Position = a_position; //copy the position

			// adjust position according to
			// sprite width and height
			gl_Position.x = ((gl_Position.x * spriteRect[2]) + spriteWorld[0]) / (screenSize[0] / 2.0);
			gl_Position.y = ((gl_Position.y * spriteRect[3]) - spriteWorld[1]) / (screenSize[1] / 2.0);

			// coordinates are being written
			// in homogeneous space, we have
			// to translate the space origin
			// to upper-left corner
			gl_Position.x -= 1.0;
			gl_Position.y += 1.0;

			// (texCoordX  * spriteWidth / textureWidth) + texSourceX
			v_uv.x = (a_uv.x * (spriteRect[2] / textureSize[0])) + spriteRect[0] / textureSize[0];
			// inverting v component
			v_uv.y = ((1.0 - a_uv.y) * (spriteRect[3] / textureSize[1])) + spriteRect[1] / textureSize[1];
		}
	);

	// Fragment Shader source
	const std::string fs = STRING
	(
		precision mediump float;
		//incoming values from the vertex shader stage.
		//if the vertices of a primitive have different values, they are interpolated!
		varying vec2 v_uv;
		uniform sampler2D texture;

		void main()
		{
			// read the fragment color from texture
			gl_FragColor = texture2D(texture, v_uv);
		}
	);

	// Set up the shader and its uniform/attribute locations.
	mProgram = CompileProgram(vs, fs);

	// Vertex shader parameters
	mVertexAttribLocation = glGetAttribLocation(mProgram, "a_position");
	mUVAttribLocation = glGetAttribLocation(mProgram, "a_uv");
	mScreenSizeUniformLocation = glGetUniformLocation(mProgram, "screenSize");
	mSpriteRectUniformLocation = glGetUniformLocation(mProgram, "spriteRect");
	mSpriteWorldUniformLocation = glGetUniformLocation(mProgram, "spriteWorld");
	mTextureSizeUniformLocation = glGetUniformLocation(mProgram, "textureSize");

	// Fragment shader parameters
	mTextureUniformLocation = glGetUniformLocation(mProgram, "texture");
}

void SpriteRenderer::InitializeBuffers() {
	GLfloat vertexPositions[] =
	{
		 0.0f, 1.0f, 0.0f,
		 0.0f, 0.0f, 0.0f,
		 1.0f, 0.0f, 0.0f,
		 1.0f, 1.0f, 0.0f,
	};

	glGenBuffers(1, &mVertexPositionBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, mVertexPositionBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositions), vertexPositions, GL_STATIC_DRAW);

	GLfloat vertexUVs[] =
	{
		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
	};

	glGenBuffers(1, &mVertexUVBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, mVertexUVBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexUVs), vertexUVs, GL_STATIC_DRAW);
}

void SpriteRenderer::Initialize()
{
	InitializeShaders();
	InitializeBuffers();
	mInitialized = true;
}

void SpriteRenderer::Draw()
{
	if (!mInitialized || !mTextureManager->IsLoaded()) {
		return;
	}
	// Clear the color buffer   
	cglClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	// Use the program object
	glUseProgram(mProgram);

	glBindBuffer(GL_ARRAY_BUFFER, mVertexPositionBuffer);
	glEnableVertexAttribArray(mVertexAttribLocation);
	glVertexAttribPointer(mVertexAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	auto texture = mTextureManager->GetTexture(L"checkersmall.bmp");

	MathHelper::Vector4 spriteRect(0.0f, 0.0f, 100.0f, 100.0f);
	glUniform4fv(mSpriteRectUniformLocation, 1, &(spriteRect.m[0]));

	MathHelper::Vector2 spriteWorld(200.0f, 200.0f);
	glUniform2fv(mSpriteWorldUniformLocation, 1, &(spriteWorld.m[0]));

	MathHelper::Vector2 screenSize(static_cast<float>(mWindowWidth), static_cast<float>(mWindowHeight));
	glUniform2fv(mScreenSizeUniformLocation, 1, &(screenSize.m[0]));

	glBindBuffer(GL_ARRAY_BUFFER, mVertexUVBuffer);
	glEnableVertexAttribArray(mUVAttribLocation);
	glVertexAttribPointer(mUVAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, 0);

	MathHelper::Vector2 textureSize(texture.Width, texture.Height);
	glUniform2fv(mTextureSizeUniformLocation, 1, &(textureSize.m[0]));

	cglActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture.TextureIndex);

	// Set the sampler texture unit to 0
	glUniform1i(mTextureUniformLocation, 0);

	GLushort indices[] = { 0, 1, 3, 1, 2, 3 };
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices);
}

void SpriteRenderer::UpdateWindowSize(GLsizei width, GLsizei height)
{
	if (!mInitialized || !mTextureManager->IsLoaded()) {
		return;
	}

    glViewport(0, 0, width, height);
    mWindowWidth = width;
    mWindowHeight = height;
}

void SpriteRenderer::UpdateRotation(int delta) {
	mDrawCount += delta;
}