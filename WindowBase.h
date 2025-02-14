#pragma once
#include <vector>
#include <string>
#include <memory>
#include <map>
#include <array>
#include <thread>
#include <Windows.h>
#include <windowsx.h>
#include <dwmapi.h>
#include <versionhelpers.h>
#include "backends/Backend.h"

class WindowBase
{
public:
	WindowBase();
	~WindowBase();
	void show();
public:
	int x, y, w, h;
	HWND hwnd;
protected:
	void initWindow();
	void initContext();
	bool initAlpha();
	void activeKeyboard(const int& x, const int& y);
	virtual void onShown() {};
	virtual void onSize() {};
	virtual void onMove() {};
	virtual void onPaint() {};
	virtual void onMousePress(const int& x, const int& y) {};
	virtual void onMousePressRight(const int& x, const int& y) {};
	virtual void onMouseDBClick(const int& x, const int& y) {};
	virtual void onMouseMove(const int& x, const int& y) {};
	virtual void onMouseDrag(const int& x, const int& y) {};
	virtual void onMouseRelease(const int& x, const int& y) {};
	virtual void onKeyDown(const uint32_t& key) {};
	virtual void onKeyDownWithCtrl(const uint32_t& key) {};
private:
	static LRESULT CALLBACK routeWinMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK processWinMsg(UINT msg, WPARAM wParam, LPARAM lParam);
private:
	std::unique_ptr<Backend> backend;
};

