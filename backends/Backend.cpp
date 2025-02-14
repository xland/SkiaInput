#include <iostream>

#include "Backend.h"
#include "OpenGL.h"
#include "Raster.h"

Backend::Backend(WindowBase* win) :win{win}
{
}

Backend::~Backend()
{
}

std::unique_ptr<Backend> Backend::create(WindowBase* win)
{
    if (isGPUAvailable()) {
        return std::make_unique<OpenGL>(win);
    }
    else {
        return std::make_unique<Raster>(win);
    }	
}

SkCanvas* Backend::getCanvas()
{
	auto surface = getSurface();
	return surface->getCanvas();
}

bool Backend::isGPUAvailable() {
    return true;
}
