#include "SystemClass.h"

SystemClass::SystemClass() : 
	m_ApplicationName(nullptr),
	m_hinstance(NULL),
	m_hwnd(NULL),
	m_Graphics(nullptr),
	m_Input(nullptr)
{
}

SystemClass::SystemClass(const SystemClass&) :
	m_ApplicationName(nullptr),
	m_hinstance(NULL),
	m_hwnd(NULL),
	m_Graphics(nullptr),
	m_Input(nullptr)
{
}

/* 일부 WIN32 API는 destructor를 호출하지 않아 memory leak이 발생하기 때문에 Shutdown 함수에서 object를 정리한다.
	ex) ExitThread()
*/
SystemClass::~SystemClass() { }

bool SystemClass::Frame()
{
	bool result = false;

	/* Application 종료 작업 */
	if (m_Input->IsKeyDown(VK_ESCAPE))
	{
		return result;
	}

	/* Graphics 객체의 작업 처리 */
	result = m_Graphics->Frame();

	return result;
}

void SystemClass::InitializeWindows(int& ScreenWidth, int& ScreenHeight)
{
	WNDCLASSEX cex;				// 윈도우 클래스
	DEVMODE dmScreenSetting;		// 렌더링 화면
	int posX, posY;

	/* System class를 현재 객체와 연결 */
	ApplicationHandler = this;

	/* Application의 instance를 얻어오기 */
	m_hinstance = GetModuleHandle(NULL);

	/* Application의 이름 설정 */
	m_ApplicationName = L"Engine";

	/* 윈도우 클래스 초기화 */
	cex.cbSize = sizeof(WNDCLASSEX);
	cex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	cex.lpfnWndProc = WndProc;
	cex.cbClsExtra = 0;
	cex.cbWndExtra = 0;
	cex.hInstance = m_hinstance;
	cex.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	cex.hIconSm = cex.hIcon;
	cex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	cex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	cex.lpszMenuName = NULL;
	cex.lpszClassName = m_ApplicationName;

	/* 윈도우 클래스 등록 */
	RegisterClassEx(&cex);

	/* 모니터 해상도 얻어오기 */
	ScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	ScreenHeight = GetSystemMetrics(SM_CYSCREEN);

	/* 화면 모드에 따라 렌더링 화면 설정 */
	if (FULL_SCREEN)				// full-screen 모드
	{
		/* 디스플레이 설정 초기화 */
		memset(&dmScreenSetting, 0, sizeof(dmScreenSetting));
		dmScreenSetting.dmSize = sizeof(dmScreenSetting);
		dmScreenSetting.dmPelsWidth = (unsigned long)ScreenWidth;		// 렌더링 화면을 모니터 전체로 설정
		dmScreenSetting.dmPelsHeight = (unsigned long)ScreenHeight;
		dmScreenSetting.dmBitsPerPel = 32;								// 32bit Color 사용
		dmScreenSetting.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		/* 디스플레이 설정 */
		ChangeDisplaySettings(&dmScreenSetting, CDS_FULLSCREEN);

		/* 윈도우 위치 설정 */
		posX = 0;
		posY = 0;
	}
	else							// default 모드(800*600)
	{
		/* 렌더링 화면 크기 설정 */
		ScreenWidth = 800;
		ScreenHeight = 600;
			

		/* 윈도우 위치 설정 */
		posX = (GetSystemMetrics(SM_CXSCREEN) - ScreenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - ScreenHeight) / 2;
	}

	/* 윈도우 생성 */
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_ApplicationName, m_ApplicationName,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
		posX, posY, ScreenWidth, ScreenHeight, NULL, NULL, m_hinstance, NULL);

	/* 윈도우를 화면에 표시 및 포커싱 */
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	/* 마우스 커서 표시 여부 설정 */
	ShowCursor(false);
}

void SystemClass::ShutdownWindows()
{
	/* 마우스 커서 표시 여부 설정 */
	ShowCursor(true);

	/* full-screen 모드일 때의 디스플레이 설정 변경 */
	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	/* 윈도우 제거 */
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	/* Application의 instance 제거 */
	UnregisterClass(m_ApplicationName, m_hinstance);
	m_hinstance = NULL;

	/* System class와 연결 해제 */
	ApplicationHandler = nullptr;
}

bool SystemClass::Initialize()
{
	int ScreenWidth = 0, ScreenHeight = 0;		// screen size
	bool result = false;						// command execution result

	// WIN32 API를 이용해 screen size 초기화
	InitializeWindows(ScreenWidth, ScreenHeight);

	// Input 객체 생성 및 초기화
	m_Input = new InputClass;
	if (!m_Input)
	{
		return result;
	}
	m_Input->Initialize();

	// Graphics 객체 생성 및 초기화
	m_Graphics = new GraphicsClass;
	if (!m_Graphics)
	{
		return result;
	}
	result = m_Graphics->Initialize(ScreenWidth, ScreenHeight, m_hwnd);

	return result;
}

void SystemClass::Shutdown()
{
	/* 객체 정리 */
	if (m_Graphics)
	{
		m_Graphics->Shutdown();
		delete m_Graphics;
		m_Graphics = nullptr;
	}

	if (m_Input)
	{
		delete m_Input;
		m_Input = nullptr;
	}

	/* 윈도우 종료 */
	ShutdownWindows();
}

void SystemClass::Run()
{
	MSG msg;
	bool done = false, result = false;

	/* MSG 초기화 */
 	ZeroMemory(&msg, sizeof(msg));

	/* main loop */
	while (!done)
	{
		/* 윈도우 메세지 처리 */
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		/* Application 종료 message 처리(윈도우 메세지 처리) */
		if (msg.message == WM_QUIT)
		{
			done = true;
		}
		
		/* Application 작업 처리 */
		result = Frame();
		/* Application 종료 message 처리(Application 작업 처리) */
		if (!result)
		{
			done = true;
		}
	}
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//  WM_DESTROY  - post a quit message and return
//
LRESULT SystemClass::MessageHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		/* 키보드 입력 처리 */
		case WM_KEYDOWN:
		{
			m_Input->KeyDown((unsigned int)wParam);
			return 0;
		}
		case WM_KEYUP:
		{
			m_Input->KeyUp((unsigned int)wParam);
			return 0;
		}
		/* 윈도우 종료 메세지 처리 */
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		/* 나머지 메세지 처리 */
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT umessage, WPARAM wParam, LPARAM lParam)
{
	switch (umessage)
	{
		/* 윈도우 종료 메세지 처리 */
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		case WM_CLOSE:
			PostQuitMessage(0);
			return 0;

		/* 그 외 모든 메세지는 System class의 MessageHandler에서 처리 */
		default:
			return ApplicationHandler->MessageHandler(hWnd, umessage, wParam, lParam);
	}
}
