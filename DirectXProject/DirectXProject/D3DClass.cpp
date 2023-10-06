#include "D3DClass.h"

D3DClass::D3DClass() : 
	m_VSYNC_Enabled(false),
	m_VideoCardMemory(0),
	m_Swapchain(nullptr),
	m_Device(nullptr),
	m_DeviceContext(nullptr),
	m_RenderTargetView(nullptr),
	m_DepthStencilBuffer(nullptr),
	m_DepthStencilState(nullptr),
	m_DepthStencilView(nullptr),
	m_RasterizerState(nullptr)
{
}

D3DClass::D3DClass(const D3DClass& ref) :
	m_VSYNC_Enabled(false),
	m_VideoCardMemory(0),
	m_Swapchain(nullptr),
	m_Device(nullptr),
	m_DeviceContext(nullptr),
	m_RenderTargetView(nullptr),
	m_DepthStencilBuffer(nullptr),
	m_DepthStencilState(nullptr),
	m_DepthStencilView(nullptr),
	m_RasterizerState(nullptr)
{
}

D3DClass::~D3DClass()
{
}

/* Prameter : ������ �ػ�, ��������ȭ ����, ������ �ڵ�, Ǯ��ũ�� ����, 3D ȯ���� Depth ��*/
bool D3DClass::Initialize(int ScreenWidth, int ScreenHeight, bool VSYNC, HWND hwnd, bool FullScreen, float ScreenDepth, float ScreenNear)
{
	/*
	�ʱ�ȭ ����
	1. �׷���ī�� ������ �޾ƿ´�.
	2. DirectX�� �ʱ�ȭ �Ѵ�.
	   1) Swap chain ������ ä���, feature level�� �����Ѵ�.
	   2) Swap chain, Direct3D Device, Direct3D Device Context�� ������ ����, Render Target View�� �����.
	   3) Depth Stencil View�� �����Ѵ�.
	      ���� : Depth ������ ä��� -> Depth buffer ���� -> Depth Stencil ������ ä��� -> Depth Stencil state ���� �� ����
				 -> Depth Stencil view�� ������ ä��� ����
	   4) Render Target View�� Depth Stencil View�� Rendering Pipeline�� ���ε�
	   5) Rasterization state ���� �� Rendering Pipeline�� ���ε�
	   6) Viewport ����
	   7) Matrix ���� �� ����(World, Projection, View, Ortho)
	*/
	
	HRESULT result;

	/* �׷���ī�� ������ ��� ���� �ʿ��� variable */
	IDXGIFactory* Factory = nullptr;													// �׷���ī�� ������ ��� ���� interface
	IDXGIAdapter* Adapter = nullptr;													// �׷���ī��
	IDXGIOutput* AdapterOutput = nullptr;												// output(�����)
	unsigned int DisplayModeNumbers = 0, i = 0, Numerator = 0, Denominator = 0, StringLength = 0;
	DXGI_MODE_DESC* DisplayModeList = nullptr;											// ���÷��� ��� ����Ʈ
	DXGI_ADAPTER_DESC AdapterDescription;												// �׷���ī�� Description
	int error;

	DXGI_SWAP_CHAIN_DESC SwapchainDescription;
	D3D_FEATURE_LEVEL FeatureLevel;
	ID3D11Texture2D* BackBufferPointer = nullptr;
	D3D11_TEXTURE2D_DESC DepthBufferDescription;
	D3D11_DEPTH_STENCIL_DESC DepthStencilDescription;
	D3D11_DEPTH_STENCIL_VIEW_DESC DepthStencilViewDescription;
	D3D11_RASTERIZER_DESC RasterizationDescription;
	D3D11_VIEWPORT Viewport;
	float FieldOfView = 0.f, ScreenAspect = 0.f;

	/* ��������ȭ ���� */
	m_VSYNC_Enabled = VSYNC;

	/* -------------------- �׷���ī�� ���� ��� ���� -------------------- */
	/* DirectX �׷���ī�� �������̽� Factory ����(�׷���ī�� pointer �޾ƿ���) */
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&Factory);
	if (FAILED(result))
	{
		return false;
	}

	/* ���丮 ��ü�� �̿��� ù��° �׷��� ī�� �������̽��� ���� Adapter ���� */
	result = Factory->EnumAdapters(0, &Adapter);
	if (FAILED(result))
	{
		return false;
	}

	/* ù ��° Adapter�� ����� output(�����) ���� */
	result = Adapter->EnumOutputs(0, &AdapterOutput);
	if (FAILED(result))
	{
		return false;
	}

	/* DSGI_FORMAT_R8G8B8A8_UNORM ����� ��� ���÷��� ���˿� �´� ���÷��� ����� ���� ���ϱ� */
	result = AdapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &DisplayModeNumbers, NULL);
	if (FAILED(result))
	{
		return false;
	}

	/* ������ ��� ����Ϳ� �׷���ī�� ������ ������ ����Ʈ(���÷��� ��� ����Ʈ) ���� */
	DisplayModeList = new DXGI_MODE_DESC[DisplayModeNumbers];
	if (!DisplayModeList)
	{
		return false;
	}

	/* ���÷��� ��� ����Ʈ ä�� �ֱ� */
	result = AdapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &DisplayModeNumbers, DisplayModeList);

	/* ��� ���÷��� ��� �� ȭ�� �ʺ�/���̿� �´� ���÷��� ��带 ã��,
	   ������ ���� ã���� ���ΰ�ħ ������ �и�� ������ ���� ���� */
	for (i = 0; i < DisplayModeNumbers; i++)
	{
		if (DisplayModeList[i].Width == (unsigned int)ScreenWidth &&
			DisplayModeList[i].Height == (unsigned int)ScreenHeight)
		{
			Numerator = DisplayModeList[i].RefreshRate.Numerator;
			Denominator = DisplayModeList[i].RefreshRate.Denominator;
		}
	}

	/* Adapter(�׷���ī��)�� Description �޾ƿ��� */
	result = Adapter->GetDesc(&AdapterDescription);
	if (!DisplayModeList)
	{
		return false;
	}

	/* ���� �׷���ī���� �޸� �뷮�� MB ������ ���� */
	m_VideoCardMemory = (int)(AdapterDescription.DedicatedVideoMemory / 1024 / 1024);

	/* ���� �׷���ī���� �̸��� char�� ���ڿ� �迭�� �ٲ� �� ���� */
	error = wcstombs_s(&StringLength, m_VideoCardDescription, 128, AdapterDescription.Description, 128);
	if (error)
	{
		return false;
	}

	/* ���÷��� ��� ����Ʈ, AdapterOutput ��ü, Adapter ��ü, Factory ��ü ���� */
	delete[] DisplayModeList;
	DisplayModeList = nullptr;

	AdapterOutput->Release();
	AdapterOutput = nullptr;

	Adapter->Release();
	Adapter = nullptr;

	Factory->Release();
	Factory = nullptr;
	/* ---------------------------------------- */

	/* -------------------- DirectX �ʱ�ȭ -------------------- */
	/* ---------- Swap Chain Description �ۼ� �� Feature Level ���� ---------- */
	/* Swap Chain�� Description �ʱ�ȭ */
	ZeroMemory(&SwapchainDescription, sizeof(SwapchainDescription));

	/* back buffer ���� ���� */
	SwapchainDescription.BufferCount = 1;

	/* back buffer ���� */
	SwapchainDescription.BufferDesc.Width = ScreenWidth;						// ȭ�� �ʺ�/����
	SwapchainDescription.BufferDesc.Height = ScreenHeight;
	SwapchainDescription.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;		// ����(�Ϲ����� 32bit�� surface�� ����)
	SwapchainDescription.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;			// �뵵

	/* Swap Chain �ֻ��� ����(ȭ�� �ֻ���) */
	if (m_VSYNC_Enabled)
	{
		SwapchainDescription.BufferDesc.RefreshRate.Numerator = Numerator;
		SwapchainDescription.BufferDesc.RefreshRate.Denominator = Denominator;
	}
	else
	{
		SwapchainDescription.BufferDesc.RefreshRate.Numerator = 0;
		SwapchainDescription.BufferDesc.RefreshRate.Denominator = 1;
	}

	/* Rendering�� ������ ���� */
	SwapchainDescription.OutputWindow = hwnd;

	/* ��Ƽ���ø� ����(����� off) */
	SwapchainDescription.SampleDesc.Count = 1;
	SwapchainDescription.SampleDesc.Quality = 0;

	/* Ǯ��ũ�� ��� �Ǵ� ������ ��� ���� */
	if (FullScreen)
	{
		SwapchainDescription.Windowed = false;
	}
	else
	{
		SwapchainDescription.Windowed = true;
	}

	/* ��ĵ���� ����, ��ĵ���̴��� �������� ����(unspecified)�� ���� */
	SwapchainDescription.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	SwapchainDescription.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	/* Swap Chain ���� back buffer�� ������ ���쵵�� ���� */
	SwapchainDescription.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	/* �߰� �ɼ� �÷��� ���� */
	SwapchainDescription.Flags = 0;												// �ƹ� �͵� �������� ����

	/* Feature Level(DirectX ����) ���� */
	FeatureLevel = D3D_FEATURE_LEVEL_11_0;
	/* -------------------- */

	/* ---------- Swap chain, Direct3D Device, Direct3D Device Context�� �����ϰ�, Render Target View�� �����. ---------- */
	/* Swap chain, Direct3D Device, Direct3D Device Context ���� */
	result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &FeatureLevel, 1,
		D3D11_SDK_VERSION, &SwapchainDescription, &m_Swapchain, &m_Device, NULL, &m_DeviceContext);
	if (FAILED(result))
	{
		return false;
	}

	/* back buffer�� �����͸� ��������, back buffer�� �����͸� �̿��� Render Target View ���� */
	result = m_Swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&BackBufferPointer);
	if (FAILED(result))
	{
		return false;
	}
	result = m_Device->CreateRenderTargetView(BackBufferPointer, NULL, &m_RenderTargetView);
	if (FAILED(result))
	{
		return false;
	}

	/* back buffer ������ �Ҵ� ���� */
	BackBufferPointer->Release();
	BackBufferPointer = nullptr;
	/* -------------------- */

	/* ---------- Depth Stencil View�� �����Ѵ�. ---------- */
	/* Depth buffer Description �ʱ�ȭ �� �ۼ� */
	ZeroMemory(&DepthBufferDescription, sizeof(DepthBufferDescription));

	DepthBufferDescription.Width = ScreenWidth;
	DepthBufferDescription.Height = ScreenHeight;
	DepthBufferDescription.MipLevels = 1;
	DepthBufferDescription.ArraySize = 1;
	DepthBufferDescription.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	DepthBufferDescription.SampleDesc.Count = 1;
	DepthBufferDescription.SampleDesc.Quality = 0;
	DepthBufferDescription.Usage = D3D11_USAGE_DEFAULT;
	DepthBufferDescription.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	DepthBufferDescription.CPUAccessFlags = 0;
	DepthBufferDescription.MiscFlags = 0;

	/* Depth Stencil buffer ���� */
	result = m_Device->CreateTexture2D(&DepthBufferDescription, NULL, &m_DepthStencilBuffer);
	if (FAILED(result))
	{
		return false;
	}

	/* Depth Stencil Description �ʱ�ȭ �� �ۼ� */
	ZeroMemory(&DepthStencilDescription, sizeof(DepthStencilDescription));

	DepthStencilDescription.DepthEnable = true;
	DepthStencilDescription.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	DepthStencilDescription.DepthFunc = D3D11_COMPARISON_LESS;
	DepthStencilDescription.StencilEnable = true;
	DepthStencilDescription.StencilReadMask = 0xFF;
	DepthStencilDescription.StencilWriteMask = 0xFF;

	// pixel�� front-facing���� �� ���, Stencil operations
	DepthStencilDescription.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	DepthStencilDescription.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	DepthStencilDescription.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	DepthStencilDescription.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// pixel�� back-facing���� �� ���, Stencil operations
	DepthStencilDescription.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	DepthStencilDescription.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	DepthStencilDescription.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	DepthStencilDescription.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	/* Depth Stencil state ���� */
	result = m_Device->CreateDepthStencilState(&DepthStencilDescription, &m_DepthStencilState);
	if (FAILED(result))
	{
		return false;
	}

	/* Depth Stencil state ���� */
	m_DeviceContext->OMSetDepthStencilState(m_DepthStencilState, 1);

	/* Depth Stencil view�� Description �ʱ�ȭ �� �ۼ� */
	ZeroMemory(&DepthStencilViewDescription, sizeof(DepthStencilViewDescription));

	DepthStencilViewDescription.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	DepthStencilViewDescription.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	DepthStencilViewDescription.Texture2D.MipSlice = 0;

	/* Depth Stencil view ���� */
	result = m_Device->CreateDepthStencilView(m_DepthStencilBuffer, &DepthStencilViewDescription, &m_DepthStencilView);
	if (FAILED(result))
	{
		return false;
	}
	/* -------------------- */

	/* ---------- Render Target View�� Depth Stencil View�� Rendering Pipeline�� ���ε� ---------- */
	m_DeviceContext->OMSetRenderTargets(1, &m_RenderTargetView, m_DepthStencilView);

	/* ---------- Rasterization state ���� �� Rendering Pipeline�� ���ε� ---------- */
	RasterizationDescription.AntialiasedLineEnable = false;
	RasterizationDescription.CullMode = D3D11_CULL_BACK;
	RasterizationDescription.DepthBias = 0;
	RasterizationDescription.DepthBiasClamp = 0.f;
	RasterizationDescription.DepthClipEnable = true;
	RasterizationDescription.FillMode = D3D11_FILL_SOLID;
	RasterizationDescription.FrontCounterClockwise = false;
	RasterizationDescription.MultisampleEnable = false;
	RasterizationDescription.ScissorEnable = false;
	RasterizationDescription.SlopeScaledDepthBias = 0.f;

	result = m_Device->CreateRasterizerState(&RasterizationDescription, &m_RasterizerState);
	if (FAILED(result))
	{
		return false;
	}

	m_DeviceContext->RSSetState(m_RasterizerState);
	
	/* ---------- Viewport ���� ---------- */
	Viewport.Width = (float)ScreenWidth;
	Viewport.Height = (float)ScreenHeight;
	Viewport.MinDepth = 0.f;
	Viewport.MaxDepth = 1.f;
	Viewport.TopLeftX = 0.f;
	Viewport.TopLeftY = 0.f;

	m_DeviceContext->RSSetViewports(1, &Viewport);

	/* ---------- Matrix ���� �� ����(World, Projection, View, Ortho) ---------- */
	// Projection Matrix ���� �� ����
	FieldOfView = (float)D3DX_PI / 4.f;
	ScreenAspect = (float)ScreenWidth / (float)ScreenHeight;

	D3DXMatrixPerspectiveFovLH(&m_ProjectionMatrix, FieldOfView, ScreenAspect, ScreenNear, ScreenDepth);

	// World Matrix�� ���� ��ķ� �ʱ�ȭ
	D3DXMatrixIdentity(&m_WorldMatrix);

	// View Matrix(Camera Matrix) ���� -> Camera Class���� �ٷ� ����

	// Ortho Matrix ����(UI���� 2D ��Ҹ� Rendering�� �� ����ϴ� Matrix)
	D3DXMatrixOrthoLH(&m_OrthoMatrix, (float)ScreenWidth, (float)ScreenHeight, ScreenNear, ScreenDepth);
	/* -------------------- */
	/* ---------------------------------------- */

	return true;
}

