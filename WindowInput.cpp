#include <windowsx.h>
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
}

void WindowInput::paintLine(const std::wstring& text, const int& lineIndex,SkCanvas* canvas)
{
    wordPos.insert({ lineIndex,std::vector<SkPoint>() });
    SkPaint paint;
    paint.setColor(0xFF00FFFF);
    paint.setStroke(false);

    auto length = text.size() * sizeof(wchar_t);
    std::vector<SkGlyphID> glyphs(text.size());
    int glyphCount = font.textToGlyphs(text.data(), length, SkTextEncoding::kUTF16, glyphs.data(), text.size());
    std::vector<SkScalar> widths(glyphCount);
    font.getWidthsBounds(glyphs.data(), glyphCount, widths.data(), nullptr, nullptr);

    SkScalar x = 12;
    float height{ fontBottom - fontTop };
    for (int i = 0; i < glyphCount; ++i) {
        wordPos[lineIndex].push_back(SkPoint::Make(x, height));
        x += widths[i]; // 累计宽度
    }
    wordPos[lineIndex].push_back(SkPoint::Make(x, height));
    canvas->drawGlyphs(glyphCount, glyphs.data(), wordPos[lineIndex].data(), SkPoint(0, lineIndex * height), font, paint);
}

void WindowInput::onEnter()
{
    if (caretWordIndex != lines[caretWordIndex].length()) {
        auto str1 = lines[caretLineIndex].substr(0, caretWordIndex);
        auto str2 = lines[caretLineIndex].substr(caretWordIndex);
        lines[caretLineIndex] = str1;
        lines.insert(lines.begin() + caretLineIndex + 1, str2);
    }
    else
    {
        lines.push_back(L"");
    }
    caretLineIndex += 1;
    caretWordIndex = 0;
    paintText();
    InvalidateRect(hwnd, nullptr, false);
}

void WindowInput::onBackspace()
{
    if (lines.size() == 0) {
        return;
    }
    if (caretWordIndex == 0) {
        if (caretLineIndex == 0) {
            return;
        }
        else {
            caretWordIndex = lines[caretLineIndex - 1].size();
            lines[caretLineIndex - 1] = lines[caretLineIndex - 1] + lines[caretLineIndex];
            lines.erase(lines.begin() + caretLineIndex);
            caretLineIndex -= 1;
            paintText();
            InvalidateRect(hwnd, nullptr, false);
            return;
        }
    }
    lines[caretLineIndex] = lines[caretLineIndex].substr(0, caretWordIndex - 1) + lines[caretLineIndex].substr(caretWordIndex);
    if (lines[caretLineIndex].empty()) {
        lines.erase(lines.begin() + caretLineIndex);
        caretLineIndex -= 1;
        if (caretLineIndex < 0) {
            caretLineIndex = 0;
            caretWordIndex = 0;
        }
        else {
            caretWordIndex = lines[caretLineIndex].length();
        }
    }
    else {
        caretWordIndex -= 1;
    }
}


void WindowInput::paintText()
{
    wordPos.clear();
    SkCanvas* canvas = surface->getCanvas();
    canvas->clear(0xFF345678);
    std::wstring currentLine;
    int lineIndex{ 0 };
    for (size_t i = 0; i < lines.size(); i++)
    {
        paintLine(lines[i], i, canvas);
    }
    if (caretLineIndex < 0) {
        caretLineIndex = wordPos.size() - 1;
    }
    if (caretWordIndex < 0) {
        caretWordIndex = wordPos[wordPos.size() - 1].size()-1;
    }
    caretVisible = true;
    flashCaret();
}

