#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>

class Window;
class DirectX11Framework
{
public:
	DirectX11Framework();
	~DirectX11Framework();

	bool Init(Window& aWindow);
	void BeginFrame();
	void EndFrame();

	ID3D11Device* GetDevice() { return myDevice; }
	ID3D11DeviceContext* GetContext() { return myContext; }

private:
	bool CreateSwapChain(Window& aWindow);
	bool CreateBackBufferAndRenderTargetView();
	bool CreateDepthBuffer(Window& aWindow);
	bool CreateAndSetViewport(Window& aWindow);

	IDXGISwapChain* mySwapChain;
	ID3D11Device* myDevice;
	ID3D11DeviceContext* myContext;
	ID3D11RenderTargetView* myBackBuffer;
	ID3D11DepthStencilView* myDepthBuffer;
};