void D3DClass::Shutdown()
{
	/* �����ϱ� ���� ������ ���� �ٲٱ� */
	/* -> Full Screen ��忡�� �Ҵ� �����ϴ� ���, ��� ���ܰ� �߻��ϱ� ������, ������ ���� �ٲٰ� ��� Direct 3D ��ü�� �����Ѵ�. */
	if (m_Swapchain)
	{
		m_Swapchain->SetFullscreenState(false, NULL);
	}

	/* ��� Direct 3D ��ü ���� */
	if (m_RasterizerState)
	{
		m_RasterizerState->Release();
		m_RasterizerState = nullptr;
	}

	if (m_DepthStencilView)
	{
		m_DepthStencilView->Release();
		m_DepthStencilView = nullptr;
	}

	if (m_DepthStencilState)
	{
		m_DepthStencilState->Release();
		m_DepthStencilState = nullptr;
	}

	if (m_DepthStencilBuffer)
	{
		m_DepthStencilBuffer->Release();
		m_DepthStencilBuffer = nullptr;
	}

	if (m_RenderTargetView)
	{
		m_RenderTargetView->Release();
		m_RenderTargetView = nullptr;
	}

	if (m_DeviceContext)
	{
		m_DeviceContext->Release();
		m_DeviceContext = nullptr;
	}

	if (m_Device)
	{
		m_Device->Release();
		m_Device = nullptr;
	}

	if (m_Swapchain)
	{
		m_Swapchain->Release();
		m_Swapchain = nullptr;
	}
}

