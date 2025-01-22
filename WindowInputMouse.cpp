#include "WindowInput.h"

void WindowInput::onMouseDown(const int& x, const int& y)
{
	auto height{ fontBottom - fontTop };
	caretLineIndex = y / height;
	if (caretLineIndex > lines.size() - 1) {
		caretLineIndex = lines.size() - 1;
	}
	int wordIndex = -1;
	for (size_t i = 0; i < wordPos[caretLineIndex].size(); i++)
	{
		auto wordX = wordPos[caretLineIndex][i].fX;
		if (x < wordX) {
			if (i == 0) {
				wordIndex = 0;
				break;
			}
			else {
				if (x < wordX - (wordX - wordPos[caretLineIndex][i - 1].fX)/2) {
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
		caretWordIndex = wordPos[caretLineIndex].size() - 1;
	}
	else {
		caretWordIndex = wordIndex;
	}
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
}

void WindowInput::onMouseMove(const int& x, const int& y)
{
}

void WindowInput::onMouseDownRight(const int& x, const int& y)
{
}
