#pragma once
#include <memory>
#include <windows.h>
#include <string>
#include <include/core/SkSurface.h>
#include <include/core/SkCanvas.h>

class WindowInput
{
public:
	WindowInput();
	~WindowInput();
	void show();
private:
	void initFont();
	void paint();
	void initWindow();
	void initImg();
	void flashCaret();
	bool enableAlpha();
	static LRESULT CALLBACK routeWinMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK processWinMsg(UINT msg, WPARAM wParam, LPARAM lParam);
private:
	int x, y, w, h;
	HWND hwnd;
	//std::unique_ptr<BLImage> img;
	//float fontSize{ 26 };
	//std::unique_ptr<BLFont> font;
	//bool caretVisible{ true };
	//BLPoint caretPos{ 10,10 };
};