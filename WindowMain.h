#pragma once

#include "WindowBase.h"
#include "GlyphBox.h"

class WindowMain:public WindowBase
{
public:
	WindowMain();
	~WindowMain();
public:
protected:
	void onPaint(SkCanvas* canvas);
	void onShown();
	void onSize();
	void onKeyDown(const uint32_t& key);
	void onKeyDownWithCtrl(const uint32_t& key);
	void onMousePress(const int& x, const int& y);
	void onMouseDBClick(const int& x, const int& y);
	void onMouseRelease(const int& x, const int& y);
	void onMouseDrag(const int& x, const int& y);
	void onMouseMove(const int& x, const int& y);
	void onMousePressRight(const int& x, const int& y);
	void onIme();
private:
	void initPosSize();
	void paintText();
	void flashCaret();
	void setCaretPos();
	std::wstring getClipboardText();
	void saveToClipboard(const std::wstring& str);
	std::vector<std::wstring> textToLines(const std::wstring& text);
	std::string convertToUTF8(const std::wstring& wstr);
	std::string convertToUTF8(const LPWSTR& wstr);
private:	
	GlyphBox glyphBox;
};