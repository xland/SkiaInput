#include <windowsx.h>
#include <dwmapi.h>
#include <versionhelpers.h>
#include "WindowInput.h"

WindowInput::WindowInput()
{
    initFont();
    initSize();
    initSurface();
    initWindow();
}

WindowInput::~WindowInput()
{
}

void WindowInput::show()
{
    SetTimer(hwnd, 1001, 600, NULL);
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);
}

void WindowInput::initFont()
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
}

void WindowInput::paintText()
{
    SkPaint paint;
    paint.setColor(0xFF00FFFF);
    paint.setStroke(false);

    auto length = text.size() * sizeof(wchar_t);
    std::vector<SkGlyphID> glyphs(text.size());
    int glyphCount = font.textToGlyphs(text.data(), length, SkTextEncoding::kUTF16, glyphs.data(), text.size());
    std::vector<SkScalar> widths(glyphCount);
    font.getWidthsBounds(glyphs.data(), glyphCount, widths.data(), nullptr, nullptr);
    wordPos.resize(glyphCount);
    
    SkScalar x = 12;
    for (int i = 0; i < glyphCount; ++i) {
        wordPos[i] = SkPoint::Make(x, fontBottom - fontTop);
        x += widths[i]; // 累计宽度
    }
    SkCanvas* canvas = surface->getCanvas();
    canvas->clear(0xFF345678);
    canvas->drawGlyphs(glyphCount, glyphs.data(), wordPos.data(), SkPoint(0, 0), font, paint);
}

void WindowInput::flashCaret()
{
    auto color = caretVisible ? 0xFF00FFFF : 0xFF345678;
    auto x = wordPos[caretIndex].fX,y = wordPos[caretIndex].fY;
    SkPoint start = SkPoint(x, y + fontTop);// 字符顶部相对于基线的偏移  neagtive
    SkPoint end = SkPoint(x, y + fontBottom); // 字符底部相对于基线的偏移
    SkCanvas* canvas = surface->getCanvas();
    SkPaint paint;
    paint.setColor(color);
    paint.setStroke(true);
    paint.setStrokeWidth(1);
    canvas->drawLine(start, end, paint);
    caretVisible = !caretVisible;
    InvalidateRect(hwnd, nullptr, true);
}


void WindowInput::paint()
{
    if (w <= 0 || h <= 0) return;
    SkPixmap pix;
    surface->peekPixels(&pix);
    PAINTSTRUCT ps;
    auto dc = BeginPaint(hwnd, &ps);
    BITMAPINFO bmi = { sizeof(BITMAPINFOHEADER), w, 0 - h, 1, 32, BI_RGB, h * 4 * w, 0, 0, 0, 0 };
    SetDIBitsToDevice(dc, 0, 0, w, h, 0, 0, 0, h, pix.addr(), &bmi, DIB_RGB_COLORS);
    ReleaseDC(hwnd, dc);
    EndPaint(hwnd, &ps);
}

void WindowInput::initSize()
{
    w = 800; h = 600;
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    x = (screenWidth - w) / 2;
    y = (screenHeight - h) / 2;
}

void WindowInput::initWindow()
{
    WNDCLASSEX wcx{};
    auto hinstance = GetModuleHandle(NULL);
    wcx.cbSize = sizeof(wcx);
    wcx.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wcx.lpfnWndProc = &WindowInput::routeWinMsg;
    wcx.cbWndExtra = sizeof(WindowInput*);
    wcx.hInstance = hinstance;
    wcx.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcx.lpszClassName = L"SkiaInput";
    auto flag = RegisterClassEx(&wcx);
    hwnd = CreateWindowEx(NULL, wcx.lpszClassName, wcx.lpszClassName, WS_POPUP, x, y, w, h, NULL, NULL, hinstance, static_cast<LPVOID>(this));
    enableAlpha();
    SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
}

void WindowInput::initSurface()
{
    SkImageInfo info = SkImageInfo::MakeN32Premul(w, h);
    surface = SkSurfaces::Raster(info);

    paintText();
}

bool WindowInput::enableAlpha()
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
    return DefWindowProc(hwnd, msg, wParam, lParam);
}
