#pragma once

#include "Context.h"

class WinBase;
class Raster:public Context
{
public:
	Raster(WinBase* win);
	~Raster();
	void resize() override;
	sk_sp<SkSurface> getSurface() override;
	void paint(HDC dc) override;
private:

};

