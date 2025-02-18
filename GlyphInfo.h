#pragma once
#include <vector>
#include <sstream>
#include <include/core/SkFontMgr.h>
#include <include/core/SkFontStyle.h>
#include <include/ports/SkTypeface_win.h>
#include <include/core/SkFontMetrics.h>
#include <include/core/SkTypes.h>
#include <include/core/SkCanvas.h>
#include <include/core/SkFont.h>

class GlyphInfo
{
public:
	GlyphInfo();
	~GlyphInfo();
public:
	std::vector<SkGlyphID> glyphs;
	std::vector<SkPoint> wordPos;
	float x, y;
private:
};

