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

/* �Ϻ� WIN32 API�� destructor�� ȣ������ �ʾ� memory leak�� �߻��ϱ� ������ Shutdown �Լ����� object�� �����Ѵ�.
	ex) ExitThread()
*/
SystemClass::~SystemClass() { }

bool SystemClass::Frame()
{
	bool result = false;

	/* Application ���� �۾� */
	if (m_Input->IsKeyDown(VK_ESCAPE))
	{
		return result;
	}

	/* Graphics ��ü�� �۾� ó�� */
	result = m_Graphics->Frame();

	return result;
}

void SystemClass::InitializeWindows(int& ScreenWidth, int& ScreenHeight)
{
	WNDCLASSEX cex;				// ������ Ŭ����
	DEVMODE dmScreenSetting;		// ������ ȭ��
	int posX, posY;

	/* System class�� ���� ��ü�� ���� */
	ApplicationHandler = this;

	/* Application�� instance�� ������ */
	m_hinstance = GetModuleHandle(NULL);

	/* Application�� �̸� ���� */
	m_ApplicationName = L"Engine";

	/* ������ Ŭ���� �ʱ�ȭ */
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

	/* ������ Ŭ���� ��� */
	RegisterClassEx(&cex);

	/* ����� �ػ� ������ */
	ScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	ScreenHeight = GetSystemMetrics(SM_CYSCREEN);

	/* ȭ�� ��忡 ���� ������ ȭ�� ���� */
	if (FULL_SCREEN)				// full-screen ���
	{
		/* ���÷��� ���� �ʱ�ȭ */
		memset(&dmScreenSetting, 0, sizeof(dmScreenSetting));
		dmScreenSetting.dmSize = sizeof(dmScreenSetting);
		dmScreenSetting.dmPelsWidth = (unsigned long)ScreenWidth;		// ������ ȭ���� ����� ��ü�� ����
		dmScreenSetting.dmPelsHeight = (unsigned long)ScreenHeight;
		dmScreenSetting.dmBitsPerPel = 32;								// 32bit Color ���
		dmScreenSetting.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		/* ���÷��� ���� */
		ChangeDisplaySettings(&dmScreenSetting, CDS_FULLSCREEN);

		/* ������ ��ġ ���� */
		posX = 0;
		posY = 0;
	}
	else							// default ���(800*600)
	{
		/* ������ ȭ�� ũ�� ���� */
		ScreenWidth = 800;
		ScreenHeight = 600;
			

		/* ������ ��ġ ���� */
		posX = (GetSystemMetrics(SM_CXSCREEN) - ScreenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - ScreenHeight) / 2;
	}

	/* ������ ���� */
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_ApplicationName, m_ApplicationName,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
		posX, posY, ScreenWidth, ScreenHeight, NULL, NULL, m_hinstance, NULL);

	/* �����츦 ȭ�鿡 ǥ�� �� ��Ŀ�� */
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	/* ���콺 Ŀ�� ǥ�� ���� ���� */
	ShowCursor(false);
}

void SystemClass::ShutdownWindows()
{
	/* ���콺 Ŀ�� ǥ�� ���� ���� */
	ShowCursor(true);

	/* full-screen ����� ���� ���÷��� ���� ���� */
	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	/* ������ ���� */
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	/* Application�� instance ���� */
	UnregisterClass(m_ApplicationName, m_hinstance);
	m_hinstance = NULL;

	/* System class�� ���� ���� */
	ApplicationHandler = nullptr;
}

bool SystemClass::Initialize()
{
	int ScreenWidth = 0, ScreenHeight = 0;		// screen size
	bool result = false;						// command execution result

	// WIN32 API�� �̿��� screen size �ʱ�ȭ
	InitializeWindows(ScreenWidth, ScreenHeight);

	// Input ��ü ���� �� �ʱ�ȭ
	m_Input = new InputClass;
	if (!m_Input)
	{
		return result;
	}
	m_Input->Initialize();

	// Graphics ��ü ���� �� �ʱ�ȭ
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
	/* ��ü ���� */
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

	/* ������ ���� */
	ShutdownWindows();
}

void SystemClass::Run()
{
	MSG msg;
	bool done = false, result = false;

	/* MSG �ʱ�ȭ */
 	ZeroMemory(&msg, sizeof(msg));

	/* main loop */
	while (!done)
	{
		/* ������ �޼��� ó�� */
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		/* Application ���� message ó��(������ �޼��� ó��) */
		if (msg.message == WM_QUIT)
		{
			done = true;
		}
		
		/* Application �۾� ó�� */
		result = Frame();
		/* Application ���� message ó��(Application �۾� ó��) */
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
		/* Ű���� �Է� ó�� */
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
		/* ������ ���� �޼��� ó�� */
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		/* ������ �޼��� ó�� */
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT umessage, WPARAM wParam, LPARAM lParam)
{
	switch (umessage)
	{
		/* ������ ���� �޼��� ó�� */
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		case WM_CLOSE:
			PostQuitMessage(0);
			return 0;

		/* �� �� ��� �޼����� System class�� MessageHandler���� ó�� */
		default:
			return ApplicationHandler->MessageHandler(hWnd, umessage, wParam, lParam);
	}
}
