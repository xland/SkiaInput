#include <sstream>
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
	funcMousePress.push_back(std::bind(&WindowMain::onMousePress, this, std::placeholders::_1, std::placeholders::_2));
	funcMouseDrag.push_back(std::bind(&WindowMain::onMouseDrag, this, std::placeholders::_1, std::placeholders::_2));
	funcMouseRelease.push_back(std::bind(&WindowMain::onMouseRelease, this, std::placeholders::_1, std::placeholders::_2));
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

void WindowMain::onSize()
{
    //paintText();
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

