/*
D3D class

- ���� : Direct 3D�� ��ɵ��� control
*/

#ifndef _D3DCLASS_H_
#define _D3DCLASS_H_

/* Direct 3D�� ��ɵ��� ����ϱ� ���� Library link */
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
	bool Initialize(int ScreenWidth, int ScreenHeight, bool VSYNC, HWND hwnd, bool FullScreen, float ScreenDepth, float ScreenNear);		// Direct 3D �ʱ�ȭ
	void Shutdown();																														// ��� Direct 3D ��ü �Ҵ� ����

	void BeginScene(float red, float green, float blue, float alpha);																		// �� frame�� ���۸��� buffer ���� ����� ����
	void EndScene();																														// �� frame�� ���������� back buffer�� �׸� ������ ȭ�鿡 ���

	ID3D11Device* GetDevice();																												// DirectX Device ��ȯ
	ID3D11DeviceContext* GetDeviceContext();																								// DirectX Device Context ��ȯ

	void GetVideoCardIndo(char* VideoCardName, int& Memory);																				// �׷���ī�� ���� ��ȯ

	void GetProjectionMatrix(D3DXMATRIX& ProjectionMatrix);																					// Projection Matrix ��ȯ
	void GetWorldMatrix(D3DXMATRIX& WorldMatrix);																							// World Matrix ��ȯ
	void GetOrthoMatrix(D3DXMATRIX& OrthoMatrix);																							// Ortho Matrix ��ȯ
};

#endif