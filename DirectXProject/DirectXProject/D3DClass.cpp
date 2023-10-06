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

/* Prameter : 윈도우 해상도, 수직동기화 여부, 윈도우 핸들, 풀스크린 여부, 3D 환경의 Depth 값*/
bool D3DClass::Initialize(int ScreenWidth, int ScreenHeight, bool VSYNC, HWND hwnd, bool FullScreen, float ScreenDepth, float ScreenNear)
{
	/*
	초기화 과정
	1. 그래픽카드 정보를 받아온다.
	2. DirectX를 초기화 한다.
	   1) Swap chain 정보를 채우고, feature level을 설정한다.
	   2) Swap chain, Direct3D Device, Direct3D Device Context를 생성한 다음, Render Target View를 만든다.
	   3) Depth Stencil View를 생성한다.
	      과정 : Depth 정보를 채우기 -> Depth buffer 생성 -> Depth Stencil 정보를 채우기 -> Depth Stencil state 생성 및 설정
				 -> Depth Stencil view의 정보를 채우고 생성
	   4) Render Target View와 Depth Stencil View를 Rendering Pipeline에 바인딩
	   5) Rasterization state 생성 및 Rendering Pipeline에 바인딩
	   6) Viewport 생성
	   7) Matrix 설정 및 생성(World, Projection, View, Ortho)
	*/
	
	HRESULT result;

	/* 그래픽카드 정보를 얻기 위해 필요한 variable */
	IDXGIFactory* Factory = nullptr;													// 그래픽카드 정보를 얻기 위한 interface
	IDXGIAdapter* Adapter = nullptr;													// 그래픽카드
	IDXGIOutput* AdapterOutput = nullptr;												// output(모니터)
	unsigned int DisplayModeNumbers = 0, i = 0, Numerator = 0, Denominator = 0, StringLength = 0;
	DXGI_MODE_DESC* DisplayModeList = nullptr;											// 디스플레이 모드 리스트
	DXGI_ADAPTER_DESC AdapterDescription;												// 그래픽카드 Description
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

	/* 수직동기화 설정 */
	m_VSYNC_Enabled = VSYNC;

	/* -------------------- 그래픽카드 정보 얻어 오기 -------------------- */
	/* DirectX 그래픽카드 인터페이스 Factory 생성(그래픽카드 pointer 받아오기) */
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&Factory);
	if (FAILED(result))
	{
		return false;
	}

	/* 팩토리 객체를 이용해 첫번째 그래픽 카드 인터페이스에 대한 Adapter 생성 */
	result = Factory->EnumAdapters(0, &Adapter);
	if (FAILED(result))
	{
		return false;
	}

	/* 첫 번째 Adapter에 연결된 output(모니터) 나열 */
	result = Adapter->EnumOutputs(0, &AdapterOutput);
	if (FAILED(result))
	{
		return false;
	}

	/* DSGI_FORMAT_R8G8B8A8_UNORM 모니터 출력 디스플레이 포맷에 맞는 디스플레이 모드의 개수 구하기 */
	result = AdapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &DisplayModeNumbers, NULL);
	if (FAILED(result))
	{
		return false;
	}

	/* 가능한 모든 모니터와 그래픽카드 조합을 저장할 리스트(디스플레이 모드 리스트) 생성 */
	DisplayModeList = new DXGI_MODE_DESC[DisplayModeNumbers];
	if (!DisplayModeList)
	{
		return false;
	}

	/* 디스플레이 모드 리스트 채워 넣기 */
	result = AdapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &DisplayModeNumbers, DisplayModeList);

	/* 모든 디스플레이 모드 중 화면 너비/높이에 맞는 디스플레이 모드를 찾고,
	   적합한 것을 찾으면 새로고침 비율의 분모와 분자의 값을 저장 */
	for (i = 0; i < DisplayModeNumbers; i++)
	{
		if (DisplayModeList[i].Width == (unsigned int)ScreenWidth &&
			DisplayModeList[i].Height == (unsigned int)ScreenHeight)
		{
			Numerator = DisplayModeList[i].RefreshRate.Numerator;
			Denominator = DisplayModeList[i].RefreshRate.Denominator;
		}
	}

	/* Adapter(그래픽카드)의 Description 받아오기 */
	result = Adapter->GetDesc(&AdapterDescription);
	if (!DisplayModeList)
	{
		return false;
	}

	/* 현재 그래픽카드의 메모리 용량을 MB 단위로 저장 */
	m_VideoCardMemory = (int)(AdapterDescription.DedicatedVideoMemory / 1024 / 1024);

	/* 현재 그래픽카드의 이름을 char형 문자열 배열로 바뀐 뒤 저장 */
	error = wcstombs_s(&StringLength, m_VideoCardDescription, 128, AdapterDescription.Description, 128);
	if (error)
	{
		return false;
	}

	/* 디스플레이 모드 리스트, AdapterOutput 객체, Adapter 객체, Factory 객체 해제 */
	delete[] DisplayModeList;
	DisplayModeList = nullptr;

	AdapterOutput->Release();
	AdapterOutput = nullptr;

	Adapter->Release();
	Adapter = nullptr;

	Factory->Release();
	Factory = nullptr;
	/* ---------------------------------------- */

	/* -------------------- DirectX 초기화 -------------------- */
	/* ---------- Swap Chain Description 작성 및 Feature Level 설정 ---------- */
	/* Swap Chain의 Description 초기화 */
	ZeroMemory(&SwapchainDescription, sizeof(SwapchainDescription));

	/* back buffer 개수 설정 */
	SwapchainDescription.BufferCount = 1;

	/* back buffer 설정 */
	SwapchainDescription.BufferDesc.Width = ScreenWidth;						// 화면 너비/높이
	SwapchainDescription.BufferDesc.Height = ScreenHeight;
	SwapchainDescription.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;		// 포맷(일반적인 32bit의 surface로 설정)
	SwapchainDescription.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;			// 용도

	/* Swap Chain 주사율 설정(화면 주사율) */
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

	/* Rendering할 윈도우 설정 */
	SwapchainDescription.OutputWindow = hwnd;

	/* 멀티샘플링 설정(현재는 off) */
	SwapchainDescription.SampleDesc.Count = 1;
	SwapchainDescription.SampleDesc.Quality = 0;

	/* 풀스크린 모드 또는 윈도우 모드 설정 */
	if (FullScreen)
	{
		SwapchainDescription.Windowed = false;
	}
	else
	{
		SwapchainDescription.Windowed = true;
	}

	/* 스캔라인 정렬, 스캔라이닝을 지정되지 않음(unspecified)로 설정 */
	SwapchainDescription.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	SwapchainDescription.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	/* Swap Chain 이후 back buffer의 내용을 지우도록 설정 */
	SwapchainDescription.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	/* 추가 옵션 플래스 설정 */
	SwapchainDescription.Flags = 0;												// 아무 것도 설정하지 않음

	/* Feature Level(DirectX 버전) 설정 */
	FeatureLevel = D3D_FEATURE_LEVEL_11_0;
	/* -------------------- */

	/* ---------- Swap chain, Direct3D Device, Direct3D Device Context를 생성하고, Render Target View를 만든다. ---------- */
	/* Swap chain, Direct3D Device, Direct3D Device Context 생성 */
	result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &FeatureLevel, 1,
		D3D11_SDK_VERSION, &SwapchainDescription, &m_Swapchain, &m_Device, NULL, &m_DeviceContext);
	if (FAILED(result))
	{
		return false;
	}

	/* back buffer의 포인터를 가져오고, back buffer의 포인터를 이용해 Render Target View 생성 */
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

	/* back buffer 포인터 할당 해제 */
	BackBufferPointer->Release();
	BackBufferPointer = nullptr;
	/* -------------------- */

	/* ---------- Depth Stencil View를 생성한다. ---------- */
	/* Depth buffer Description 초기화 및 작성 */
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

	/* Depth Stencil buffer 생성 */
	result = m_Device->CreateTexture2D(&DepthBufferDescription, NULL, &m_DepthStencilBuffer);
	if (FAILED(result))
	{
		return false;
	}

	/* Depth Stencil Description 초기화 및 작성 */
	ZeroMemory(&DepthStencilDescription, sizeof(DepthStencilDescription));

	DepthStencilDescription.DepthEnable = true;
	DepthStencilDescription.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	DepthStencilDescription.DepthFunc = D3D11_COMPARISON_LESS;
	DepthStencilDescription.StencilEnable = true;
	DepthStencilDescription.StencilReadMask = 0xFF;
	DepthStencilDescription.StencilWriteMask = 0xFF;

	// pixel이 front-facing으로 된 경우, Stencil operations
	DepthStencilDescription.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	DepthStencilDescription.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	DepthStencilDescription.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	DepthStencilDescription.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// pixel이 back-facing으로 된 경우, Stencil operations
	DepthStencilDescription.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	DepthStencilDescription.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	DepthStencilDescription.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	DepthStencilDescription.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	/* Depth Stencil state 생성 */
	result = m_Device->CreateDepthStencilState(&DepthStencilDescription, &m_DepthStencilState);
	if (FAILED(result))
	{
		return false;
	}

	/* Depth Stencil state 설정 */
	m_DeviceContext->OMSetDepthStencilState(m_DepthStencilState, 1);

	/* Depth Stencil view의 Description 초기화 및 작성 */
	ZeroMemory(&DepthStencilViewDescription, sizeof(DepthStencilViewDescription));

	DepthStencilViewDescription.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	DepthStencilViewDescription.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	DepthStencilViewDescription.Texture2D.MipSlice = 0;

	/* Depth Stencil view 생성 */
	result = m_Device->CreateDepthStencilView(m_DepthStencilBuffer, &DepthStencilViewDescription, &m_DepthStencilView);
	if (FAILED(result))
	{
		return false;
	}
	/* -------------------- */

	/* ---------- Render Target View와 Depth Stencil View를 Rendering Pipeline에 바인딩 ---------- */
	m_DeviceContext->OMSetRenderTargets(1, &m_RenderTargetView, m_DepthStencilView);

	/* ---------- Rasterization state 생성 및 Rendering Pipeline에 바인딩 ---------- */
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
	
	/* ---------- Viewport 생성 ---------- */
	Viewport.Width = (float)ScreenWidth;
	Viewport.Height = (float)ScreenHeight;
	Viewport.MinDepth = 0.f;
	Viewport.MaxDepth = 1.f;
	Viewport.TopLeftX = 0.f;
	Viewport.TopLeftY = 0.f;

	m_DeviceContext->RSSetViewports(1, &Viewport);

	/* ---------- Matrix 설정 및 생성(World, Projection, View, Ortho) ---------- */
	// Projection Matrix 설정 및 생성
	FieldOfView = (float)D3DX_PI / 4.f;
	ScreenAspect = (float)ScreenWidth / (float)ScreenHeight;

	D3DXMatrixPerspectiveFovLH(&m_ProjectionMatrix, FieldOfView, ScreenAspect, ScreenNear, ScreenDepth);

	// World Matrix를 단위 행렬로 초기화
	D3DXMatrixIdentity(&m_WorldMatrix);

	// View Matrix(Camera Matrix) 생성 -> Camera Class에서 다룰 예정

	// Ortho Matrix 생성(UI같은 2D 요소를 Rendering할 때 사용하는 Matrix)
	D3DXMatrixOrthoLH(&m_OrthoMatrix, (float)ScreenWidth, (float)ScreenHeight, ScreenNear, ScreenDepth);
	/* -------------------- */
	/* ---------------------------------------- */

	return true;
}

void D3DClass::Shutdown()
{
	/* 종료하기 전에 윈도우 모드로 바꾸기 */
	/* -> Full Screen 모드에서 할당 해제하는 경우, 몇몇 예외가 발생하기 때문에, 윈도우 모드로 바꾸고 모든 Direct 3D 객체를 해제한다. */
	if (m_Swapchain)
	{
		m_Swapchain->SetFullscreenState(false, NULL);
	}

	/* 모든 Direct 3D 객체 해제 */
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

	/* buffer를 초기화할 color 설정 */
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	/* Back buffer와 Depth buffer 지우기 */
	m_DeviceContext->ClearRenderTargetView(m_RenderTargetView, color);
	m_DeviceContext->ClearDepthStencilView(m_DepthStencilView, D3D11_CLEAR_DEPTH, 1.f, 0);
}

void D3DClass::EndScene()
{
	/* Back buffer의 내용을 Output(화면)에 표시 */
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