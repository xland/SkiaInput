#include "WindowMain.h"



void WindowMain::onMouseDBClick(const int& x, const int& y)
{

}



void WindowMain::onMouseMove(const int& x, const int& y)
{
}

void WindowMain::onMousePressRight(const int& x, const int& y)
{
}

bool WindowMain::setClipboard(const std::wstring& text)
{
    if (!OpenClipboard(nullptr))
    {
        return false;
    }
    EmptyClipboard();
    size_t size = (text.length() + 1) * sizeof(wchar_t);
    HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, size);
    if (hGlobal == nullptr)
    {
        CloseClipboard();
        return false;
    }
    wchar_t* pData = static_cast<wchar_t*>(GlobalLock(hGlobal));
    if (pData == nullptr)
    {
        GlobalFree(hGlobal);
        CloseClipboard();
        return false;
    }
    wcscpy_s(pData, text.length() + 1, text.c_str());
    GlobalUnlock(hGlobal);
    if (SetClipboardData(CF_UNICODETEXT, hGlobal) == nullptr)
    {
        GlobalFree(hGlobal);
        CloseClipboard();
        return false;
    }
    CloseClipboard();
    return true;
}

std::wstring WindowMain::getClipboard()
{
    if (!OpenClipboard(nullptr))
    {
        return L"";
    }
    if (!IsClipboardFormatAvailable(CF_UNICODETEXT))
    {
        CloseClipboard();
        return L"";
    }
    HGLOBAL hGlobal = GetClipboardData(CF_UNICODETEXT);
    if (hGlobal == nullptr)
    {
        CloseClipboard();
        return L"";
    }
    std::wstring result{ static_cast<wchar_t*>(GlobalLock(hGlobal)) };    
    GlobalUnlock(hGlobal);
    CloseClipboard();
	return result;
}