void D3DClass::BeginScene(float red, float green, float blue, float alpha)
{
	float color[4];

	/* buffer�� �ʱ�ȭ�� color ���� */
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	/* Back buffer�� Depth buffer ����� */
	m_DeviceContext->ClearRenderTargetView(m_RenderTargetView, color);
	m_DeviceContext->ClearDepthStencilView(m_DepthStencilView, D3D11_CLEAR_DEPTH, 1.f, 0);
}

void D3DClass::EndScene()
{
	/* Back buffer�� ������ Output(ȭ��)�� ǥ�� */
	if (m_VSYNC_Enabled)
	{
		m_Swapchain->Present(1, 0);
	}
	else
	{
		m_Swapchain->Present(0, 0);
	}
}

ID3D11Device* D3DClass::GetDevice()
{
	return m_Device;
}

ID3D11DeviceContext* D3DClass::GetDeviceContext()
{
	return m_DeviceContext;
}

void D3DClass::GetProjectionMatrix(D3DXMATRIX& ProjectionMatrix)
{
	ProjectionMatrix = m_ProjectionMatrix;
}

void D3DClass::GetWorldMatrix(D3DXMATRIX& WorldMatrix)
{
	WorldMatrix = m_WorldMatrix;
}

void D3DClass::GetOrthoMatrix(D3DXMATRIX& OrthoMatrix)
{
	OrthoMatrix = m_OrthoMatrix;
}

void D3DClass::GetVideoCardIndo(char* VideoCardName, int& Memory)
{
	strcpy_s(VideoCardName, 128, m_VideoCardDescription);
	Memory = m_VideoCardMemory;
}