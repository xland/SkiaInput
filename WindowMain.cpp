#include <windowsx.h>
#include <include/core/SkFontMgr.h>
#include <include/core/SkFontStyle.h>
#include <include/ports/SkTypeface_win.h>
#include <include/core/SkFontMetrics.h>

#include "WindowMain.h"

WindowMain::WindowMain()
{
    initText();
    initFont();
    initPosSize();
    createWindow();
}

WindowMain::~WindowMain()
{
}

void WindowMain::onShown()
{
    paintText();
    //SetTimer(hwnd, 1001, 600, NULL);
    activeKeyboard();
}

void WindowMain::onSize()
{
    paintText();
}

void WindowMain::initPosSize()
{
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    w = 800;
    h = 600;
    x = (screenWidth - w) / 2;
    y = (screenHeight - h) / 2;
}


void WindowMain::initText()
{
    std::wstring text{ LR"(破阵子·为陈同甫赋壮词以寄之
辛弃疾 · 宋 · XinQiJi(1140年－1207年) 

醉里挑灯看剑，梦回吹角连营。
八百里分麾下炙，五十弦翻塞外声，沙场秋点兵。
马作的卢飞快，弓如霹雳弦惊。
了却君王天下事，赢得生前身后名。可怜白发生！
)" };
    lines = textToLines(text);
}

void WindowMain::initFont()
{
    auto fontMgr = SkFontMgr_New_GDI();
    auto fontStyle = SkFontStyle::Normal();    
    auto typeFace = fontMgr->matchFamilyStyle("Microsoft YaHei", fontStyle);
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


LRESULT WindowMain::procMsg(UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_TIMER) {
        if (wParam == 1001)
        {
            flashCaret();
            InvalidateRect(hwnd, nullptr, false);
        }
    }
    else if (msg == WM_LBUTTONDOWN) {
        isMouseDown = true;
        onMouseDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        return 0;
    }
    else if (msg == WM_LBUTTONDBLCLK) {
        onDoubleClick(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        return 0;
    }
    else if (msg == WM_LBUTTONUP) {
        isMouseDown = false;
        onMouseUp(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        return 0;
    }
    else if (msg == WM_MOUSEMOVE) {
        if (isMouseDown) {
            onMouseDrag(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        }
        else {
            onMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        }
        return 0;
    }
    else if (msg == WM_RBUTTONDOWN) {
        onMouseDownRight(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        return 0;
    }    
    else if (msg == WM_KEYDOWN) {
        onKeyDown(wParam);
        return 0;
    }
    else if (msg == WM_CHAR) {
        if (!iswprint(wParam)) return 0;
        onChar(wParam);
        return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}