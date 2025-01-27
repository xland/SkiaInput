#include <d3d11.h>
#include <dxgi.h>
#include <iostream>
#include <wrl/client.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include "SkWGL.h"

#include "Context.h"
#include "OpenGL.h"
#include "Raster.h"

using Microsoft::WRL::ComPtr;

Context::Context(WinBase* win) :win{win}
{
}

Context::~Context()
{
}

std::unique_ptr<Context> Context::create(WinBase* win)
{
    if (isGPUAvailable()) {
        return std::make_unique<OpenGL>(win);
    }
    else {
        return std::make_unique<Raster>(win);
    }	
}

SkCanvas* Context::getCanvas()
{
	auto surface = getSurface();
	return surface->getCanvas();
}

bool Context::isGPUAvailable() {


    ComPtr<IDXGIFactory> dxgiFactory;
    HRESULT hr = CreateDXGIFactory(__uuidof(IDXGIFactory), reinterpret_cast<void**>(dxgiFactory.GetAddressOf()));
    return SUCCEEDED(hr);
}
