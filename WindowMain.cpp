#include <sstream>
#include "WindowMain.h"


WindowMain::WindowMain()
{
    initPosSize();
    initWindow();
    initAlpha();
    glyphBox.init(this);
}

WindowMain::~WindowMain()
{
}

void WindowMain::onPaint(SkCanvas* canvas)
{
    if (paintState == 1) {
        glyphBox.paintCaret(canvas);
        paintState = 0;
        return;
    }
    canvas->clear(glyphBox.colorBg);
    glyphBox.paintSelectBg(canvas);
    glyphBox.paintText(canvas);
}

void WindowMain::onShown()
{
    SetTimer(hwnd, 1001, 600, NULL);
    glyphBox.refreshCaret();
}

void WindowMain::onTimer(const uint32_t& key)
{
    if (key == 1001) {
        paintState = 1;
        InvalidateRect(hwnd, nullptr, false);
    }
}

void WindowMain::onIme()
{
    if (HIMC himc = ImmGetContext(hwnd)) {
        auto pos = glyphBox.getInputPos();
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
        glyphBox.moveCaretLeft();
    }
    else if (key == VK_UP) {
        glyphBox.moveCaretUp();
    }
    else if (key == VK_RIGHT) {
        glyphBox.moveCaretRight();
    }
    else if (key == VK_DOWN) {
        glyphBox.moveCaretDown();
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

void WindowMain::onMousePress(const int& x, const int& y)
{
    glyphBox.moveCaret(x, y);
    glyphBox.caretXStart = glyphBox.caretX;
    glyphBox.caretYStart = glyphBox.caretY;
    glyphBox.caretXEnd = -1;
    glyphBox.caretYEnd = -1;
    glyphBox.refreshCaret();
}

void WindowMain::onMouseDrag(const int& x, const int& y)
{
    glyphBox.moveCaret(x, y);
    glyphBox.caretXEnd = glyphBox.caretX;
    glyphBox.caretYEnd = glyphBox.caretY;
    //glyphBox.adjustSelection();
    glyphBox.refreshCaret();
}

void WindowMain::onMouseRelease(const int& x, const int& y)
{
    glyphBox.checkCancelSelection();

}

