#include <iostream>

#include "Context.h"
#include "OpenGL.h"
#include "Raster.h"

Context::Context(WindowBase* win) :win{win}
{
}

Context::~Context()
{
}

std::unique_ptr<Context> Context::create(WindowBase* win)
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
    return true;
}
