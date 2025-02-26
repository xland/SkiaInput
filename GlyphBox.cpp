#include "GlyphBox.h"
#include "WindowMain.h"
#include "WindowCaret.h"

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
	caretWin = std::make_shared<WindowCaret>(getLineHeight(), win);
	win->funcChar.push_back(std::bind(&GlyphBox::onChar, this, std::placeholders::_1));
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
    refreshCaret();
}
void GlyphBox::paintSelectBg(SkCanvas* canvas)
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
        canvas->drawRect(SkRect::MakeLTRB(startPos.fX, startPos.fY-lineHeight, endPos.fX, endPos.fY), paint);
    }
    else {
        auto startLineSize = infos[line1].wordPos.size();
        auto rect = SkRect::MakeLTRB(startPos.fX, startPos.fY - lineHeight,
            infos[line1].wordPos[startLineSize - 1].fX + infos[line1].x, startPos.fY);
        canvas->drawRect(rect, paint);
        for (size_t i = line1 +1; i <= line2-1; i++)
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
SkPoint GlyphBox::getInputPos()
{
    return getCaretPos(caretX, caretY);
}

SkPoint GlyphBox::getCaretPos(const int& caretX, const int& caretY)
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

void GlyphBox::checkCancelSelection()
{
    if (caretYEnd == -1 || caretXEnd == -1 || (caretXStart == caretXEnd && caretYStart == caretYEnd)) {
        caretXStart = -1;
        caretXEnd = -1;
        caretYStart = -1;
        caretYEnd = -1;
        InvalidateRect(win->hwnd, nullptr, false);
    }
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
    infos.clear();
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

void GlyphBox::refreshCaret()
{
    auto lineHeight = getLineHeight();
    auto pos = getInputPos();
    caretWin->moveCaret(pos.fX, pos.fY - lineHeight);
}

void GlyphBox::onChar(const std::wstring& str)
{
    std::wstringstream ss(text);
    std::wstring line;
    int lineIndex{ 0 };
    int charIndex{ 0 };
    while (std::getline(ss, line)) {
        if (caretY == lineIndex) {
			charIndex += caretX;
            break;
        }
        else {
            charIndex += line.length() + 1;
            lineIndex += 1;
        }
    }

    if (str == L"\r") {
		text.insert(charIndex, L"\n");
		caretX = 0;
		caretY += 1;
    }
    else if (str == L"\b")
    {

    }
    else {
        text.insert(charIndex, str);
        caretX += str.length();
    }
    initInfo();
	InvalidateRect(win->hwnd, nullptr, false);
}
