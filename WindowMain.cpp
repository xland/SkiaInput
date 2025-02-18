#include <sstream>
#include "WindowMain.h"


WindowMain::WindowMain()
{
    initPosSize();
    initWindow();
    initAlpha();
    initFont();
}

WindowMain::~WindowMain()
{
}

void WindowMain::onPaint(SkCanvas* canvas)
{
    canvas->clear(0x22FF66FF);
    std::wstringstream ss(text);
    SkPaint paint;
    paint.setColor(SK_ColorBLACK);
    paint.setAntiAlias(true);

    std::wstring line;
    int lineIndex{ 0 };
    while (std::getline(ss, line)) {
        auto length = line.size() * sizeof(wchar_t);
        std::vector<SkGlyphID> glyphs(line.size());
        int glyphCount = font.textToGlyphs(line.data(), length, SkTextEncoding::kUTF16, glyphs.data(), line.size());
        std::vector<SkScalar> widths(glyphCount);
        font.getWidthsBounds(glyphs.data(), glyphCount, widths.data(), nullptr, nullptr);
        SkScalar x = 12;
        float height{ fontBottom - fontTop };
        std::vector<SkPoint> wordPos;
        for (int i = 0; i < glyphCount; ++i) {
            wordPos.push_back(SkPoint::Make(x, height));
            x += widths[i]; // 累计宽度
        }
        wordPos.push_back(SkPoint::Make(x, height));
        canvas->drawGlyphs(glyphCount, glyphs.data(), wordPos.data(), SkPoint(0, lineIndex * height+12.f), font, paint);
        lineIndex += 1;
    }
}

void WindowMain::onShown()
{
    //flashCaret();
    //SetTimer(hwnd, 1001, 600, NULL);
    //activeKeyboard();
}

void WindowMain::onSize()
{
    //paintText();
}

void WindowMain::onKeyDown(const uint32_t& key)
{
    if (key == VK_BACK || key == VK_DELETE) {
        //removeShape();

    }
    else if (key == VK_ESCAPE) {
        //escPress();

    }
    else if (key == VK_LEFT) {
        //moveByKey(0);

    }
    else if (key == VK_UP) {
        //moveByKey(1);

    }
    else if (key == VK_RIGHT) {
        //moveByKey(2);

    }
    else if (key == VK_DOWN) {
        //moveByKey(3);
    }
}

void WindowMain::onKeyDownWithCtrl(const uint32_t& key)
{
    if (key == 'Z') {
        //undo(); 撤销
    }
    else if (key == 'Y') {
        //redo(); 重做
    }
    else if (key == 'C') {
        //copyColor(0); Hex
    }
    else if (key == 'V') {
        //copyColor(1); RGB
    }
    else if (key == 'X') {
        //copyColor(1); RGB
    }
    else if (key == 'A') {
        //copyColor(1); RGB
    }
}

void WindowMain::initPosSize()
{
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    w = 1200;
    h = 800;
    x = (screenWidth - w) / 2;
    y = (screenHeight - h) / 2;
}

float WindowMain::getLineHeight()
{
    static float lineHeight = [this]() {
        //BLFontMetrics fm = font->metrics();
        //return fm.ascent - fm.descent + fm.lineGap + lineSpan;
        return 24;
        }();
    return lineHeight;
}

void WindowMain::initFont()
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
    fontAsent = metrics.fAscent;
    fontDesent = metrics.fDescent;
}


