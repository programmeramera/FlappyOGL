#include "pch.h"

void cglActiveTexture(GLenum texture)
{
#if _DEBUG
	OutputDebugString(L"glActiveTexture\n");
#endif
	glActiveTexture(texture);
}