#pragma once
#include <include/gpu/ganesh/gl/GrGLInterface.h>
#include <include/gpu/ganesh/GrDirectContext.h>
#include "Context.h"

class WindowBase;
class OpenGL:public Context
{
public:
	OpenGL(WindowBase* win);
	~OpenGL();
	void resize() override;
	sk_sp<SkSurface> getSurface() override;
	void paint(HDC dc) override;
	void textureFromImage(sk_sp<SkImage>& image) override;
private:
	void init();
private:
	HGLRC hglrc;
	sk_sp<GrDirectContext> m_grContext;
	sk_sp<const GrGLInterface> fBackendContext;
	int fStencilBits = 0;
	int fSampleCount = 3;
	SkSurfaceProps fSurfaceProps;
	GrGLFramebufferInfo fbInfo;
};

