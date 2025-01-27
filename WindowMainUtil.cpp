#include <dwmapi.h>
#include <versionhelpers.h>
#include "WindowMain.h"


std::array<int, 4> WindowMain::getSelectionArea() {
    int startLine{ selectStartLine }, 
        startWord{ selectStartWord },
        endLine{ selectEndLine }, 
        endWord{ selectEndWord };
    if (selectEndLine < selectStartLine) {
        endLine = selectStartLine;
        endWord = selectStartWord;
        startLine = selectEndLine;
        startWord = selectEndWord;
    }
    else if (startLine == selectStartLine && startWord < selectStartWord) {
        endLine = selectStartLine;
        endWord = selectStartWord;
        startLine = selectEndLine;
        startWord = selectEndWord;
    }
	return { startLine,startWord,endLine,endWord };
}
void WindowMain::saveToClipboard(const std::wstring& str) {
    if (OpenClipboard(hwnd)) {
        EmptyClipboard();
        HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, (str.size() + 1) * sizeof(wchar_t));
        if (hMem) {
            wchar_t* pMem = (wchar_t*)GlobalLock(hMem);
            if (pMem) {
                wcscpy_s(pMem, str.size() + 1, str.c_str());
                GlobalUnlock(hMem);
                SetClipboardData(CF_UNICODETEXT, hMem);
            }
        }
        CloseClipboard();
    }
}
std::wstring WindowMain::getClipboardText() {
    if (!OpenClipboard(nullptr)) {
        return L"";
    }
    std::wstring clipboardText;
    if (IsClipboardFormatAvailable(CF_UNICODETEXT)) {
        HANDLE hData = GetClipboardData(CF_UNICODETEXT);
        if (hData != nullptr) {
            wchar_t* pszText = static_cast<wchar_t*>(GlobalLock(hData));
            if (pszText != nullptr) {
                clipboardText = pszText;
                GlobalUnlock(hData);    
            }
        }
    }
    CloseClipboard();
    return clipboardText;
}


std::vector<std::wstring> WindowMain::textToLines(const std::wstring& text) {

	std::vector<std::wstring> result;
    std::wstring currentLine;
    for (size_t i = 0; i < text.size(); ++i) {
        if (text[i] == L'\n' || (text[i] == L'\r' && (i + 1 < text.size() && text[i + 1] == L'\n'))) {
            // 如果遇到 '\n' 或者 '\r\n'（Windows换行），调用特定方法
            result.push_back(currentLine);
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
        result.push_back(currentLine);
    }
	return result;
}

bool WindowMain::enableAlpha()
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
bool WindowMain::hasSelection() {
    if (lines.size() == 0) {
        return false;
    }
	return selectStartLine != -1 && selectStartWord != -1 && selectEndLine != -1 && selectEndWord != -1;
}

void WindowMain::deleteSelection() {
    auto [startLine, startWord, endLine, endWord] = getSelectionArea();
    if (startLine == endLine)
    {
        lines[startLine] = lines[startLine].substr(0, startWord) + lines[startLine].substr(endWord);
    }
    else
    {
        std::vector<int> delIndexs;
        for (size_t i = startLine; i <= endLine; i++)
        {
            int start{ 0 }, end = lines[i].size() - 1;
            if (i == startLine) {
                start = startWord;
                lines[i] = lines[i].substr(0, start);
            }
            if (i == endLine) {
                end = endWord;
                lines[startLine] = lines[startLine] + lines[i].substr(end);
            }
            if (start == 0)
            {
                delIndexs.push_back(i);
            }
        }
        for (int i = delIndexs.size() - 1; i >= 0; --i) {
            lines.erase(lines.begin() + delIndexs[i]);
        }
    }
    caretLineIndex = startLine;
    caretWordIndex = startWord;
    cancelSelection();
    paintText();
    InvalidateRect(hwnd, nullptr, false);
    activeKeyboard();
}
void  WindowMain::cancelSelection() {
    selectStartLine = -1;
    selectStartWord = -1;
    selectEndLine = -1;
    selectEndWord = -1;
}
void WindowMain::activeKeyboard()
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
        //CANDIDATEFORM cand = {};
        //cand.dwStyle = CFS_CANDIDATEPOS;
        //cand.ptCurrentPos.x = x;
        //cand.ptCurrentPos.y = y;
        //ImmSetCandidateWindow(himc, &cand);
        ImmReleaseContext(hwnd, himc);
    }
}