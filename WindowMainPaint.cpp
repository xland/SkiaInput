#include "WindowMain.h"


void WindowMain::flashCaret()
{
    //auto color = caretVisible ? colorFore : colorBg; 
    //auto ctx = getCtx();
    //ctx->setStrokeStyle(BLRgba32(color));
    //ctx->setStrokeWidth(1);
    //ctx->strokeLine(caretX, caretY, caretX, caretY + getLineHeight());
    //caretVisible = !caretVisible;
}
void WindowMain::setCaretPos()
{
    /*int charLineIndex = 0;
    caretY = padding;
    for (int i = 0; i < caretIndex; i++) {
        if (text[i] == L'\n') {
            caretY += getLineHeight();
            charLineIndex = 0;
        }
        else {
            charLineIndex += 1;
        }
    }
    auto subStr = text.substr(caretIndex - charLineIndex, charLineIndex);
    BLGlyphBuffer gb;
    BLTextMetrics tm;
    gb.setWCharText(subStr.data());
    font->shape(gb);
    font->getTextMetrics(gb, tm);
    caretX = padding +  (tm.boundingBox.x1 - tm.boundingBox.x0);*/
}
//void WindowMain::paintSelectedBg(SkCanvas* canvas) {
//    if (selectStartLine == -1 || selectStartWord == -1 || selectEndLine == -1 || selectEndWord == -1) {
//        return;
//    }
//	if (selectStartLine == selectEndLine && selectStartWord == selectEndWord) {
//		return;
//	}
//    auto [startLine, startWord, endLine, endWord ] = getSelectionArea();
//    auto height{ fontBottom - fontTop };
//    SkPaint paint;
//    paint.setColor(0X66000000);
//    for (size_t i = startLine; i <= endLine; i++)
//    {
//        float l, t, r, b;
//        if (i == startLine) {
//            auto p = wordPos[i][startWord];
//            l = p.fX; 
//            t = p.fY + fontTop + i * height;
//            b = p.fY + fontBottom + i * height;
//        }
//        else {
//            auto p = wordPos[i][0];
//            l = p.fX;
//            t = p.fY + fontTop + i * height;
//            b = p.fY + fontBottom + i * height;
//        }
//        if (i == endLine) {
//            auto p = wordPos[endLine][endWord];
//            r = p.fX;
//        }
//        else {
//            auto p = wordPos[i][wordPos[i].size()-1];
//            r = p.fX;
//        }
//        canvas->drawRect(SkRect::MakeLTRB(l, t, r, b),paint);
//    }
//}

void WindowMain::paintText()
{
    /*auto ctx = getCtx();
    ctx->fillAll(BLRgba32(colorBg));
    BLGlyphBuffer gb;
    BLTextMetrics tm;
    BLFontMetrics fm = font->metrics();
    float x = padding, y = padding + fm.ascent;
    std::wstring line;
    for (wchar_t ch : text) {
        if (ch == L'\n') {
            gb.setWCharText(line.data());
            font->shape(gb);
            font->getTextMetrics(gb, tm);
            ctx->fillGlyphRun(BLPoint(x, y), *font.get(), gb.glyphRun(), BLRgba32(colorFore));
            line.clear();
            y += getLineHeight();
        }
        else {
            line += ch;
        }
    }
    if (!line.empty()) {
        gb.setWCharText(line.data());
        font->shape(gb);
        font->getTextMetrics(gb, tm);
        ctx->fillGlyphRun(BLPoint(x, y), *font.get(), gb.glyphRun(), BLRgba32(colorFore));
    }

    setCaretPos();
    flashCaret();*/
}
