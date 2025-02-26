﻿#include <sstream>
#include "WindowMain.h"
#include "WindowCaret.h"


WindowMain::WindowMain()
{
    initPosSize();
    initWindow();
	Util::alphaWindow(hwnd);
    glyphBox.init(this);

	funcPaint.push_back(std::bind(&WindowMain::onPaint, this, std::placeholders::_1));
	funcShown.push_back(std::bind(&WindowMain::onShown, this));
	funcSize.push_back(std::bind(&WindowMain::onSize, this));
	funcKeyDownWithCtrl.push_back(std::bind(&WindowMain::onKeyDownWithCtrl, this, std::placeholders::_1));
	funcMousePress.push_back(std::bind(&WindowMain::onMousePress, this, std::placeholders::_1, std::placeholders::_2));
	funcMouseDrag.push_back(std::bind(&WindowMain::onMouseDrag, this, std::placeholders::_1, std::placeholders::_2));
	funcMouseRelease.push_back(std::bind(&WindowMain::onMouseRelease, this, std::placeholders::_1, std::placeholders::_2));
	funcIme.push_back(std::bind(&WindowMain::onIme, this));
}

WindowMain::~WindowMain()
{
}

void WindowMain::onPaint(SkCanvas* canvas)
{
    canvas->clear(glyphBox.colorBg);
    glyphBox.paintSelectBg(canvas);
    glyphBox.paintText(canvas);
}

void WindowMain::onShown()
{
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
}

void WindowMain::onMouseDrag(const int& x, const int& y)
{
    glyphBox.moveCaret(x, y);
    glyphBox.caretXEnd = glyphBox.caretX;
    glyphBox.caretYEnd = glyphBox.caretY;
    InvalidateRect(hwnd, nullptr, false);
}

void WindowMain::onMouseRelease(const int& x, const int& y)
{
    glyphBox.checkCancelSelection();
}

