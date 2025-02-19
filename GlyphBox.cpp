#include "GlyphBox.h"
#include "WindowMain.h"

GlyphBox::GlyphBox()
{
}

GlyphBox::~GlyphBox()
{
}

void GlyphBox::init(WindowMain* win)
{
    this->win = win;
    initFont();
    initInfo();
}
void GlyphBox::paintText(SkCanvas* canvas)
{
    SkPaint paint;
    paint.setColor(SK_ColorBLACK);
    paint.setAntiAlias(true);
    for (auto& info: infos)
    {
        canvas->drawGlyphs(info.wordPos.size() - 1, info.glyphs.data(), info.wordPos.data(), SkPoint(info.x, info.y), font, paint);
    }
}
void GlyphBox::paintCaret(SkCanvas* canvas)
{
    SkPaint paint;
    paint.setBlendMode(SkBlendMode::kClear);
    paint.setColor(SK_ColorBLACK);
    paint.setStroke(true);
    paint.setStrokeWidth(1);
    auto height = getLineHeight();
    auto pos = getInputPos();
    SkPoint startPos(pos.fX, pos.fY - fontBottom + fontTop);
    canvas->drawLine(startPos, pos, paint);
    paint.setBlendMode(SkBlendMode::kSrcOver);
    auto color = caretVisible || win->paintState != 1 ? win->colorFore : win->colorBg;
    paint.setColor(color);
    paint.setStroke(true);
    paint.setStrokeWidth(1);
    canvas->drawLine(startPos, pos, paint);
    caretVisible = !caretVisible;
}
SkPoint GlyphBox::getInputPos()
{
    auto x = infos[caretY].wordPos[caretX].fX + infos[caretY].x;
    auto y = infos[caretY].wordPos[caretX].fY + infos[caretY].y;
    return SkPoint(x, y + fontBottom);
}

void GlyphBox::moveCaretLeft()
{
    if (caretX == 0 && caretY == 0) return;
    caretX -= 1;
    if (caretX < 0) {
        caretY -= 1;
        if (caretY < 0) {
            caretX = 0;
            caretY = 0;
        }
        else {
            caretX = infos[caretY].wordPos.size() - 1;
        }
    }
    refreshCaret();
}
void GlyphBox::moveCaretRight()
{
    if (caretX == infos[caretY].wordPos.size() - 1 && caretY == infos.size() - 1) {
        return;
    }
    caretX += 1;
    if (caretX >= infos[caretY].wordPos.size()) {
        caretY += 1;
        if (caretY >= infos.size()) {
            caretY = infos.size() - 1;
            caretX = infos[caretY].wordPos.size()-1;
        }
        else {
            caretX = 0;
        }
    }
    refreshCaret();
}
void GlyphBox::moveCaretUp()
{
    if (caretY == 0) {
        return;
    }
    caretY -= 1;
    if (caretX >= infos[caretY].wordPos.size()) {
        caretX = infos[caretY].wordPos.size() - 1;
    }
    refreshCaret();
}
void GlyphBox::moveCaretDown()
{
    if (caretY == infos.size() - 1) {
        return;
    }
    caretY += 1;
    if (caretX >= infos[caretY].wordPos.size()) {
        caretX = infos[caretY].wordPos.size() - 1;
    }
    refreshCaret();
}

void GlyphBox::moveCaret(const int& x, const int& y)
{
    auto lineHight = getLineHeight();
    bool flag{ false };
    caretY = 0;
    for (size_t i = 0; i < infos.size(); i++)
    {
        if (y < lineHight + infos[i].y) {
            caretY = i;
            flag = true;
            break;
        }
    }
    if (!flag) {
        caretY = infos.size() - 1;
    }
    flag = false;
    for (size_t i = 0; i < infos[caretY].wordPos.size()-1; i++)
    {
        if (x < padding + infos[caretY].wordPos[i].fX + (infos[caretY].wordPos[i + 1].fX - infos[caretY].wordPos[i].fX) / 2) {
            caretX = i;
            flag = true;
            break;
        }
    }
    if (!flag) {
        caretX = infos[caretY].wordPos.size() - 1;
    }
    refreshCaret();
}

void GlyphBox::refreshCaret()
{
    win->paintState = 2;
    caretVisible = true;
    InvalidateRect(win->hwnd, nullptr, false);
}

void GlyphBox::initFont()
{
    auto fontMgr = SkFontMgr_New_GDI();
    auto typeFace = fontMgr->matchFamilyStyle("Microsoft YaHei Light", SkFontStyle());
    font = SkFont(typeFace, fontSize);
    font.setEdging(SkFont::Edging::kSubpixelAntiAlias);
    font.setSubpixel(true);
    SkFontMetrics metrics;
    font.getMetrics(&metrics);
    fontTop = metrics.fTop;
    fontBottom = metrics.fBottom;
}
void GlyphBox::initInfo()
{
    auto height = getLineHeight();
    std::wstringstream ss(text);
    std::wstring line;
    int lineIndex{ 0 };
    while (std::getline(ss, line)) {
        GlyphInfo info;
        info.x = padding;
        info.y = lineIndex * height * lineSpace + padding;
        auto length = line.size() * sizeof(wchar_t);
        info.glyphs.resize(line.size());
        auto glyphCount = font.textToGlyphs(line.data(), length, SkTextEncoding::kUTF16, info.glyphs.data(), line.size());
        std::vector<SkScalar> widths;
        widths.resize(glyphCount);
        font.getWidthsBounds(info.glyphs.data(), glyphCount, widths.data(), nullptr, nullptr);
        info.wordPos.resize(glyphCount + 1);
        SkScalar x = 0;
        for (int i = 0; i < glyphCount; ++i) {
            info.wordPos[i] = SkPoint::Make(x, height);
            x += widths[i];
        }
        info.wordPos[glyphCount] = SkPoint::Make(x, height);
        lineIndex += 1;
        infos.push_back(info);
    }
}
float GlyphBox::getLineHeight()
{
    static float lineHeight = [this]() {
        SkFontMetrics metrics;
        font.getMetrics(&metrics);
        auto height = metrics.fBottom - metrics.fTop;
        return height;
        }();
    return lineHeight;
}
