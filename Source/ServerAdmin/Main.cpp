// ServerAdmin.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "ServerAdmin.h"
#include "Window.h"
#include "DirectX11Framework.h"
#include <string>
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>

static ImFont* ImGui_LoadFont(ImFontAtlas& atlas, const char* name, float size, const ImVec2& displayOffset = ImVec2(0, 0))
{
    char* windir = nullptr;
    if (_dupenv_s(&windir, nullptr, "WINDIR") || windir == nullptr)
        return nullptr;

    static const ImWchar ranges[] =
    {
        0x0020, 0x00FF, // Basic Latin + Latin Supplement
        0x0104, 0x017C, // Polish characters and more
        0,
    };

    ImFontConfig config;
    config.OversampleH = 4;
    config.OversampleV = 4;
    config.PixelSnapH = false;

    auto path = std::string(windir) + "\\Fonts\\" + name;
    auto font = atlas.AddFontFromFileTTF(path.c_str(), size, &config, ranges);

    free(windir);

    return font;
}
ImFontAtlas fontAtlas;

void OnWindowResize(DirectX11Framework& aDirectX11Framework, Window& aWindow);

#pragma region ConsoleInit
void InitConsole()
{
#pragma warning( push )
#pragma warning( disable : 4996 )
#pragma warning( disable : 6031 )
    AllocConsole();
    freopen("CONIN$", "r", stdin);
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);

    setbuf(stdin, NULL);
    setbuf(stdout, NULL);
    setbuf(stderr, NULL);

#pragma warning( pop )
}
void CloseConsole()
{
#pragma warning( push )
#pragma warning( disable : 4996 )
    fclose(stdin);
    fclose(stdout);
    fclose(stderr);
#pragma warning( pop )
}
#pragma endregion ConsoleInit

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    InitConsole();

    DirectX11Framework dx11;
    std::unique_ptr<Window> window = Window::Create(L"Server Admin", 200, 100, 800, 600);
    dx11.Init(*window);

    auto font = ImGui_LoadFont(fontAtlas, "segoeui.ttf", 18.0f);
    fontAtlas.Build();
    ImGui::CreateContext(&fontAtlas);
    ImGui_ImplWin32_Init(window->GetHWND());
    ImGui_ImplDX11_Init(dx11.GetDevice(), dx11.GetContext());
    ImGui::StyleColorsDark();

    bool run = true;
    int hits = 0;
    while (run)
    {
        MSG msg = { 0 };
        while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            if (msg.message == USER_EVENT_RESIZE)
            {
                OnWindowResize(dx11, *window);
            }
            else if (msg.message == WM_QUIT)
            {
                run = false;
            }
        }

        dx11.BeginFrame();
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        // 
        ImGui::Begin("Main Server");

        ImGui::End();


        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
        dx11.EndFrame();
    }

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    
    return 0;
}

void OnWindowResize(DirectX11Framework& aDirectX11Framework, Window& aWindow)
{
    RECT rect;
    GetWindowRect(aWindow.GetHWND(), &rect);
    aDirectX11Framework.UpdateResizeWindow(rect.right - rect.left, rect.bottom - rect.top);
}
