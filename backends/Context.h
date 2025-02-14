#pragma once
#include <string>
#include <memory>
#include <Windows.h>
#include <include/core/SkSurface.h>

class WinBase;
class Context
{
public:
	~Context();
	static std::unique_ptr<Context> create(WinBase* win);
	virtual void resize() = 0;
	virtual sk_sp<SkSurface> getSurface() = 0;
	SkCanvas* getCanvas();
	virtual void textureFromImage(sk_sp<SkImage>& image) {};
	virtual void paint(HDC dc) = 0;
public:
	std::string backendType;
protected:
	Context(WinBase* win);
protected:
	WinBase* win;
	sk_sp<SkSurface> surface;
private:
	static bool isGPUAvailable();
};

