#include "WindowMain.h"

WindowMain::WindowMain()
{
    initPosSize();
    initWindow();
    initAlpha();
    initFont();
    paintText();
}

WindowMain::~WindowMain()
{
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
    //BLFontFace face;
    ////BLResult err = face.createFromFile("C:\\Windows\\Fonts\\msyhl.ttc");
    //BLResult err = face.createFromFile("C:\\Windows\\Fonts\\simhei.ttf"); //黑体
    //if (err) {
    //    return;
    //}
    //font = std::make_unique<BLFont>();
    //font->createFromFace(face, 26.0f);
}


