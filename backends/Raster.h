#pragma once

#include "Context.h"

class WindowBase;
class Raster:public Context
{
public:
	Raster(WindowBase* win);
	~Raster();
	void resize() override;
	sk_sp<SkSurface> getSurface() override;
	void paint(HDC dc) override;
private:

};

