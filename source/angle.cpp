#include "pch.h"

void cglActiveTexture(GLenum texture)
{
//#if _DEBUG
//	OutputDebugString(L"glActiveTexture\n");
//#endif
	glActiveTexture(texture);
}

void cglClearColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha) {
	glClearColor(red, green, blue, alpha);
}