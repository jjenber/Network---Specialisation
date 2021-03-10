#include "DirectX11Framework.h"
#include "Window.h"

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")

#define CREATE_RESOURCE(_CREATE_FUNCTION) if (_CREATE_FUNCTION == false) return false;


    DirectX11Framework::DirectX11Framework() :
	    myBackBuffer(nullptr),
        myContext(nullptr),
        myDevice(nullptr),
        mySwapChain(nullptr),
        myDepthBuffer(nullptr)
    {

    }

    DirectX11Framework::~DirectX11Framework()
    {
        myBackBuffer->Release();
        myDepthBuffer->Release();
        mySwapChain->Release();
        myContext->Release();
        myDevice->Release();
    }

    bool DirectX11Framework::Init(Window& aWindow)
    {
        CREATE_RESOURCE(CreateSwapChain(aWindow));
        CREATE_RESOURCE(CreateBackBufferAndRenderTargetView());
        CREATE_RESOURCE(CreateDepthBuffer(aWindow));
        return true;
    }

    void DirectX11Framework::BeginFrame()
    {
        static const float clearColor[4] = { 0.4f, .6f, .9f, 1.f };
        myContext->ClearRenderTargetView(myBackBuffer, clearColor);
        myContext->ClearDepthStencilView(myDepthBuffer, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    }

    void DirectX11Framework::EndFrame()
    {
        mySwapChain->Present(1, 0);
    }
    
    void DirectX11Framework::UpdateResizeWindow(unsigned int aWidth, unsigned int aHeight)
    {
        myContext->OMSetRenderTargets(0, 0, 0);
        myBackBuffer->Release();
        mySwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
        CreateBackBufferAndRenderTargetView();
        myContext->OMSetRenderTargets(1, &myBackBuffer, NULL);
        CreateAndSetViewport(aWidth, aHeight);
    }

    bool DirectX11Framework::CreateSwapChain(Window& aWindow)
    {
        HRESULT result;
        DXGI_SWAP_CHAIN_DESC swapchainDesc = {};
        swapchainDesc.BufferCount = 1;
        swapchainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        swapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapchainDesc.OutputWindow = aWindow.GetHWND();
        swapchainDesc.SampleDesc.Count = 1;
        swapchainDesc.Windowed = true;

        result = D3D11CreateDeviceAndSwapChain(
            nullptr, D3D_DRIVER_TYPE_HARDWARE,
            nullptr, 0, nullptr, 0, D3D11_SDK_VERSION,
            &swapchainDesc, &mySwapChain, &myDevice, nullptr, &myContext);
    
        return true;
    }

    bool DirectX11Framework::CreateBackBufferAndRenderTargetView()
    {
        HRESULT result;
        ID3D11Texture2D* backbufferTexture;
    
        result = mySwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backbufferTexture);
        if (backbufferTexture)
        {
            result = myDevice->CreateRenderTargetView(backbufferTexture, nullptr, &myBackBuffer);
        }
        backbufferTexture->Release();

        return true;
    }

    bool DirectX11Framework::CreateDepthBuffer(Window& aWindow)
    {
        ID3D11Texture2D* depthbufferTexture = nullptr;
        D3D11_TEXTURE2D_DESC depthBufferDesc = { 0 };

        depthBufferDesc.Width = aWindow.GetWidth();
        depthBufferDesc.Height = aWindow.GetHeight();
        depthBufferDesc.ArraySize = 1;
        depthBufferDesc.Format = DXGI_FORMAT_D32_FLOAT;
        depthBufferDesc.SampleDesc.Count = 1;
        depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
  
        HRESULT result = myDevice->CreateTexture2D(&depthBufferDesc, nullptr, &depthbufferTexture);
        if (depthbufferTexture)
        {
            result = myDevice->CreateDepthStencilView(depthbufferTexture, nullptr, &myDepthBuffer);
        }

        myContext->OMSetRenderTargets(1, &myBackBuffer, myDepthBuffer);

        return true;
    }

    bool DirectX11Framework::CreateAndSetViewport(unsigned int aWidth, unsigned int aHeight)
    {
        D3D11_VIEWPORT viewport = { 0 };
        viewport.TopLeftX = 0.0f;
        viewport.TopLeftY = 0.0f;
        viewport.Width  = static_cast<float>(aWidth);
        viewport.Height = static_cast<float>(aHeight);
        viewport.MinDepth = 0.0f;
        viewport.MaxDepth = 1.0f;
        myContext->RSSetViewports(1, &viewport);
        return true;
    }
