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
#include "../WindowBase.h"
#include "OpenGL.h"


namespace {
    #define WGL_CONTEXT_MAJOR_VERSION_ARB 0x2091
    #define WGL_CONTEXT_MINOR_VERSION_ARB 0x2092
    #define WGL_CONTEXT_PROFILE_MASK_ARB  0x9126
    #define WGL_CONTEXT_CORE_PROFILE_BIT_ARB 0x00000001 
    typedef HGLRC(WINAPI* PFNWGLCREATECONTEXTATTRIBSARBPROC)(HDC hDC, HGLRC hShareContext, const int* attribList);
    PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = nullptr;
}

OpenGL::OpenGL(WindowBase* win) : Backend(win), fSurfaceProps(0, kRGB_H_SkPixelGeometry)
{
    backendType = "OpenGL";
    init();
}

OpenGL::~OpenGL()
{
    surface.reset(nullptr);
    wglMakeCurrent(nullptr, nullptr);
    wglDeleteContext(hglrc);
    hglrc = NULL;
}

void OpenGL::resize()
{
    surface.reset(nullptr);
}

sk_sp<SkSurface> OpenGL::getSurface()
{
    if (nullptr == surface) {
        auto backendRT = GrBackendRenderTargets::MakeGL(win->w, win->h,
            fSampleCount,
            fStencilBits,
            fbInfo);
        surface = SkSurfaces::WrapBackendRenderTarget(m_grContext.get(),
            backendRT,
            kBottomLeft_GrSurfaceOrigin,
            kRGBA_8888_SkColorType,
            nullptr,
            &fSurfaceProps);
    }
    return surface;
}

void OpenGL::init()
{
    HDC hdc = GetDC(win->hwnd);
    PIXELFORMATDESCRIPTOR pfd = {};
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 24;
    pfd.iLayerType = PFD_MAIN_PLANE;
    int pixelFormat = ChoosePixelFormat(hdc, &pfd);
    SetPixelFormat(hdc, pixelFormat, &pfd);
    HGLRC oldContext = wglCreateContext(hdc);
    wglMakeCurrent(hdc, oldContext);
    wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
    int attribs[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
        WGL_CONTEXT_MINOR_VERSION_ARB, 3,
        WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        0
    };
    HGLRC newContext = wglCreateContextAttribsARB(hdc, 0, attribs);
    if (newContext) {
        wglMakeCurrent(nullptr, nullptr);
        wglDeleteContext(oldContext);
        hglrc = newContext;
    }
    else {
        hglrc = oldContext;
    }
    wglMakeCurrent(hdc, hglrc);
    ReleaseDC(win->hwnd, hdc);

    // 关闭 VSYNC ，否则会由于帧率的抖动导致平均帧率降低。
    // VSYNC 会阻塞 SkSurface::flush，从而使每帧的耗时接近16.6ms,帧率最多60fps
    //eglSwapInterval(display_, 0);
    //eglSwapBuffers(display_, surface_);

    fBackendContext = GrGLMakeNativeInterface();
    m_grContext = GrDirectContexts::MakeGL(fBackendContext);

    GrGLint buffer;
    // 这一句非常重要
    // 原代码为：GR_GL_CALL(fBackendContext.get(), GetIntegerv(GR_GL_FRAMEBUFFER_BINDING, &buffer));
    fBackendContext.get()->fFunctions.fGetIntegerv(GR_GL_FRAMEBUFFER_BINDING, &buffer);
    
    fbInfo.fFBOID = buffer;
    fbInfo.fFormat = GR_GL_RGBA8;
    fbInfo.fProtected = skgpu::Protected(false);
}

void OpenGL::paint(HDC dc) {
    m_grContext->flushAndSubmit(surface.get());
    SwapBuffers(dc);
}
void OpenGL::textureFromImage(sk_sp<SkImage>& image)
{
    image = SkImages::TextureFromImage(m_grContext.get(), image.get());
}