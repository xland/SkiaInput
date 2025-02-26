#include <memory>
#include "WindowCaret.h"
#include "Util.h"
#include "WindowMain.h"


WindowCaret::WindowCaret(const int& height,WindowMain* win):h{height},win{win}
{
    initWindow();
    pixels.resize(w * h);
    SetTimer(hwnd, 1001, 600, NULL);
	win->funcMove.push_back(std::bind(&WindowCaret::moveWin, this, std::placeholders::_1, std::placeholders::_2));
}

WindowCaret::~WindowCaret()
{
}

void WindowCaret::moveCaret(const int& x, const int& y)
{
	SetWindowPos(hwnd, HWND_TOPMOST, 
        x, y, 0, 0,
        SWP_NOSIZE | SWP_NOACTIVATE| SWP_NOZORDER| SWP_SHOWWINDOW);
}

void WindowCaret::moveWin(const int& x, const int& y)
{
	//SetWindowPos(hwnd, HWND_TOPMOST,
 //       win->x + this->x, win->y + this->y, 0, 0, 
 //       SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOZORDER | SWP_SHOWWINDOW);
}

void WindowCaret::initWindow()
{
    const TCHAR clsName[] = L"WindowCaret";
    static WNDCLASSEX wcx;
    auto hinstance = GetModuleHandle(NULL);
    wcx.cbSize = sizeof(wcx);
    wcx.style = CS_HREDRAW | CS_VREDRAW;
    wcx.lpfnWndProc = &WindowCaret::routeWinMsg;
    wcx.cbWndExtra = sizeof(WindowCaret*);
    wcx.hInstance = hinstance;
    wcx.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcx.hCursor = LoadCursor(NULL, IDC_IBEAM);
    wcx.lpszClassName = clsName;
    RegisterClassEx(&wcx);
    hwnd = CreateWindowEx(WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_NOACTIVATE | WS_EX_TOPMOST,  
        clsName, clsName, WS_POPUP, 0, 0, w, h, nullptr, nullptr, hinstance, nullptr);
    SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)this);

    SetParent(hwnd, win->hwnd);
}

void WindowCaret::onPaint()
{
    std::fill(pixels.begin(), pixels.end(), color);
    HDC hdc = GetDC(hwnd);
    HDC hdcMem = CreateCompatibleDC(hdc);
    BITMAPINFO bmi = { sizeof(BITMAPINFOHEADER), w, -h, 1, 32, BI_RGB };
    void* pBits;
    HBITMAP hBitmap = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, &pBits, NULL, 0);
    if (!hBitmap) return;
    memcpy(pBits, pixels.data(), w * h * sizeof(DWORD));
    DeleteObject(SelectObject(hdcMem, hBitmap));
    BLENDFUNCTION blend = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
    POINT pSrc = { 0, 0 };
    SIZE size = { w, h };
    UpdateLayeredWindow(hwnd, hdc, NULL, &size, hdcMem, &pSrc, 0, &blend, ULW_ALPHA);
    DeleteObject(hBitmap);
    DeleteDC(hdcMem);
    ReleaseDC(hwnd, hdc);
    color = color == 0xFF000000 ? 0x00000000 : 0xFF000000;
}

LRESULT WindowCaret::routeWinMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    auto obj = reinterpret_cast<WindowCaret*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
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
                return 0; //确认改变窗口客户区
            }
            return DefWindowProc(hWnd, msg, wParam, lParam);
        }
        case WM_WINDOWPOSCHANGING: {
            WINDOWPOS* pPos = (WINDOWPOS*)lParam;
            if (!(pPos->flags & SWP_NOZORDER)) {
                pPos->flags |= SWP_NOZORDER;
            }
            return 0;
        }
        case WM_NCHITTEST: {
            return HTTRANSPARENT;
        }
        case WM_CLOSE:
        {
            DestroyWindow(hWnd);
            return 0;
        }
        case WM_DESTROY:
        {
            SetWindowLongPtr(hWnd, GWLP_USERDATA, 0);
            UnregisterClass(L"WindowCaret", nullptr);
            return 0;
        }
        case WM_TIMER: {
            if (wParam == 1001) {
				obj->onPaint();
            }
            return 0;
        }
        default:
        {
            return DefWindowProc(hWnd, msg, wParam, lParam);
        }
    }
}
