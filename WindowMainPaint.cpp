#include "WindowMain.h"

void WindowMain::paintText(SkCanvas* canvas)
{
    SkPaint paint;
    paint.setColor(SK_ColorBLACK);
    paint.setAntiAlias(true);
    for (auto& info : infos)
    {
        canvas->drawGlyphs(info.wordPos.size() - 1, info.glyphs.data(), info.wordPos.data(), SkPoint(info.x, info.y), font, paint);
    }
}
void WindowMain::paintSelectBg(SkCanvas* canvas)
{
    if (caretXStart == -1 || caretYStart == -1 || caretXEnd == -1 || caretYEnd == -1) {
        return;
    }
    if (caretXStart == caretXEnd && caretYStart == caretYEnd) {
        return;
    }
    int line1{ caretYStart }, line2{ caretYEnd }, word1{ caretXStart }, word2{ caretXEnd };
    if (line2 < line1) {
        std::swap(line1, line2);
        std::swap(word1, word2);
    }
    else if (line2 == line1) {
        if (word2 < word1) {
            std::swap(word1, word2);
        }
    }
    auto lineHeight = getLineHeight();
    auto startPos = getCaretPos(word1, line1);
    auto endPos = getCaretPos(word2, line2);
    SkPaint paint;
    paint.setColor(colorSelected);
    if (line1 == line2) {
        canvas->drawRect(SkRect::MakeLTRB(startPos.fX, startPos.fY - lineHeight, endPos.fX, endPos.fY), paint);
    }
    else {
        auto startLineSize = infos[line1].wordPos.size();
        auto rect = SkRect::MakeLTRB(startPos.fX, startPos.fY - lineHeight,
            infos[line1].wordPos[startLineSize - 1].fX + infos[line1].x, startPos.fY);
        canvas->drawRect(rect, paint);
        for (size_t i = line1 + 1; i <= line2 - 1; i++)
        {
            auto pos = getCaretPos(0, i);
            auto size = infos[i].wordPos.size();
            rect.setLTRB(infos[i].x, pos.fY - lineHeight, infos[i].wordPos[size - 1].fX + infos[line1].x, pos.fY);
            canvas->drawRect(rect, paint);
        }
        rect.setLTRB(infos[line1].x, endPos.fY - lineHeight, endPos.fX, endPos.fY);
        canvas->drawRect(rect, paint);
    }
}

void WindowMain::paintScroller(SkCanvas* canvas)
{
    auto width{ 16 };
    auto rect = SkRect::MakeLTRB(w- width, 0, w, h);
    SkPaint paint;
    paint.setColor(0x66888888);
    canvas->drawRect(rect, paint);

    rect = SkRect::MakeXYWH(w - width, 0, w, 200);
    paint.setColor(0x66828228);
    canvas->drawRect(rect, paint);
}