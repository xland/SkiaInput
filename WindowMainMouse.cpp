#include "WindowMain.h"
void WindowMain::onMousePress(const int& x, const int& y)
{
    moveCaret(x, y);
    caretXStart = caretX;
    caretYStart = caretY;
    caretXEnd = -1;
    caretYEnd = -1;
}
void WindowMain::onMouseDrag(const int& x, const int& y)
{
    moveCaret(x, y);
    caretXEnd = caretX;
    caretYEnd = caretY;
    InvalidateRect(hwnd, nullptr, false);
}
void WindowMain::onMouseRelease(const int& x, const int& y)
{
    if (caretYEnd == -1 || caretXEnd == -1 || (caretXStart == caretXEnd && caretYStart == caretYEnd)) {
        caretXStart = -1;
        caretXEnd = -1;
        caretYStart = -1;
        caretYEnd = -1;
        InvalidateRect(hwnd, nullptr, false);
    }
}