void WindowInput::flashCaret()
{
    auto color = caretVisible ? 0xFF00FFFF : 0xFF345678;
    SkPoint& p = wordPos[caretLineIndex][caretWordIndex];
    auto height{ fontBottom - fontTop };
    SkPoint start = SkPoint(p.fX, p.fY + fontTop+caretLineIndex*height);// 字符顶部相对于基线的偏移  neagtive
    SkPoint end = SkPoint(p.fX, p.fY + fontBottom + caretLineIndex * height); // 字符底部相对于基线的偏移
    SkCanvas* canvas = surface->getCanvas();
    SkPaint paint;
    paint.setColor(color);
    paint.setStroke(true);
    paint.setStrokeWidth(1);
    canvas->drawLine(start, end, paint);
    caretVisible = !caretVisible;
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

void WindowInput::onMouseDown(const int& x, const int& y)
{
}

void WindowInput::onDoubleClick(const int& x, const int& y)
{
}

void WindowInput::onMouseUp(const int& x, const int& y)
{
}

void WindowInput::onMouseDrag(const int& x, const int& y)
{
}

void WindowInput::onMouseMove(const int& x, const int& y)
{
}

void WindowInput::onMouseDownRight(const int& x, const int& y)
{
}

void WindowInput::onKeyDown(const unsigned int& val)
{
    if (val == VK_UP) {
        if (caretLineIndex <= 0) return;
        caretLineIndex -= 1;
        if (caretWordIndex >= wordPos[caretLineIndex].size() - 1) {
            caretWordIndex = wordPos[caretLineIndex].size() - 1;
        }
        paintText();
        InvalidateRect(hwnd, nullptr, false);
    }
    else if (val == VK_DOWN) {
        if (caretLineIndex >= wordPos.size()-1) return;
        caretLineIndex += 1;
        if (caretWordIndex >= wordPos[caretLineIndex].size() - 1) {
            caretWordIndex = wordPos[caretLineIndex].size() - 1;
        }
        paintText();
        InvalidateRect(hwnd, nullptr, false);
    }
    else if (val == VK_LEFT) {
        caretWordIndex -= 1;
        if (caretWordIndex < 0) {
            if (caretLineIndex <= 0) {
                caretWordIndex = 0;
                return;
            }
            caretLineIndex -= 1;
            caretWordIndex = wordPos[caretLineIndex].size()-1;
        }
        paintText();
        InvalidateRect(hwnd, nullptr, false);
    }
    else if (val == VK_RIGHT) {
        caretWordIndex += 1;
        if (caretWordIndex >= wordPos[caretLineIndex].size()) {
            if (caretLineIndex >= wordPos.size()-1) {
                caretWordIndex -= 1;
                return;
            }
            caretLineIndex += 1;
            caretWordIndex = 0;
        }
        paintText();
        InvalidateRect(hwnd, nullptr, false);
    }
}

void WindowInput::onChar(const unsigned int& val)
{
    if (val == 13) { //enter
        onEnter();
    }
    else if (val == 8) { //backspace 删除一个字
        onBackspace();
    }
    else {
        std::wstring word{ (wchar_t)val };
        //text = text+word;
        //if (lines.size() == 0) {
        //    text.push_back(word);
        //}
        //else {
        //    auto str1 = lines[lineIndex].substr(0, wordIndex);
        //    auto str2 = lines[lineIndex].substr(wordIndex);
        //    lines[lineIndex] = str1 + word + str2;
        //}
        caretWordIndex += 1;
        paintText();
        InvalidateRect(hwnd, nullptr, false);
        activeKeyboard();
    }
}

LRESULT WindowInput::routeWinMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    auto obj = reinterpret_cast<WindowInput*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
    if (!obj) {
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }
    switch(msg)
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
    else if (msg == WM_LBUTTONDOWN){
        isMouseDown = true;
        onMouseDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        return 0;
    }
    else if (msg == WM_LBUTTONDBLCLK){
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
        onChar(wParam);
        return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}
void WindowInput::activeKeyboard()
{
    if (HIMC himc = ImmGetContext(hwnd))
    {
        auto x = wordPos[caretLineIndex][caretWordIndex].fX;
        auto y = wordPos[caretLineIndex][caretWordIndex].fY;
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