#pragma once

#include "WindowBase.h"

class WindowMain:public WindowBase
{
public:
	WindowMain();
	~WindowMain();
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

private:
	void initFont();
	void initPosSize();
	float getLineHeight();
	void paintText();
	void flashCaret();
	void setCaretPos();
	std::wstring getClipboardText();
	void saveToClipboard(const std::wstring& str);
	std::vector<std::wstring> textToLines(const std::wstring& text);
	std::string convertToUTF8(const std::wstring& wstr);
	std::string convertToUTF8(const LPWSTR& wstr);
private:
	SkFont font;
	float fontTop, fontBottom, fontAsent, fontDesent;//fontAsent从基线到字体中最高字符顶部的距离,fTop 是从基线到字体中理论上最高点的距离。
	std::wstring text{ LR"(破阵子 · 为陈同甫赋壮词以寄之
辛弃疾 · 宋 · XinQiJi (1140年－1207年) 

醉里挑灯看剑，梦回吹角连营。
八百里分麾下炙，五十弦翻塞外声，沙场秋点兵。
马作的卢飞快，弓如霹雳弦惊。
了却君王天下事，赢得生前身后名。可怜白发生！
)" };
	size_t caretIndex{ 7 };
	float caretX, caretY;
	bool caretVisible{ true };
	float padding{ 18 };
	float fontSize{ 16 }, lineHeight{ 1.5 };
	uint32_t colorBg{ 0X2222FF88 }, colorFore{ 0XFF000000 };
};