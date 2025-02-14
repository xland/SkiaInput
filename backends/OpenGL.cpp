#include <include/gpu/ganesh/gl/GrGLInterface.h>
#include <include/gpu/ganesh/gl/GrGLDirectContext.h>
#include <include/gpu/ganesh/gl/GrGLAssembleHelpers.h>
#include <include/gpu/ganesh/gl/GrGLAssembleInterface.h>

#include <include/gpu/ganesh/SkImageGanesh.h>
#include <include/gpu/ganesh/GrBackendSurface.h>
#include <include/gpu/ganesh/GrDirectContext.h>
#include <include/gpu/ganesh/SkSurfaceGanesh.h>
#include <include/gpu/ganesh/gl/GrGLBackendSurface.h>

#include <src/base/SkMathPriv.h>
#include <src/gpu/ganesh/GrCaps.h>
#include <src/gpu/ganesh/GrDirectContextPriv.h>
#include <src/gpu/ganesh/gl/GrGLDefines.h>
#include <src/gpu/ganesh/gl/GrGLUtil.h>

#include <Windows.h>
#include <GL/gl.h>
#include "SkWGL.h"
#include "../WinBase.h"
#include "OpenGL.h"

OpenGL::OpenGL(WinBase* win) : Context(win), fBackendContext{nullptr}, fSurfaceProps(0, kRGB_H_SkPixelGeometry)
{
    backendType = "OpenGL";
}

OpenGL::~OpenGL()
{
    destroyContext();
}

void OpenGL::resize()
{
    destroyContext();
    init();
}

sk_sp<SkSurface> OpenGL::getSurface()
{
    if (nullptr == surface) {
        GrGLint buffer;
        // 这一句非常重要
        // 原代码为：GR_GL_CALL(fBackendContext.get(), GetIntegerv(GR_GL_FRAMEBUFFER_BINDING, &buffer));
        fBackendContext.get()->fFunctions.fGetIntegerv(GR_GL_FRAMEBUFFER_BINDING, &buffer);
        GrGLFramebufferInfo fbInfo;
        fbInfo.fFBOID = buffer;
        fbInfo.fFormat = GR_GL_RGBA8;
        fbInfo.fProtected = skgpu::Protected(false);
        auto backendRT = GrBackendRenderTargets::MakeGL(win->w, win->h,
            fSampleCount,
            fStencilBits,
            fbInfo);        
        surface = SkSurfaces::WrapBackendRenderTarget(fContext.get(),
            backendRT,
            kBottomLeft_GrSurfaceOrigin,
            kRGBA_8888_SkColorType,
            nullptr,
            &fSurfaceProps);
    }
    return surface;
}

void OpenGL::destroyContext() {
    surface.reset(nullptr);
    if (fContext) {
        // in case we have outstanding refs to this (lua?)
        fContext->abandonContext();
        fContext.reset();
    }
    fBackendContext.reset(nullptr);
    wglDeleteContext(fHGLRC);
    fHGLRC = NULL;
}

void OpenGL::init()
{
    HDC dc = GetDC(win->hwnd);
    fHGLRC = SkCreateWGLContext(dc, fSampleCount, false, kGLPreferCompatibilityProfile_SkWGLContextRequest);
    SkWGLExtensions extensions;
    if (extensions.hasExtension(dc, "WGL_EXT_swap_control")) {
        extensions.swapInterval(1);
    }
    //if (wglMakeCurrent(dc, fHGLRC)) {
    //    auto glInterface = GrGLMakeNativeInterface();
    //    bool renderDocAttached = glInterface->hasExtension("GL_EXT_debug_tool");
    //    glInterface.reset(nullptr);
    //    if (renderDocAttached) {
    //        wglDeleteContext(fHGLRC);
    //        fHGLRC = SkCreateWGLContext(dc, fSampleCount, false, kGLPreferCoreProfile_SkWGLContextRequest);
    //    }
    //}
    if (wglMakeCurrent(dc, fHGLRC)) {
        glClearStencil(0);
        glClearColor(0, 0, 0, 0);
        glStencilMask(0xffffffff);

        glClear(GL_STENCIL_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        // use DescribePixelFormat to get the stencil and color bit depth.
        int pixelFormat = GetPixelFormat(dc);
        PIXELFORMATDESCRIPTOR pfd;
        DescribePixelFormat(dc, pixelFormat, sizeof(pfd), &pfd);
        fStencilBits = pfd.cStencilBits;

        // Get sample count if the MSAA WGL extension is present
        if (extensions.hasExtension(dc, "WGL_ARB_multisample")) {
            static const int kSampleCountAttr = SK_WGL_SAMPLES;
            extensions.getPixelFormatAttribiv(dc, pixelFormat, 0, 1, &kSampleCountAttr, &fSampleCount);
            fSampleCount = std::max(fSampleCount, 1);
        }
        else {
            fSampleCount = 1;
        }
        glViewport(0, 0, win->w, win->h);
    }
    fBackendContext = GrGLMakeNativeInterface();
    fContext = GrDirectContexts::MakeGL(fBackendContext, fGrContextOptions);
    //std::unique_ptr<Recorder> recorder = fContext->makeRecorder();
    ReleaseDC(win->hwnd, dc);
}

void OpenGL::paint(HDC dc) {
    fContext->flushAndSubmit(surface.get());
    SwapBuffers(dc);
}

void OpenGL::textureFromImage(sk_sp<SkImage>& image)
{
    image = SkImages::TextureFromImage(fContext.get(), image.get());
}
