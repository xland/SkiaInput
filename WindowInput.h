#pragma once
#include <memory>
#include <windows.h>
#include <string>
#include <map>
#include <include/core/SkSurface.h>
#include <include/core/SkCanvas.h>
#include <include/core/SkFontMgr.h>
#include <include/core/SkFontStyle.h>
#include <include/ports/SkTypeface_win.h>
#include <include/core/SkFont.h>
#include <include/core/SkFontMetrics.h>

class WindowInput
{
public:
	WindowInput();
	~WindowInput();
	void show();
private:
	void initSize();
	void initText();
	void initWindow();
	void initSurface();
	void initFont();
	void paint();
	void paintText();
	void flashCaret();
	bool enableAlpha();
	void activeKeyboard();
	void paintLine(const std::wstring& text,const int& lineIndex,SkCanvas* canvas);
	void onEnter();
	void onBackspace();
	void onMouseDown(const int& x, const int& y);
	void onDoubleClick(const int& x, const int& y);
	void onMouseUp(const int& x, const int& y);
	void onMouseDrag(const int& x, const int& y);
	void onMouseMove(const int& x, const int& y);
	void onMouseDownRight(const int& x, const int& y);
	void onKeyDown(const unsigned int& val);
	void onChar(const unsigned int& val);
	static LRESULT CALLBACK routeWinMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK processWinMsg(UINT msg, WPARAM wParam, LPARAM lParam);
private:
	int x, y, w, h;
	HWND hwnd;
	sk_sp<SkSurface> surface;
	
	SkFont font;
	float fontTop, fontBottom;
	std::vector<std::wstring> lines;
	std::map<int, std::vector<SkPoint>> wordPos;
	bool caretVisible{ true };
	int caretLineIndex{ -1 };
	int caretWordIndex{ -1 };
	float fontSize{ 26 };
	bool isMouseDown{ false };
};