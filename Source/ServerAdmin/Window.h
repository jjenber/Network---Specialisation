#pragma once
#include <memory>

class Window
{
public:
	static std::unique_ptr<Window> Create(
		const wchar_t* aTitle, 
		int aPositionX, int aPositionY, 
		unsigned int aWidth, unsigned int aHeight);

	HWND			GetHWND();
	int				GetPositionX() { return myPositionX; };
	int				GetPositionY() { return myPositionY; };
	unsigned int	GetWidth()     { return myWidth; };
	unsigned int	GetHeight()    { return myHeight; };

private:
	static LRESULT CALLBACK WinProc(_In_ HWND hwnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);
	
	HWND			myWindowHandle;
	unsigned int	myWidth;
	unsigned int	myHeight;
	int				myPositionX;
	int				myPositionY;
};


