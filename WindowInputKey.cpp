#include "WindowInput.h"

void WindowInput::onKeyDown(const unsigned int& val)
{
    if (val == VK_UP) {
        onKeyUp();
    }
    else if (val == VK_DOWN) {
        onKeyDown();    
    }
    else if (val == VK_LEFT) {
		onKeyLeft();
    }
    else if (val == VK_RIGHT) {
		onKeyRight();
    }
    else if (val == VK_RETURN) {
        onKeyEnter();
    }
    else if (val == VK_BACK) {
        onKeyBackspace();
    }
    else if (val == VK_DELETE) {
        onKeyDelete();
	}
    else {
        bool ctrlPressed = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
        if (ctrlPressed) {
			if (val == 'C') {
                onCopy();
			}
			else if (val == 'V') {
				onPaste();
			}
			else if (val == 'X') {
                onCut();
			}
            else if (val == 'A') {
                onSelectAll();
            }
        }
    }
}
void WindowInput::onKeyLeft() {
    caretWordIndex -= 1;
    if (caretWordIndex < 0) {
        if (caretLineIndex <= 0) {
            caretWordIndex = 0;
            return;
        }
        caretLineIndex -= 1;
        caretWordIndex = wordPos[caretLineIndex].size() - 1;
    }
    activeKeyboard();
    paintText();
    InvalidateRect(hwnd, nullptr, false);
}
void WindowInput::onKeyUp() {
    if (caretLineIndex <= 0) return;
    caretLineIndex -= 1;
    if (caretWordIndex >= wordPos[caretLineIndex].size() - 1) {
        caretWordIndex = wordPos[caretLineIndex].size() - 1;
    }
    activeKeyboard();
    paintText();
    InvalidateRect(hwnd, nullptr, false);
}


void WindowInput::onCopy() {
    if (selectStartLine == -1 || selectStartWord == -1 || selectEndLine == -1 || selectEndWord == -1) {
        return;
    }
    if (selectStartLine == selectEndLine && selectStartWord == selectEndWord) {
        return;
    }
    auto [startLine, startWord, endLine, endWord] = getStartEnd();
    std::wstring str;
    for (size_t i = startLine; i <= endLine; i++)
    {
        int start{ 0 }, end = lines[i].size() - 1;
        if (i == startLine) {
            start = startWord;
        }
        if (i == endLine) {
            end = endWord;
        }
        str += lines[i].substr(start, end - start) + L"\n";
    }
    str.pop_back();
    saveToClipboard(str);
}
void WindowInput::onPaste() {
	auto clipboardStr = getClipboardText();
    if (clipboardStr.empty())
    {
        return;
    }
    auto [startLine, startWord, endLine, endWord] = getStartEnd();
    if (selectStartLine != -1 && selectEndLine != -1 && selectStartWord != -1 && selectEndWord != -1) {
        if (startLine == endLine)
        {
            lines[startLine] = lines[startLine].substr(0, startWord) + lines[startLine].substr(endWord);
        }
        else {
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
        selectStartLine = -1;
        selectStartWord = -1;
        selectEndLine = -1;
        selectEndWord = -1;
    }
    auto cLines = textToLines(clipboardStr);
    auto tempStr = lines[caretLineIndex].substr(caretWordIndex);
	if (cLines.size() == 1)
	{
        lines[caretLineIndex] = lines[caretLineIndex].substr(0, caretWordIndex) + cLines[0] + tempStr;
		caretWordIndex = caretWordIndex + cLines[0].size();
    }
    else if (cLines.size() == 2) {
        lines[caretLineIndex] = lines[caretLineIndex].substr(0, caretWordIndex) + cLines[0];
		lines.insert(lines.begin() + caretLineIndex+1, cLines[1]+ tempStr);
		caretLineIndex += 1;
        caretWordIndex = cLines[1].size();
    }
    else {
        lines[caretLineIndex] = lines[caretLineIndex].substr(0, caretWordIndex) + cLines[0];
        lines.insert(lines.begin() + caretLineIndex + 1, cLines.begin()+1,cLines.end()-1);
		lines.insert(lines.begin() + caretLineIndex + cLines.size() - 1, cLines[cLines.size() - 1] + tempStr);
        caretLineIndex += cLines.size()-1;
        caretWordIndex = cLines[1].size();
    }
    paintText();
    InvalidateRect(hwnd, nullptr, false);
    activeKeyboard();

}
void WindowInput::onSelectAll() {
    if (lines.size() == 0) {
        return;
    }
    selectStartLine = 0;
    selectStartWord = 0;
    selectEndLine = lines.size() - 1;
    selectEndWord = lines[selectEndLine].size();
    paintText();
    InvalidateRect(hwnd, nullptr, false);
    activeKeyboard();
}
void WindowInput::onCut() {
    if (selectStartLine == -1 || selectStartWord == -1 || selectEndLine == -1 || selectEndWord == -1) {
        return;
    }
    if (selectStartLine == selectEndLine && selectStartWord == selectEndWord) {
        return;
    }
    auto [startLine, startWord, endLine, endWord] = getStartEnd();
    std::wstring str;
    if (startLine == endLine) 
    {
        lines[startLine] = lines[startLine].substr(0, startWord) + lines[startLine].substr(endWord);
        str += lines[startLine].substr(startWord, endWord - startWord);
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
            str += lines[i].substr(start, end - start) + L"\n";
        }
        str.pop_back();
        for (int i = delIndexs.size() - 1; i >= 0; --i) {
            lines.erase(lines.begin() + delIndexs[i]);
        }
    }
	saveToClipboard(str);
    caretLineIndex = startLine;
    caretWordIndex = startWord;
    selectStartLine = -1;
    selectStartWord = -1; 
    selectEndLine = -1;
    selectEndWord = -1;
    paintText();
    InvalidateRect(hwnd, nullptr, false);
    activeKeyboard();
}

