#include "WindowMain.h"


void WindowMain::moveCaretLeft()
{
    if (caretX == 0 && caretY == 0) return;
    caretX -= 1;
    if (caretX < 0) {
        caretY -= 1;
        if (caretY < 0) {
            caretX = 0;
            caretY = 0;
        }
        else {
            caretX = infos[caretY].wordPos.size() - 1;
        }
    }
    refreshCaret();
}
void WindowMain::moveCaretRight()
{
    if (caretX == infos[caretY].wordPos.size() - 1 && caretY == infos.size() - 1) {
        return;
    }
    caretX += 1;
    if (caretX >= infos[caretY].wordPos.size()) {
        caretY += 1;
        if (caretY >= infos.size()) {
            caretY = infos.size() - 1;
            caretX = infos[caretY].wordPos.size() - 1;
        }
        else {
            caretX = 0;
        }
    }
    refreshCaret();
}
void WindowMain::moveCaretUp()
{
    if (caretY == 0) {
        return;
    }
    caretY -= 1;
    if (caretX >= infos[caretY].wordPos.size()) {
        caretX = infos[caretY].wordPos.size() - 1;
    }
    refreshCaret();
}
void WindowMain::moveCaretDown()
{
    if (caretY == infos.size() - 1) {
        return;
    }
    caretY += 1;
    if (caretX >= infos[caretY].wordPos.size()) {
        caretX = infos[caretY].wordPos.size() - 1;
    }
    refreshCaret();
}
void WindowMain::keyBack()
{
    if (!delSelected())
    {
        if (caretX == 0 && caretY == 0) return;
        auto charIndex = getCharIndex(caretX, caretY);
        caretX -= 1;
        if (caretX < 0) {
            caretY -= 1;
            if (caretY < 0) {
                caretX = 0;
                caretY = 0;
            }
            else {
                caretX = infos[caretY].wordPos.size() - 1;
            }
        }
        text.erase(charIndex - 1, 1);
    }
    addHistory();
    initInfo();
    InvalidateRect(hwnd, nullptr, false);
}
void WindowMain::keyDel()
{
    if (!delSelected())
    {
        if (caretX == infos[caretY].wordPos.size() - 1 && caretY == infos.size() - 1) {
            return;
        }
        auto charIndex = getCharIndex(caretX, caretY);
        text.erase(charIndex, 1);
    }
    addHistory();
    initInfo();
    InvalidateRect(hwnd, nullptr, false);
}
void WindowMain::keyEnter()
{
    delSelected();
    auto charIndex = getCharIndex(caretX, caretY);
    text.insert(charIndex, L"\n");
    caretX = 0;
    caretY += 1;
    addHistory();
    initInfo();
    InvalidateRect(hwnd, nullptr, false);
}

void WindowMain::onChar(const std::wstring& str)
{
    delSelected();
    auto charIndex = getCharIndex(caretX, caretY);
    text.insert(charIndex, str);
    caretX += str.length();
    initInfo();
    InvalidateRect(hwnd, nullptr, false);
}

void WindowMain::onKey(const uint32_t& key)
{
    if (key == VK_BACK) {
        keyBack();
    }
    else if (key == VK_RETURN) {
        keyEnter();
    }
    else if (key == VK_DELETE) {
        keyDel();
    }
    else if (key == VK_LEFT) {
        moveCaretLeft();
    }
    else if (key == VK_UP) {
        moveCaretUp();
    }
    else if (key == VK_RIGHT) {
        moveCaretRight();
    }
    else if (key == VK_DOWN) {
        moveCaretDown();
    }
    else if (key == VK_DELETE) {
        moveCaretDown();
    }
}

void WindowMain::onKeyWithCtrl(const uint32_t& key)
{
    if (key == 'Z') {
        if (text != history[historyIndex].text) {
            addHistory();
        }
        undo();
    }
    else if (key == 'Y') {
        redo();
    }
    else if (key == 'C') {
        copy();
    }
    else if (key == 'V') {
        paste();
    }
    else if (key == 'X') {
        cut();
    }
    else if (key == 'A') {
        selectAll();
    }
}

void WindowMain::copy()
{
    if (caretXStart == -1 || caretYStart == -1 || caretXEnd == -1 || caretYEnd == -1) {
        return;
    }
    if (caretXStart == caretXEnd && caretYStart == caretYEnd) {
        return;
    }
    auto [charIndex0, charIndex1] = getSelectionCharIndex();
    auto str = text.substr(charIndex0, charIndex1 - charIndex0);
    setClipboard(str);
}

void WindowMain::cut()
{
    if (caretXStart == -1 || caretYStart == -1 || caretXEnd == -1 || caretYEnd == -1) {
        return;
    }
    if (caretXStart == caretXEnd && caretYStart == caretYEnd) {
        return;
    }
    auto [charIndex0, charIndex1] = getSelectionCharIndex();
    auto str = text.substr(charIndex0, charIndex1 - charIndex0);
    setClipboard(str);
    text.erase(charIndex0, charIndex1 - charIndex0);
    caretX = caretXStart;
    caretY = caretYStart;
    caretXStart = -1;  caretYStart = -1; caretXEnd = -1;  caretYEnd = -1;
    addHistory();
    initInfo();
    InvalidateRect(hwnd, nullptr, false);
}

void WindowMain::paste()
{
    auto str = getClipboard();
    if (str.empty()) return;
    str.erase(std::remove(str.begin(), str.end(), L'\r'), str.end());
    int x{0}, y{-1};
    {
        std::wstringstream ss(str);
        std::wstring line;
        while (std::getline(ss, line))
        {
            ++y;
        }
        x += line.size();
    }
    delSelected();
    auto charIndex = getCharIndex(caretX, caretY);
    text.insert(charIndex, str);
    if (y == 0) {
        caretX += x;
    }
    else {
        caretY += y;
        caretX = x;
    }
    caretXStart = -1;  caretYStart = -1; caretXEnd = -1;  caretYEnd = -1;
    addHistory();
    initInfo();
    InvalidateRect(hwnd, nullptr, false);
}
void WindowMain::selectAll()
{
    caretXStart = 0;
    caretYStart = 0;
    caretXEnd = infos[infos.size() - 1].wordPos.size() - 1;
    caretYEnd = infos.size() - 1;
    InvalidateRect(hwnd, nullptr, false);
}

void WindowMain::undo()
{
    if (history.size() <= 1 || historyIndex >= history.size()-1) return;
    historyIndex += 1;
    auto& his = history[historyIndex];
    text = his.text;
    caretX = his.caretX;
    caretY = his.caretY;
    caretXStart = -1;  caretYStart = -1; caretXEnd = -1;  caretYEnd = -1;
    initInfo();
    InvalidateRect(hwnd, nullptr, false);
}

void WindowMain::redo()
{
    if (historyIndex <= 0) return;
    historyIndex -= 1;
    auto& his = history[historyIndex];
    text = his.text;
    caretX = his.caretX;
    caretY = his.caretY;
    caretXStart = -1;  caretYStart = -1; caretXEnd = -1;  caretYEnd = -1;
    initInfo();
    InvalidateRect(hwnd, nullptr, false);
}
