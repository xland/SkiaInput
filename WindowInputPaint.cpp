#include "WindowInput.h"


void WindowInput::flashCaret()
{
    auto color = caretVisible ? 0xFF00FFFF : 0xFF345678;
    SkPoint p;
    if (caretLineIndex == 0 && caretWordIndex == 0) {
        p = SkPoint(12, 0 - fontTop + fontDesent);
    }
    else {
        p = wordPos[caretLineIndex][caretWordIndex];
    }
    auto height{ fontBottom - fontTop };
    SkPoint start = SkPoint(p.fX, p.fY + fontTop + caretLineIndex * height);// 字符顶部相对于基线的偏移  neagtive
    SkPoint end = SkPoint(p.fX, p.fY + fontBottom + caretLineIndex * height); // 字符底部相对于基线的偏移
    auto canvas = surface->getCanvas();
    SkPaint paint;
    paint.setColor(color);
    paint.setStroke(true);
    paint.setStrokeWidth(1);
    canvas->drawLine(start, end, paint);
    caretVisible = !caretVisible;
}
void WindowInput::paintSelectedBg(SkCanvas* canvas) {
    if (selectStartLine == -1 || selectStartWord == -1 || selectEndLine == -1 || selectEndWord == -1) {
        return;
    }

    int startLine{ selectStartLine }, startWord{ selectStartWord }, 
        endLine{ selectEndLine }, endWord{ selectEndWord };
    if (selectEndLine < selectStartLine) {
        endLine = selectStartLine;
        endWord = selectStartWord;
        startLine = selectEndLine;
        startWord = selectEndWord;
    }
    else if (startLine == selectStartLine && startWord < selectStartWord) {
        endLine = selectStartLine;
        endWord = selectStartWord;
        startLine = selectEndLine;
        startWord = selectEndWord;
    }

    auto height{ fontBottom - fontTop };
    SkPaint paint;
    paint.setColor(0X66FFFFFF);
    for (size_t i = startLine; i <= endLine; i++)
    {
        float l, t, r, b;
        if (i == startLine) {
            auto p = wordPos[i][startWord];
            l = p.fX; 
            t = p.fY + fontTop + i * height;
            b = p.fY + fontBottom + i * height;
        }
        else {
            auto p = wordPos[i][0];
            l = p.fX;
            t = p.fY + fontTop + i * height;
            b = p.fY + fontBottom + i * height;
        }
        if (i == endLine) {
            auto p = wordPos[endLine][endWord];
            r = p.fX;
        }
        else {
            auto p = wordPos[i][wordPos[i].size()-1];
            r = p.fX;
        }
        canvas->drawRect(SkRect::MakeLTRB(l, t, r, b),paint);
    }
}

void WindowInput::paint()
{
    if (w <= 0 || h <= 0) return;
    SkPixmap pix;
    surface->peekPixels(&pix);
    PAINTSTRUCT ps;
    auto dc = BeginPaint(hwnd, &ps);
    BITMAPINFO bmi = { sizeof(BITMAPINFOHEADER), w, 0 - h, 1, 32, BI_RGB, h * 4 * w, 0, 0, 0, 0 };
    SetDIBitsToDevice(dc, 0, 0, w, h, 0, 0, 0, h, pix.addr(), &bmi, DIB_RGB_COLORS);
    ReleaseDC(hwnd, dc);
    EndPaint(hwnd, &ps);
}

void WindowInput::paintLine(const std::wstring& text, const int& lineIndex, SkCanvas* canvas)
{
    wordPos.insert({ lineIndex,std::vector<SkPoint>() });
    SkPaint paint;
    paint.setColor(0xFF00FFFF);
    paint.setStroke(false);

    auto length = text.size() * sizeof(wchar_t);
    std::vector<SkGlyphID> glyphs(text.size());
    int glyphCount = font.textToGlyphs(text.data(), length, SkTextEncoding::kUTF16, glyphs.data(), text.size());
    std::vector<SkScalar> widths(glyphCount);
    font.getWidthsBounds(glyphs.data(), glyphCount, widths.data(), nullptr, nullptr);

    SkScalar x = 12;
    float height{ fontBottom - fontTop };
    for (int i = 0; i < glyphCount; ++i) {
        wordPos[lineIndex].push_back(SkPoint::Make(x, height));
        x += widths[i]; // 累计宽度
    }
    wordPos[lineIndex].push_back(SkPoint::Make(x, height));
    canvas->drawGlyphs(glyphCount, glyphs.data(), wordPos[lineIndex].data(), SkPoint(0, lineIndex * height), font, paint);
}
void WindowInput::paintText()
{
    SkCanvas* canvas = surface->getCanvas();
    canvas->clear(0xFF345678);
    paintSelectedBg(canvas);
    wordPos.clear();
    int lineIndex{ 0 };
    for (size_t i = 0; i < lines.size(); i++)
    {
        paintLine(lines[i], i, canvas);
    }
    if (caretLineIndex < 0) {
        caretLineIndex = wordPos.size() - 1;
    }
    if (caretWordIndex < 0) {
        caretWordIndex = wordPos[wordPos.size() - 1].size() - 1;
    }
    caretVisible = true;
    flashCaret();
}
