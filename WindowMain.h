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
	void onPaint(SkCanvas* canvas) override;
	void onShown() override;
	void onSize() override;
	void onKeyDown(const uint32_t& key) override;
	void onKeyDownWithCtrl(const uint32_t& key) override;
	void onMousePress(const int& x, const int& y) override;
	void onMouseDBClick(const int& x, const int& y) override;
	void onMouseRelease(const int& x, const int& y) override;
	void onMouseDrag(const int& x, const int& y) override;
	void onMouseMove(const int& x, const int& y) override;
	void onMousePressRight(const int& x, const int& y) override;
	void onTimer(const uint32_t& key) override;
	void onIme() override;
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