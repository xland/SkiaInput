#pragma once
#include <vector>
#include <Windows.h>
#include <windowsx.h>
#include <dwmapi.h>

class WindowMain;
class WindowCaret
{
public:
	~WindowCaret();
	static void init(const int& height);
	static void move(const int&x,const int& y,WindowMain* win);
public:
	int x{ 0 }, y{ 0 }, w{ 2 }, h{0};
	HWND hwnd;
private:
	WindowCaret(const int& height);
	void initWindow();
	void onPaint();
private:
	static LRESULT CALLBACK routeWinMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
private:
	std::vector<DWORD> pixels;
	DWORD color{ 0xFF000000 };
};

