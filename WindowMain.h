#pragma once
#include <include/core/SkFont.h>
#include "WinBase.h"

class WindowMain:public WinBase
{
public:
	WindowMain();
	~WindowMain();
protected:
	void onShown() override;
	void onSize() override;
private:
	void initText();
	void initFont();
	void initPosSize();
	LRESULT CALLBACK procMsg(UINT msg, WPARAM wParam, LPARAM lParam) override;

	void paintText();
	void flashCaret();
	void paintSelectedBg(SkCanvas* canvas);
	void paintLine(const std::wstring& text, const int& lineIndex, SkCanvas* canvas);

	void onMouseDown(const int& x, const int& y);
	void onDoubleClick(const int& x, const int& y);
	void onMouseUp(const int& x, const int& y);
	void onMouseDrag(const int& x, const int& y);
	void onMouseMove(const int& x, const int& y);
	void onMouseDownRight(const int& x, const int& y);
	std::pair<float, float> setMouseAtIndex(const int& x, const int& y);

	void onKeyDown(const unsigned int& val);
	void onKeyEnter();
	void onKeyBackspace();
	void onKeyDelete();
	void onKeyLeft();
	void onKeyUp();
	void onKeyRight();
	void onKeyDown();
	void onCopy();
	void onPaste();
	void onCut();
	void onSelectAll();
	void onChar(const unsigned int& val);

	bool enableAlpha();
	void activeKeyboard();
	bool hasSelection();
	void deleteSelection();
	void cancelSelection();
	std::wstring getClipboardText();
	std::array<int, 4> getSelectionArea();
	void saveToClipboard(const std::wstring& str);
	std::vector<std::wstring> textToLines(const std::wstring& text);
private:
	SkFont font;
	float fontTop, fontBottom, fontAsent, fontDesent;//fontAsent从基线到字体中最高字符顶部的距离,fTop 是从基线到字体中理论上最高点的距离。
	std::vector<std::wstring> lines;
	std::map<int, std::vector<SkPoint>> wordPos;

	bool caretVisible{ true };
	int caretLineIndex{ -1 }, caretWordIndex{ -1 };
	int selectStartLine{ -1 }, selectStartWord{ -1 }, selectEndLine{ -1 }, selectEndWord{ -1 };
	float fontSize{ 26 };
	bool isMouseDown{ false };
	SkColor colorBg{ 0XFFFFFFFF }, colorFore{ 0XFF000000 };
};