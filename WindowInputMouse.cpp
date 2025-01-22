#include "WindowInput.h"

void WindowInput::onMouseDown(const int& x, const int& y)
{
	auto lwIndex = setMouseAtIndex(x,y);
	caretLineIndex = lwIndex.first;
	caretWordIndex = lwIndex.second;
	selectStartLine = caretLineIndex;
	selectStartWord = caretWordIndex;
	paintText();
	InvalidateRect(hwnd, nullptr, false);
	activeKeyboard();
}

void WindowInput::onDoubleClick(const int& x, const int& y)
{

}

void WindowInput::onMouseUp(const int& x, const int& y)
{
}

void WindowInput::onMouseDrag(const int& x, const int& y)
{
	auto lwIndex = setMouseAtIndex(x, y);
	if (lwIndex.first == selectStartLine && lwIndex.second == selectStartWord) {
		return;
	}
	selectEndLine = lwIndex.first;
	selectEndWord = lwIndex.second;
	paintText();
	InvalidateRect(hwnd, nullptr, false);
	activeKeyboard();
}

void WindowInput::onMouseMove(const int& x, const int& y)
{
}

void WindowInput::onMouseDownRight(const int& x, const int& y)
{
}

std::pair<float, float>  WindowInput::setMouseAtIndex(const int& x, const int& y) {
	std::pair<float, float> result;
	auto height{ fontBottom - fontTop };
	result.first = y / height;
	if (result.first > lines.size() - 1) {
		result.first = lines.size() - 1;
	}
	int wordIndex = -1;
	for (size_t i = 0; i < wordPos[result.first].size(); i++)
	{
		auto wordX = wordPos[result.first][i].fX;
		if (x < wordX) {
			if (i == 0) {
				wordIndex = 0;
				break;
			}
			else {
				if (x < wordX - (wordX - wordPos[result.first][i - 1].fX) / 2) {
					wordIndex = i - 1;
				}
				else {
					wordIndex = i;
				}
				break;
			}
		}
	}
	if (wordIndex < 0) {
		result.second = wordPos[result.first].size() - 1;
	}
	else {
		result.second = wordIndex;
	}
	return result;
}