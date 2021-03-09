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

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    std::unique_ptr<Window> window = Window::Create(L"Server Admin", 0, 0, 800, 600);
    DirectX11Framework dx11;
    dx11.Init(*window);

    auto font = ImGui_LoadFont(fontAtlas, "segoeui.ttf", 18.0f);
    fontAtlas.Build();
    ImGui::CreateContext(&fontAtlas);
    ImGui_ImplWin32_Init(window->GetHWND());
    ImGui_ImplDX11_Init(dx11.GetDevice(), dx11.GetContext());
    ImGui::StyleColorsDark();

    bool run = true;
    while (run)
    {
        MSG msg = { 0 };
        while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            if (ImGui_ImplWin32_WndProcHandler(msg.hwnd, msg.message, msg.wParam, msg.lParam))
            {
                continue;
            }
            if (msg.message == WM_QUIT)
            {
                run = false;
            }
        }

        dx11.BeginFrame();
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        // 
        ImGui::Begin("Editor");

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
