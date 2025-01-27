#pragma once
#include <include/gpu/ganesh/gl/GrGLInterface.h>
#include <include/gpu/ganesh/GrDirectContext.h>
#include "Context.h"

class WinBase;
class OpenGL:public Context
{
public:
	OpenGL(WinBase* win);
	~OpenGL();
	void resize() override;
	sk_sp<SkSurface> getSurface() override;
	void paint(HDC dc) override;
	void textureFromImage(sk_sp<SkImage>& image) override;
private:
	void init();
	void destroyContext();
private:
	HGLRC fHGLRC{nullptr};
	int fStencilBits = 0;
	int fSampleCount = 3;
	sk_sp<const GrGLInterface> fBackendContext;
	sk_sp<GrDirectContext> fContext;
	GrContextOptions fGrContextOptions;
	SkSurfaceProps fSurfaceProps;
};

