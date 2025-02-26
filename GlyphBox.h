#pragma once
#include <include/core/SkFontMgr.h>
#include <include/core/SkFontStyle.h>
#include <include/ports/SkTypeface_win.h>
#include <include/core/SkFontMetrics.h>
#include <include/core/SkTypes.h>
#include <include/core/SkCanvas.h>
#include <include/core/SkFont.h>
#include "GlyphInfo.h"

class WindowMain;
class WindowCaret;
class GlyphBox
{
public:
	GlyphBox();
	~GlyphBox();
	void init(WindowMain* win);
	void paintText(SkCanvas* canvas);
	void paintSelectBg(SkCanvas* canvas);
	void moveCaret(const int& x, const int& y);
	void checkCancelSelection();
	void refreshCaret();
	void onChar(const std::wstring& str);
	void onKey(const uint32_t& key);
	void onKeyWithCtrl(const uint32_t& key);
	void onIme();
public:
	uint32_t colorBg{ 0X2222FF88 }, colorFore{ 0XFF000000 }, colorSelected{ 0X8822FF88 };
	int caretX{ 3 }, caretY{ 1 };
	int caretXStart{ -1 }, caretYStart{ -1 };
	int caretXEnd{ -1 }, caretYEnd{ -1 };
private:
	void moveCaretLeft();
	void moveCaretRight();
	void moveCaretUp();
	void moveCaretDown();
	void keyBack();
	void keyEnter();
	void keyDel();
	void initFont();
	void initInfo();
	float getLineHeight();
	void copy();
	void cut();
	void paste();
	SkPoint getCaretPos(const int& caretX, const int& caretY);
	int getCharIndex(const int& caretX, const int& caretY);
private:
	SkFont font;
	std::wstring text{ LR"(破阵子 · 为陈同甫赋壮词以寄之
辛弃疾 · 宋 · XinQiJi (1140年－1207年) 

醉里挑灯看剑，梦回吹角连营。
八百里分麾下炙，五十弦翻塞外声，沙场秋点兵。
马作的卢飞快，弓如霹雳弦惊。
了却君王天下事，赢得生前身后名。可怜白发生！
)" };
	bool caretVisible{ true };
	float padding{ 18 };
	float fontSize{ 26 };
	float fontTop, fontBottom;
	float lineSpace{ 1.2 };
	std::vector<GlyphInfo> infos;
	std::shared_ptr<WindowCaret> caretWin;
	WindowMain* win;
};

