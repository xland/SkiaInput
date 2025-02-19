#include "WindowBase.h"

WindowBase::WindowBase()
{
}

WindowBase::~WindowBase()
{
}

void WindowBase::initWindow()
{
    const TCHAR clsName[] = L"BlendInput";
    static WNDCLASSEX wcx;
    static bool isClsReg = false;
    auto hinstance = GetModuleHandle(NULL);
    if (!isClsReg) {
        wcx.cbSize = sizeof(wcx);
        wcx.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
        wcx.lpfnWndProc = &WindowBase::routeWinMsg;
        wcx.cbWndExtra = sizeof(WindowBase*);
        wcx.hInstance = hinstance;
        wcx.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wcx.hCursor = LoadCursor(NULL, IDC_IBEAM);
        wcx.lpszClassName = clsName;
        RegisterClassEx(&wcx);
        isClsReg = true;
    }
    //必须是WS_POPUP，不能是WS_OVERLAPPEDWINDOW，不然渲染会出问题
    hwnd = CreateWindowEx(NULL, clsName, clsName, WS_OVERLAPPEDWINDOW, x, y, w, h, nullptr, nullptr, hinstance, nullptr);
    SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)this);
    backend = Backend::create(this);
}
bool WindowBase::initAlpha()
{
    if (!IsWindowsVistaOrGreater()) { return false; }
    BOOL isCompositionEnable = false;
    //检查DWM是否启用
    DwmIsCompositionEnabled(&isCompositionEnable);
    if (!isCompositionEnable) { return true; }
    DWORD currentColor = 0;
    BOOL isOpaque = false;
    //检查是否支持毛玻璃效果
    DwmGetColorizationColor(&currentColor, &isOpaque);
    if (!isOpaque || IsWindows8OrGreater())
    {
        HRGN region = CreateRectRgn(0, 0, -1, -1);
        DWM_BLURBEHIND bb = { 0 };
        bb.dwFlags = DWM_BB_ENABLE | DWM_BB_BLURREGION;
        bb.hRgnBlur = region;
        bb.fEnable = TRUE;
        DwmEnableBlurBehindWindow(hwnd, &bb);
        DeleteObject(region);
        return true;
    }
    else // For Window7
    {
        DWM_BLURBEHIND bb = { 0 };
        bb.dwFlags = DWM_BB_ENABLE;
        DwmEnableBlurBehindWindow(hwnd, &bb);
        return false;
    }
}
void WindowBase::initContext()
{
}
void WindowBase::show()
{
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);
    onShown();
}
LRESULT WindowBase::routeWinMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    auto obj = reinterpret_cast<WindowBase*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
    if (!obj) {
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }
    switch (msg)
    {
    //case WM_NCCALCSIZE:
    //{
    //    if (wParam == TRUE) {
    //        NCCALCSIZE_PARAMS* pncsp = reinterpret_cast<NCCALCSIZE_PARAMS*>(lParam);
    //        pncsp->rgrc[0] = pncsp->rgrc[1]; //窗口客户区覆盖整个窗口
    //        return 0; //确认改变窗口客户区
    //    }
    //    return DefWindowProc(hWnd, msg, wParam, lParam);
    //}
    case WM_ERASEBKGND:
    {
        return TRUE;
    }
    case WM_CLOSE:
    {
        DestroyWindow(hWnd);
        return 0;
    }
    case WM_DESTROY:
    {
        SetWindowLongPtr(hWnd, GWLP_USERDATA, 0);
        UnregisterClass(L"ScreenCapture", nullptr);
        return 0;
    }
    default:
    {
        return obj->processWinMsg(msg, wParam, lParam);
    }
    }
}
LRESULT WindowBase::processWinMsg(UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_TIMER: {
        onTimer(wParam);
        return 0;
    }
    case WM_MOVE: {
        x = LOWORD(lParam);
        y = HIWORD(lParam);
        onMove();
        return 0;
    }
    case WM_SIZE: {
        //todo minimize
        w = LOWORD(lParam);
        h = HIWORD(lParam);
        backend->resize();
        onSize();
        return 0;
    }
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        onPaint(backend->getCanvas());
        backend->paint(hdc);
        ReleaseDC(hwnd, hdc);
        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_LBUTTONDOWN:
    {
        onMousePress(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        return 0;
    }
    case WM_MOUSEMOVE:
    {
        if (wParam & MK_LBUTTON) {
            onMouseDrag(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        }
        else {
            onMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        }
        return 0;
    }
    case WM_LBUTTONDBLCLK:
    {
        onMouseDBClick(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        return 0;
    }
    case WM_LBUTTONUP:
    {
        onMouseRelease(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        return 0;
    }
    case WM_RBUTTONDOWN:
    {
        onMousePressRight(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        return 0;
    }
    case WM_KEYDOWN:
    {
        if ((GetKeyState(VK_CONTROL) & 0x8000) != 0) {
            onKeyDownWithCtrl(wParam);
        }
        else {
            onKeyDown(wParam);
        }
        break;
    }
    case WM_IME_STARTCOMPOSITION:
    {
        onIme();
        return 0;
    }
    default: {
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    }
    return 0;
}