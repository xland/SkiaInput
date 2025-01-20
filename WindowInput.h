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
	void initWindow();
	void initSurface();
	void initFont();
	void paint();
	void paintText();
	void flashCaret();
	bool enableAlpha();
	void activeKeyboard();
	void paintOneLine(const std::wstring& text,const int& lineIndex,SkCanvas* canvas);
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
	std::wstring text{ LR"(破阵子·为陈同甫赋壮词以寄之
辛弃疾·宋·XinQiJi(1140年－1207年)

醉里挑灯看剑，梦回吹角连营。
八百里分麾下炙，五十弦翻塞外声，沙场秋点兵。
马作的卢飞快，弓如霹雳弦惊。
了却君王天下事，赢得生前身后名。可怜白发生！
)" };
	SkFont font;
	float fontTop, fontBottom;
	std::map<int, std::vector<SkPoint>> wordPos;
	bool caretVisible{ true };
	int caretLineIndex{ -1 };
	int caretWordIndex{ -1 };
	float fontSize{ 26 };
	bool isMouseDown{ false };
};