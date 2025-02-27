﻿#include "WindowMain.h"
#include "WindowCaret.h"


WindowMain::WindowMain()
{
    addHistory();
    initWinPosSize();
    initWindow();
	alphaWindow();
    initFont();
    caretWin = std::make_shared<WindowCaret>(getLineHeight(), this);
    initInfo();
	funcPaint.push_back(std::bind(&WindowMain::onPaint, this, std::placeholders::_1));
	funcShown.push_back(std::bind(&WindowMain::onShown, this));
	funcMousePress.push_back(std::bind(&WindowMain::onMousePress, this, std::placeholders::_1, std::placeholders::_2));
	funcMouseDrag.push_back(std::bind(&WindowMain::onMouseDrag, this, std::placeholders::_1, std::placeholders::_2));
	funcMouseRelease.push_back(std::bind(&WindowMain::onMouseRelease, this, std::placeholders::_1, std::placeholders::_2));
    funcChar.push_back(std::bind(&WindowMain::onChar, this, std::placeholders::_1));
    funcKeyDown.push_back(std::bind(&WindowMain::onKey, this, std::placeholders::_1));
    funcKeyDownWithCtrl.push_back(std::bind(&WindowMain::onKeyWithCtrl, this, std::placeholders::_1));
    funcIme.push_back(std::bind(&WindowMain::onIme, this));
}

WindowMain::~WindowMain()
{
}

void WindowMain::onPaint(SkCanvas* canvas)
{
    canvas->clear(colorBg);
    paintSelectBg(canvas);
    paintText(canvas);
    paintScroller(canvas);
}

void WindowMain::onShown()
{
}

void WindowMain::initWinPosSize()
{
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    w = 1200;
    h = 800;
    x = (screenWidth - w) / 2;
    y = (screenHeight - h) / 2;
}


void WindowMain::refreshCaret()
{
    auto lineHeight = getLineHeight();
    auto pos = getCaretPos(caretX, caretY);
    caretWin->move(pos.fX, pos.fY - lineHeight);
}


void WindowMain::onIme()
{
    if (HIMC himc = ImmGetContext(hwnd)) {
        auto pos = getCaretPos(caretX, caretY);
        long x{ (long)pos.fX }, y{ (long)pos.fY - 8 };
        CANDIDATEFORM cf = {};
        cf.dwIndex = 0;
        cf.dwStyle = CFS_EXCLUDE;
        cf.ptCurrentPos.x = x;
        cf.ptCurrentPos.y = y;
        ImmSetCandidateWindow(himc, &cf);
        COMPOSITIONFORM cfComp = {};
        cfComp.dwStyle = CFS_POINT;
        cfComp.ptCurrentPos.x = x;
        cfComp.ptCurrentPos.y = y;
        ImmSetCompositionWindow(himc, &cfComp);
        ImmReleaseContext(hwnd, himc);
    }
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
}
void WindowMain::initInfo()
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
    auto pos = getCaretPos(caretX, caretY);
    caretWin->move(pos.fX, pos.fY - height);
}
float WindowMain::getLineHeight()
{
    static float lineHeight = [this]() {
        SkFontMetrics metrics;
        font.getMetrics(&metrics);
        auto height = metrics.fBottom - metrics.fTop;
        return height;
        }();
    return lineHeight;
}
int WindowMain::getCharIndex(const int& caretX, const int& caretY)
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
    return charIndex;
}

SkPoint WindowMain::getCaretPos(const int& caretX, const int& caretY)
{
    auto x = infos[caretY].wordPos[caretX].fX + infos[caretY].x;
    auto y = infos[caretY].wordPos[caretX].fY + infos[caretY].y;
    return SkPoint(x, y + fontBottom);
}

void WindowMain::moveCaret(const int& x, const int& y)
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
    for (size_t i = 0; i < infos[caretY].wordPos.size() - 1; i++)
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

bool WindowMain::delSelected()
{
    auto flag1 = (caretXStart == -1 || caretYStart == -1 || caretXEnd == -1 || caretYEnd == -1);
    auto flag2 = (caretXStart == caretXEnd && caretYStart == caretYEnd);
    if (!flag1 && !flag2) {
        auto [charIndex0, charIndex1] = getSelectionCharIndex();
        text.erase(charIndex0, charIndex1 - charIndex0);
        caretX = std::min(caretXStart,caretXEnd);
        caretY = std::min(caretYStart,caretXEnd);
        caretXStart = -1;  caretYStart = -1;
        caretXEnd = -1;  caretYEnd = -1;
        return true;
    }
    return false;
}

void WindowMain::addHistory()
{
    if (historyIndex > 0) {
        history.erase(history.begin(), history.begin() + historyIndex);
        historyIndex = 0;
    }
    history.insert(history.begin(), { text,caretX,caretY });
}

std::pair<int, int> WindowMain::getSelectionCharIndex()
{
    auto charIndex0 = getCharIndex(caretXStart, caretYStart);
    auto charIndex1 = getCharIndex(caretXEnd, caretYEnd);
    if (charIndex0 > charIndex1) std::swap(charIndex0, charIndex1);
    return { charIndex0,charIndex1 };
}
