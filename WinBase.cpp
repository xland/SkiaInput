#include "WinBase.h"

WinBase::WinBase()
{
}

WinBase::~WinBase()
{
}
void WinBase::show()
{
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);
    onShown();
}
void WinBase::createWindow()
{
    const TCHAR clsName[] = L"SkiaApp";
    static WNDCLASSEX wcex;
    static bool isClsReg = false;
    auto hinstance = GetModuleHandle(NULL);
    if (!isClsReg) {
        wcex.cbSize = sizeof(WNDCLASSEX);
        wcex.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS | CS_OWNDC | CS_BYTEALIGNCLIENT;
        wcex.lpfnWndProc = &WinBase::wndProc;
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = 0;
        wcex.hInstance = hinstance;
        wcex.hIcon = LoadIcon(hinstance, IDI_APPLICATION);
        wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wcex.lpszMenuName = nullptr;
        wcex.lpszClassName = clsName;
        wcex.hIconSm = LoadIcon(hinstance, IDI_APPLICATION);
        if (!RegisterClassEx(&wcex)) {
            return;
        }
        isClsReg = true;
    }
    hwnd = CreateWindowEx(NULL, clsName, clsName, WS_OVERLAPPEDWINDOW,
        x, y, w, h, nullptr, nullptr, hinstance, nullptr);
    SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)this);
    ctx = Context::create(this);
}

LRESULT WinBase::wndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    auto win = (WinBase*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
    switch (msg)
    {
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC dc = BeginPaint(hWnd, &ps);
        win->paint(win->ctx->getCanvas());
        win->ctx->paint(dc);
        ReleaseDC(hWnd, dc);
        EndPaint(hWnd, &ps);
        return 0;
    }
    case WM_ACTIVATE: {
        // disable/enable rendering here, depending on wParam != WA_INACTIVE
        break;
    }
    case WM_SIZE: {
        win->w = LOWORD(lParam);
        win->h = HIWORD(lParam);
        win->ctx->resize();
        win->onSize();
        return 0;
    }
    case WM_MOVE: {
        win->x = LOWORD(lParam);
        win->y = HIWORD(lParam);
        return 0;
    }
    case WM_DESTROY:
    {
        SetWindowLongPtr(hWnd, GWLP_USERDATA, 0);
        return 0;
    }
    //case WM_IME_ENDCOMPOSITION:
    //{
    //    HIMC himc = ImmGetContext(hWnd);
    //    ImmNotifyIME(himc, NI_COMPOSITIONSTR, CPS_CANCEL, 0); // 清空输入内容
    //    ImmReleaseContext(hWnd, himc);
    //    return 0;
    //}
    default: {
        if (win) {
            return win->procMsg(msg, wParam, lParam);
        }
        break;
    }
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK WinBase::procMsg(UINT msg, WPARAM wParam, LPARAM lParam) {
    return DefWindowProc(hwnd, msg, wParam, lParam);
}