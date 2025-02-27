#pragma once
#include <vector>

struct GlyphInfo
{
	std::vector<SkGlyphID> glyphs;
	std::vector<SkPoint> wordPos;
	float x, y;
};