void WindowInput::onKeyRight() {
    caretWordIndex += 1;
    if (caretWordIndex >= wordPos[caretLineIndex].size()) {
        if (caretLineIndex >= wordPos.size() - 1) {
            caretWordIndex -= 1;
            return;
        }
        caretLineIndex += 1;
        caretWordIndex = 0;
    }
    paintText();
    InvalidateRect(hwnd, nullptr, false);
    activeKeyboard();
}
void WindowInput::onKeyDown() {
    if (caretLineIndex >= wordPos.size() - 1) return;
    caretLineIndex += 1;
    if (caretWordIndex >= wordPos[caretLineIndex].size() - 1) {
        caretWordIndex = wordPos[caretLineIndex].size() - 1;
    }
    paintText();
    InvalidateRect(hwnd, nullptr, false);
    activeKeyboard();
}

void WindowInput::onKeyEnter()
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
    activeKeyboard();
}
void WindowInput::onKeyDelete() {
    if (lines.size() == 0) {
        return;
    }
    if (caretWordIndex == lines[caretLineIndex].size()) {
        if (caretLineIndex == lines.size() - 1) {
            return;
        }
        else {
            lines[caretLineIndex] = lines[caretLineIndex] + lines[caretLineIndex + 1];
            lines.erase(lines.begin() + caretLineIndex + 1);
            paintText();
            InvalidateRect(hwnd, nullptr, false);
            activeKeyboard();
            return;
        }
    }
    lines[caretLineIndex] = lines[caretLineIndex].substr(0, caretWordIndex) + lines[caretLineIndex].substr(caretWordIndex + 1);
    if (lines[caretLineIndex].empty()) {
        lines.erase(lines.begin() + caretLineIndex);
        if (caretLineIndex >= lines.size()) {
            caretLineIndex = lines.size() - 1;
            caretWordIndex = lines[caretLineIndex].length();
        }
    }
    paintText();
    InvalidateRect(hwnd, nullptr, false);
    activeKeyboard();
}
void WindowInput::onKeyBackspace()
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
            activeKeyboard();
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
    paintText();
    InvalidateRect(hwnd, nullptr, false);
    activeKeyboard();
}

void WindowInput::onChar(const unsigned int& val)
{
    std::wstring word{ (wchar_t)val };
    if (lines.size() == 0) {
        lines.push_back(word);
    }
    else {
        auto str1 = lines[caretLineIndex].substr(0, caretWordIndex);
        auto str2 = lines[caretLineIndex].substr(caretWordIndex);
        lines[caretLineIndex] = str1 + word + str2;
    }
    caretWordIndex += 1;
    paintText();
    InvalidateRect(hwnd, nullptr, false);
    activeKeyboard();
}