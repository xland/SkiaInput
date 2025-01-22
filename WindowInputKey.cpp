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
    activeKeyboard();
    paintText();
    InvalidateRect(hwnd, nullptr, false);
}
void WindowInput::onKeyDown() {
    if (caretLineIndex >= wordPos.size() - 1) return;
    caretLineIndex += 1;
    if (caretWordIndex >= wordPos[caretLineIndex].size() - 1) {
        caretWordIndex = wordPos[caretLineIndex].size() - 1;
    }
    activeKeyboard();
    paintText();
    InvalidateRect(hwnd, nullptr, false);
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
    activeKeyboard();
    paintText();
    InvalidateRect(hwnd, nullptr, false);
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
            activeKeyboard();
            paintText();
            InvalidateRect(hwnd, nullptr, false);
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
    activeKeyboard();
    paintText();
    InvalidateRect(hwnd, nullptr, false);
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
            activeKeyboard();
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
    activeKeyboard();
    paintText();
    InvalidateRect(hwnd, nullptr, false);
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