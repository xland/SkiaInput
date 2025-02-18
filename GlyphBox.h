#pragma once
#include <include/core/SkFontMgr.h>
#include <include/core/SkFontStyle.h>
#include <include/ports/SkTypeface_win.h>
#include <include/core/SkFontMetrics.h>
#include <include/core/SkTypes.h>
#include <include/core/SkCanvas.h>
#include <include/core/SkFont.h>
#include "GlyphInfo.h"

class GlyphBox
{
public:
	GlyphBox();
	~GlyphBox();
	void init();
	void paintText(SkCanvas* canvas);
	void paintCaret(SkCanvas* canvas);
private:
	void initFont();
	void initInfo();
	float getLineHeight();
private:
	SkFont font;
	std::wstring text{ LR"(破阵子 · 为陈同甫赋壮词以寄之
辛弃疾 · 宋 · XinQiJi (1140年－1207年) 

醉里挑灯看剑，梦回吹角连营。
八百里分麾下炙，五十弦翻塞外声，沙场秋点兵。
马作的卢飞快，弓如霹雳弦惊。
了却君王天下事，赢得生前身后名。可怜白发生！
)" };
	size_t caretX{ 6 };
	size_t caretY{ 1 };
	bool caretVisible{ true };
	float padding{ 18 };
	float fontSize{ 26 };
	float fontTop, fontBottom;
	float lineSpace{ 1.2 };
	std::vector<GlyphInfo> infos;
};

