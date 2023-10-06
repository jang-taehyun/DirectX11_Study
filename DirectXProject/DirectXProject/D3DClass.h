/*
D3D class

- 역할 : Direct 3D의 기능들을 control
*/

#ifndef _D3DCLASS_H_
#define _D3DCLASS_H_

/* Direct 3D의 기능들을 사용하기 위한 Library link */
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3dx10.lib")
#pragma comment(lib, "d3dcompiler.lib")

/* DirectX header include */
#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>
#include <d3dx10math.h>

class D3DClass
{
	/* member variable */
private:
	bool m_VSYNC_Enabled;
	int m_VideoCardMemory;
	char m_VideoCardDescription[128];

	IDXGISwapChain* m_Swapchain;

	ID3D11Device* m_Device;
	ID3D11DeviceContext* m_DeviceContext;

	ID3D11RenderTargetView* m_RenderTargetView;

	ID3D11Texture2D* m_DepthStencilBuffer;
	ID3D11DepthStencilState* m_DepthStencilState;
	ID3D11DepthStencilView* m_DepthStencilView;

	ID3D11RasterizerState* m_RasterizerState;

	D3DXMATRIX m_ProjectionMatrix;
	D3DXMATRIX m_WorldMatrix;
	D3DXMATRIX m_OrthoMatrix;

	/* constructor & destructor */
public:
	D3DClass();
	D3DClass(const D3DClass& ref);
	~D3DClass();

	/* interface */
public:
	bool Initialize(int ScreenWidth, int ScreenHeight, bool VSYNC, HWND hwnd, bool FullScreen, float ScreenDepth, float ScreenNear);		// Direct 3D 초기화
	void Shutdown();																														// 모든 Direct 3D 객체 할당 해제

	void BeginScene(float red, float green, float blue, float alpha);																		// 매 frame의 시작마다 buffer 내용 지우는 역할
	void EndScene();																														// 매 frame의 마지막마다 back buffer에 그린 내용을 화면에 출력

	ID3D11Device* GetDevice();																												// DirectX Device 반환
	ID3D11DeviceContext* GetDeviceContext();																								// DirectX Device Context 반환

	void GetVideoCardIndo(char* VideoCardName, int& Memory);																				// 그래픽카드 정보 반환

	void GetProjectionMatrix(D3DXMATRIX& ProjectionMatrix);																					// Projection Matrix 반환
	void GetWorldMatrix(D3DXMATRIX& WorldMatrix);																							// World Matrix 반환
	void GetOrthoMatrix(D3DXMATRIX& OrthoMatrix);																							// Ortho Matrix 반환
};

#endif