#include "WindowInput.h"


std::array<int, 4> WindowInput::getStartEnd() {
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
void WindowInput::saveToClipboard(const std::wstring& str) {
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