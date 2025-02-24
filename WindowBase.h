#pragma once
#include <vector>
#include <string>
#include <memory>
#include <map>
#include <array>
#include <thread>
#include <functional>
#include <include/core/SkData.h>
#include <include/core/SkPaint.h>
#include <include/core/SkCanvas.h>
#include <include/core/SkFont.h>
#include <include/core/SkSurface.h>
#include <include/core/SkFontMgr.h>
#include <include/core/SkFontStyle.h>
#include <include/ports/SkTypeface_win.h>
#include <include/core/SkFontMetrics.h>

#include "Util.h"
#include "backends/Backend.h"


class WindowBase
{
public:
	WindowBase();
	~WindowBase();
	void show();
public:
	std::vector<std::function<void()>> funcShown;
	std::vector<std::function<void(const int& w, const int& h)>> funcSize;
	std::vector<std::function<void(const int& x, const int& y)>> funcMove;
	std::vector<std::function<void(SkCanvas*)>> funcPaint;
	std::vector<std::function<void(const int&, const int&)>> funcMousePress;
	std::vector<std::function<void(const int&, const int&)>> funcMousePressRight;
	std::vector<std::function<void(const int&, const int&)>> funcMouseDBClick;
	std::vector<std::function<void(const int&, const int&)>> funcMouseMove;
	std::vector<std::function<void(const int&, const int&)>> funcMouseDrag;
	std::vector<std::function<void(const int&, const int&)>> funcMouseRelease;
	std::vector<std::function<void(const uint32_t&)>> funcKeyDown;
	std::vector<std::function<void(const uint32_t&)>> funcKeyDownWithCtrl;
	std::vector<std::function<void(const uint32_t&)>> funcTimer;
	std::vector<std::function<void()>> funcIme;
	int x, y, w, h;
	HWND hwnd;
protected:
	void initWindow();

private:
	static LRESULT CALLBACK routeWinMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK processWinMsg(UINT msg, WPARAM wParam, LPARAM lParam);
	void onShown();
	void onSize(const int& w, const int& h);
	void onMove(const int& x, const int& y);
	void onPaint(SkCanvas* canvas);
	void onMousePress(const int& x, const int& y);
	void onMousePressRight(const int& x, const int& y);
	void onMouseDBClick(const int& x, const int& y);
	void onMouseMove(const int& x, const int& y);
	void onMouseDrag(const int& x, const int& y);
	void onMouseRelease(const int& x, const int& y);
	void onKeyDown(const uint32_t& key);
	void onKeyDownWithCtrl(const uint32_t& key);
	void onTimer(const uint32_t& key);
	void onIme();
private:
	std::unique_ptr<Backend> backend;
};

