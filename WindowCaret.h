#pragma once
#include <vector>
#include <Windows.h>
#include <windowsx.h>
#include <dwmapi.h>

class WindowMain;
class WindowCaret
{
public:
	WindowCaret(const int& height, WindowMain* win);
	~WindowCaret();
	void move(const int&x,const int& y);
public:
	int  w{ 1 }, h{0};
	HWND hwnd;
private:
	void initWindow();
	void onPaint();
private:
	static LRESULT CALLBACK routeWinMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
private:
	std::vector<DWORD> pixels;
	DWORD color{ 0xFF000000 };
	WindowMain* win;

};

