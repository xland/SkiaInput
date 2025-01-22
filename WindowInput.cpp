#include <dwmapi.h>
#include <versionhelpers.h>
#include "WindowInput.h"

WindowInput::WindowInput()
{
    initText();
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
    activeKeyboard();
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
	fontAsent = metrics.fAscent;
	fontDesent = metrics.fDescent;
}

void WindowInput::initSize()
{
    w = 800; h = 600;
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    x = (screenWidth - w) / 2;
    y = (screenHeight - h) / 2;
}

void WindowInput::initText()
{
    std::wstring text{ LR"(破阵子·为陈同甫赋壮词以寄之
辛弃疾 · 宋 · XinQiJi(1140年－1207年) 

醉里挑灯看剑，梦回吹角连营。
八百里分麾下炙，五十弦翻塞外声，沙场秋点兵。
马作的卢飞快，弓如霹雳弦惊。
了却君王天下事，赢得生前身后名。可怜白发生！
)" };

    std::wstring currentLine;
    for (size_t i = 0; i < text.size(); ++i) {
        if (text[i] == L'\n' || (text[i] == L'\r' && (i + 1 < text.size() && text[i + 1] == L'\n'))) {
            // 如果遇到 '\n' 或者 '\r\n'（Windows换行），调用特定方法
            lines.push_back(currentLine);
            // 清空当前行准备处理下一行
            currentLine.clear();
            // 如果是 "\r\n" 组合，跳过 '\r'
            if (text[i] == L'\r' && i + 1 < text.size() && text[i + 1] == L'\n') {
                ++i; // 跳过下一个 '\n'
            }
        }
        else {
            // 否则将当前字符添加到当前行
            currentLine.push_back(text[i]);
        }
    }
    // 处理最后一行（如果文本没有以换行符结束）
    if (!currentLine.empty()) {
        lines.push_back(currentLine);
    }
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
    wcx.hCursor = LoadCursor(NULL, IDC_IBEAM);
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

void WindowInput::activeKeyboard()
{
    if (HIMC himc = ImmGetContext(hwnd))
    {
        auto x = wordPos[caretLineIndex][caretWordIndex].fX;
        auto y = wordPos[caretLineIndex][caretWordIndex].fY + caretLineIndex * (fontBottom - fontTop);
        COMPOSITIONFORM comp = {};
        comp.ptCurrentPos.x = x;
        comp.ptCurrentPos.y = y;
        comp.dwStyle = CFS_FORCE_POSITION;
        ImmSetCompositionWindow(himc, &comp);
        CANDIDATEFORM cand = {};
        cand.dwStyle = CFS_CANDIDATEPOS;
        cand.ptCurrentPos.x = x;
        cand.ptCurrentPos.y = y;
        ImmSetCandidateWindow(himc, &cand);
        ImmReleaseContext(hwnd, himc);
    }
}