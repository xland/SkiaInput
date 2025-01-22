#include <windowsx.h>
#include "WindowInput.h"


LRESULT WindowInput::routeWinMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    auto obj = reinterpret_cast<WindowInput*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
    if (!obj) {
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }
    switch (msg)
    {
    case WM_NCCALCSIZE:
    {
        if (wParam == TRUE) {
            NCCALCSIZE_PARAMS* pncsp = reinterpret_cast<NCCALCSIZE_PARAMS*>(lParam);
            pncsp->rgrc[0] = pncsp->rgrc[1]; //窗口客户区覆盖整个窗口
            return 0;
        }
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }
    case WM_ERASEBKGND: {
        return 0;
    }
    case WM_DESTROY:
    {
        SetWindowLongPtr(hWnd, GWLP_USERDATA, NULL);
        UnregisterClass(L"SkiaInput", NULL);
        PostQuitMessage(0);
        return 0;
    }
    default:
    {
        return obj->processWinMsg(msg, wParam, lParam);
    }
    }
}

LRESULT WindowInput::processWinMsg(UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_PAINT) {
        paint();
        return 0;
    }
    else if (msg == WM_TIMER) {
        if (wParam == 1001)
        {
            flashCaret();
            InvalidateRect(hwnd, nullptr, false);
        }
    }
    else if (msg == WM_SIZE) {
        w = LOWORD(lParam);
        h = HIWORD(lParam);
        initSurface();
        return 0;
    }
    else if (msg == WM_MOVE) {
        x = LOWORD(lParam);
        y = HIWORD(lParam);
        return 0;
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