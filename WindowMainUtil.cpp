#include <dwmapi.h>
#include <versionhelpers.h>
#include "WindowMain.h"

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



std::string WindowMain::convertToUTF8(const std::wstring& wstr)
{
    const int count = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.length(), NULL, 0, NULL, NULL);
    std::string str(count, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &str[0], count, NULL, NULL);
    return str;
}

std::string WindowMain::convertToUTF8(const LPWSTR& wstr)
{
    const int count = WideCharToMultiByte(CP_UTF8, 0, wstr, (int)wcslen(wstr), NULL, 0, NULL, NULL);
    std::string str(count, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr, -1, &str[0], count, NULL, NULL);
    return str;
}