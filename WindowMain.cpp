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
    canvas->clear(colorBg);
    glyphBox.paintText(canvas);
}

void WindowMain::onShown()
{
    SetTimer(hwnd, 1001, 600, NULL);
    auto pos = glyphBox.getInputPos();
    activeKeyboard(pos.fX,pos.fY);
}

void WindowMain::onTimer(const uint32_t& key)
{
    paintState = 1;
    InvalidateRect(hwnd, nullptr, false);
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




