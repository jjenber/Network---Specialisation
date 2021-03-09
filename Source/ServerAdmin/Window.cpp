#include <Windows.h>
#include "Window.h"

std::unique_ptr<Window> Window::Create(
    const wchar_t* aTitle, 
    int aPositionX, int aPositionY, 
    unsigned int aWidth, unsigned int aHeight)
{
    Window window = Window();
    const wchar_t* className = L"Server Admin";

    WNDCLASSW windowClass = {};
    windowClass.style = CS_VREDRAW | CS_HREDRAW | CS_OWNDC;
    windowClass.lpfnWndProc = Window::WinProc;
    windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
    windowClass.lpszClassName = className;
    RegisterClassW(&windowClass);

    CreateWindowW(className, aTitle,
        WS_OVERLAPPEDWINDOW | WS_POPUP | WS_VISIBLE,
        aPositionX, aPositionY, aWidth, aHeight,
        nullptr, nullptr, nullptr, &window);

    window.myWidth  = aWidth;
    window.myHeight = aHeight;
    window.myPositionX = aPositionX;
    window.myPositionY = aPositionY;

    return std::make_unique<Window>(std::move(window));
}

HWND Window::GetHWND()
{
    return myWindowHandle;
}

LRESULT Window::WinProc(_In_ HWND hwnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
    static Window* window = nullptr;

    switch (uMsg)
    {
    case WM_CLOSE:
        // TODO: Prompt user
        // else 
        DestroyWindow(hwnd);
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    case WM_CREATE:
        CREATESTRUCT* createStruct = reinterpret_cast<CREATESTRUCT*>(lParam);
        window = reinterpret_cast<Window*>(createStruct->lpCreateParams);
        window->myWindowHandle = hwnd;
        break;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
